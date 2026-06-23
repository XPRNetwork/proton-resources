<template>
  <div class="pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="ui-title-1 sm:text-center">Buy Blockchain Storage</h1>
        <div class="ui-text mt-5 sm:text-center">
          <p>
            Blockchain Storage (RAM) allows you to store data on the blockchain such as NFTs,
            marketplace sales and tokens.
          </p>
        </div>
      </div>

      <div class="flex justify-center mt-6 sm:p-6">
        <div
          class="max-w-xl mt-5 bg-white shadow sm:rounded-lg px-6 py-5 sm:flex sm:items-start sm:justify-between"
        >
          <h4 class="sr-only">RAM</h4>
          <div class="sm:flex sm:items-start">
            <div class="mt-3 sm:mt-0 sm:ml-4">
              <div class="font-medium text-neutral-900">
                Account Usage:
                <span v-if="account">{{ account.ram_usage }} / {{ account.ram_quota }} bytes</span>
              </div>
              <div class="mt-1 text-sm text-neutral-600 sm:flex sm:items-center">
                <div>Note: WebAuth provides 12,000 unsellable bytes to every account</div>
              </div>

              <div class="mt-6 sm:items-center">
                <label for="buybytes" class="form-field-label mb-2 block"
                  >Buy Storage Bytes ({{ ram_price_per_byte }} XPR / byte):
                </label>

                <div class="mt-2 flex">
                  <div class="form-field w-full sm:max-w-xs">
                    <input
                      type="text"
                      name="buybytes"
                      id="buybytes"
                      v-model="buyRamBytes"
                      :placeholder="`Maximum ${account ? max_per_user_bytes - ram : max_per_user_bytes}`"
                      class="form-field__input"
                    />
                  </div>
                  <button
                    type="button"
                    class="ui-button ui-is-primary w-full sm:ml-3 sm:w-auto"
                    @click="buyRam"
                  >
                    Buy
                  </button>
                </div>

                <div v-if="buyRamBytes" class="mt-2 px-3 text-sm">
                  = {{ (ram_price_per_byte * buyRamBytes).toFixed(4) }} XPR
                </div>
              </div>

              <div v-if="ram" class="mt-5 sm:items-center">
                <label for="sellbytes" class="form-field-label mb-2 block"
                  >Sell Storage Bytes ({{ ram_price_per_byte_without_fee }} XPR / byte):
                </label>
                <div class="mt-2 flex">
                  <div class="form-field w-full sm:max-w-xs">
                    <input
                      type="text"
                      name="sellbytes"
                      id="sellbytes"
                      v-model="sellRamBytes"
                      :placeholder="`Maximum ${account ? Math.min(ram, +account.ram_quota - +account.ram_usage) : max_per_user_bytes}`"
                      class="form-field__input"
                    />
                  </div>
                  <button
                    type="button"
                    class="ui-button ui-is-primary w-full sm:ml-3 sm:w-auto"
                    @click="sellRam"
                  >
                    Sell
                  </button>
                </div>

                <div v-if="sellRamBytes" class="mt-2 px-3 text-sm">
                  = {{ (ram_price_per_byte_without_fee * sellRamBytes).toFixed(4) }} XPR
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { rpc } from '@/api/user'
import { getRamPrice } from '@/api/ram'
import { computed, onMounted, ref, watch } from 'vue'
import { useUserStore } from '@/stores/user'
import type { RpcInterfaces } from '@proton/js'

const userStore = useUserStore()

const buyRamBytes = ref(0)
const sellRamBytes = ref(0)
const account = ref<RpcInterfaces.GetAccountResult | undefined>(undefined)
const ram = ref(0)
const quantity = ref(0)
const ramlimit = ref(0)
const ram_price_per_byte = ref(0)
const ram_price_per_byte_without_fee = ref(0)
const max_per_user_bytes = ref(0)
const ram_fee_percent = ref(0)

const actor = computed(() => userStore.actor)

const login = userStore.login
const transact = userStore.transact

const updateUserRam = async () => {
  if (actor.value) {
    account.value = await rpc.get_account(actor.value)
    const user = await rpc.get_table_rows({
      code: 'eosio',
      scope: actor.value,
      table: 'usersram',
      limit: 1,
    })

    if (user && user.rows && user.rows.length) {
      ram.value = user.rows[0].ram
      quantity.value = user.rows[0].quantity.split(' ')[0]
      ramlimit.value = user.rows[0].ramlimit
    }
  } else {
    account.value = undefined
    ram.value = 0
    quantity.value = 0
    ramlimit.value = 0
    ram_price_per_byte_without_fee.value = 0
  }
}

const updateRamPrice = async () => {
  const stats = await getRamPrice()

  if (stats) {
    max_per_user_bytes.value = stats.max_per_user_bytes
    ram_fee_percent.value = stats.ram_fee_percent
    ram_price_per_byte_without_fee.value = stats.ram_price_per_byte_without_fee
    ram_price_per_byte.value = stats.ram_price_per_byte
  }
}

const buyRam = async () => {
  if (!actor.value) {
    await login()
    if (!actor.value) {
      alert('Could not find user')
      return
    } else {
      setTimeout(() => buyRam(), 2000)
      return
    }
  }

  const actions = [
    {
      account: 'eosio',
      name: 'buyrambytes',
      data: {
        payer: actor.value,
        receiver: actor.value,
        bytes: buyRamBytes.value,
      },
    },
  ]

  await transact({ actions })
  setTimeout(() => updateUserRam(), 1000)
  buyRamBytes.value = 0
}

const sellRam = async () => {
  if (!actor.value) {
    await login()
    if (!actor.value) {
      alert('Could not find user')
      return
    } else {
      setTimeout(() => sellRam(), 2000)
      return
    }
  }

  const actions = [
    {
      account: 'eosio',
      name: 'sellram',
      data: {
        account: actor.value,
        bytes: sellRamBytes.value,
      },
    },
  ]

  await transact({ actions })
  setTimeout(() => updateUserRam(), 1000)
  sellRamBytes.value = 0
}

watch(
  actor,
  () => {
    updateUserRam()
  },
  {
    immediate: true,
  },
)

onMounted(() => {
  updateRamPrice()
})
</script>

<style></style>
