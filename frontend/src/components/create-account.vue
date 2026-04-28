<template>
  <div class="bg-linear-to-b from-white to-gray-50 pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="text-5xl font-extrabold text-gray-900 sm:text-center">Create account</h1>
        <p class="mt-5 text-xl text-gray-500 sm:text-center">
          Create a new account with your own custom keys
        </p>
      </div>

      <div class="flex justify-center mt-6 py-6">
        <div
          class="max-w-xl mt-5 bg-white shadow sm:rounded-lg px-6 py-5 sm:flex sm:items-start sm:justify-between"
        >
          <div class="w-full">
            <div class="font-medium text-gray-900">
              Can create:
              <span v-if="balance !== undefined && costOfAccountInXpr">
                {{ Math.floor(balance / costOfAccountInXpr) }} accounts
              </span>
              <span v-else> Please login </span>
            </div>
            <div class="mt-1 pr-16 text-sm text-gray-600 sm:flex sm:items-center">
              <div>
                Note: Non-vanity names cost {{ costOfAccountInXpr.toFixed(2) }} XPR to create.
                Vanity (.xpr, .btc, etc) names cost 50 XPR to create.
              </div>
            </div>

            <div class="mt-8 flex flex-col space-y-4">
              <div class="w-full">
                Account Name:
                <input
                  type="text"
                  name="buybytes"
                  class="shadow-sm focus:ring-purple-500 focus:border-purple-500 block w-full sm:text-sm border-gray-300 rounded-md"
                  v-model="newAccountName"
                  :placeholder="`Account Name (a-z, 1-5, min 4 chars)`"
                />
              </div>

              <div class="w-full">
                Account Public Key:
                <input
                  type="text"
                  name="buybytes"
                  class="shadow-sm focus:ring-purple-500 focus:border-purple-500 block w-full sm:text-sm border-gray-300 rounded-md"
                  v-model="newAccountOwnerKey"
                  :placeholder="`Account Key (Starting with PUB_K1)`"
                />
              </div>

              <button
                type="button"
                class="cursor-pointer w-full inline-flex items-center justify-center px-4 py-2 border border-transparent shadow-sm font-medium rounded-md text-white bg-purple-600 hover:bg-purple-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500sm:w-auto sm:text-sm"
                @click="createAccount"
              >
                Create Account
              </button>
            </div>

            <div v-if="accountError" class="mt-2 text-red-600">
              {{ accountError }}
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { computed, onMounted, ref, watch } from 'vue'
import { rpc } from '@/api/user'
import { getRamPrice } from '@/api/ram'
import { useUserStore } from '@/stores/user'

const userStore = useUserStore()
const balance = ref<number>(0)
const newAccountName = ref('')
const newAccountOwnerKey = ref('')
const accountError = ref('')
const ramPerAccount = ref(12000)
const ram_price_per_byte = ref(0)
const ram_price_per_byte_without_fee = ref(0)
const max_per_user_bytes = ref(0)
const ram_fee_percent = ref(0)

const actor = computed(() => userStore.actor)

const costOfAccountInXpr = computed(() => ram_price_per_byte.value * ramPerAccount.value)

const login = userStore.login
const transact = userStore.transact

const fetchBalance = async () => {
  if (actor.value) {
    const [balanceSource] = await rpc.get_currency_balance('eosio.token', actor.value, 'XPR')
    if (balanceSource) {
      balance.value = +(balanceSource.split(' ')[0] ?? 0)
    } else {
      balance.value = 0
    }
  } else {
    balance.value = 0
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

const createAccount = async () => {
  if (!actor.value) {
    await login()
    if (!actor.value) {
      alert('Could not find user')
      return
    } else {
      setTimeout(() => createAccount(), 2000)
      return
    }
  }

  const creator = actor.value
  const newAccountNameV = newAccountName.value.trim().toLowerCase()
  const newAccountOwnerKeyV = newAccountOwnerKey.value.trim()
  const ramPerAccountV = ramPerAccount.value

  if (!newAccountNameV) {
    accountError.value = 'Please enter a new account name'
    return
  }

  if (newAccountNameV.length > 12) {
    accountError.value = 'Account names can be maximum 12 characters + numbers long'
    return
  }

  if (!RegExp('^[a-zA-Z12345.]+$').test(newAccountNameV)) {
    accountError.value = 'Account names can only contain the characters a-z and number 1-5.'
    return
  }

  if (!newAccountOwnerKeyV) {
    accountError.value = 'Please enter a new account key'
    return
  }

  const newAccountData = {
    creator: creator,
    name: newAccountNameV,
    owner: {
      threshold: 1,
      keys: [
        {
          key: newAccountOwnerKeyV,
          weight: 1,
        },
      ],
      accounts: [],
      waits: [],
    },
    active: {
      threshold: 1,
      keys: [
        {
          key: newAccountOwnerKeyV,
          weight: 1,
        },
      ],
      accounts: [],
      waits: [],
    },
  }

  const suffix = newAccountNameV.indexOf('.') !== -1 && newAccountNameV.split('.')[1]
  const actions = suffix
    ? [
        {
          account: 'eosio.token',
          name: 'transfer',
          data: {
            from: creator,
            to: suffix,
            quantity: '50.0000 XPR',
            memo: '',
          },
        },
        {
          account: suffix,
          name: 'newaccount',
          data: newAccountData,
        },
      ]
    : [
        {
          account: 'eosio',
          name: 'newaccount',
          data: newAccountData,
        },
        {
          account: 'eosio',
          name: 'buyrambytes',
          data: {
            payer: creator,
            receiver: newAccountNameV,
            bytes: ramPerAccountV,
          },
        },
        {
          account: 'eosio.proton',
          name: 'newaccres',
          data: {
            account: newAccountNameV,
          },
        },
      ]

  await transact({ actions })
  setTimeout(() => fetchBalance(), 1500)
  newAccountName.value = ''
  newAccountOwnerKey.value = ''
  accountError.value = ''
}

watch(
  actor,
  () => {
    fetchBalance()
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
