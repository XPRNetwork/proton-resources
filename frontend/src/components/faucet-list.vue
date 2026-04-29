<template>
  <div class="bg-linear-to-b from-white to-gray-50 pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="text-5xl font-extrabold text-gray-900 sm:text-center">Faucets</h1>
        <p class="mt-5 text-xl text-gray-500 sm:text-center">
          Free faucets on XPR Network. <br />
          For full list of tokens, contracts and precisions, check
          <a href="https://explorer.xprnetwork.org/tokens" target="_blank" class="text-purple-600"
            >XPR Network Explorer</a
          >
        </p>
      </div>

      <div class="flex justify-center mt-6 sm:p-6">
        <div class="flex flex-col overflow-x-scroll overflow-y-hidden">
          <div class="-my-2 sm:-mx-6 lg:-mx-8">
            <div class="py-2 align-middle inline-block min-w-full sm:px-6 lg:px-8">
              <div class="shadow border-b border-gray-200 sm:rounded-lg">
                <table class="min-w-full divide-y divide-gray-200">
                  <thead class="bg-gray-50">
                    <tr>
                      <th
                        scope="col"
                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider"
                      >
                        Token
                      </th>
                      <th
                        scope="col"
                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider"
                      >
                        Contract
                      </th>
                      <th
                        scope="col"
                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider"
                      >
                        In faucet
                      </th>
                      <th
                        scope="col"
                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider"
                      >
                        Can claim
                      </th>
                      <th
                        scope="col"
                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider"
                      >
                        Claim timer
                      </th>
                      <th scope="col" class="relative px-6 py-3">
                        <span class="sr-only">CLAIM</span>
                      </th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr
                      v-for="(faucet, faucetIdx) in faucets"
                      :key="faucet.index"
                      :class="faucetIdx % 2 === 0 ? 'bg-white' : 'bg-gray-50'"
                    >
                      <td class="px-6 py-4 whitespace-nowrap text-sm font-medium text-gray-900">
                        {{ faucet.savedToken.quantity.split(' ')[1] }}
                      </td>
                      <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                        {{ faucet.savedToken.contract }}
                      </td>
                      <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                        {{ faucet.savedToken.quantity }}
                      </td>
                      <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                        {{ faucet.claimToken.quantity }}
                      </td>
                      <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                        {{ parseSeconds(faucet.duration) }}
                      </td>
                      <td class="px-6 py-4 whitespace-nowrap text-right text-sm font-medium">
                        <template v-if="actor">
                          <span
                            v-if="
                              account &&
                              account.expiryByProgramProcessed &&
                              now < (account.expiryByProgramProcessed.get(faucet.index) ?? 0)
                            "
                          >
                            Claim in
                            {{
                              parseSeconds(
                                (account.expiryByProgramProcessed.get(faucet.index) ?? 0) - now,
                              )
                            }}
                          </span>
                          <button
                            v-else
                            @click="claim(faucet.index)"
                            type="button"
                            class="cursor-pointer w-full inline-flex items-center justify-center px-4 py-2 border border-transparent shadow-sm font-medium rounded-md text-white bg-purple-600 hover:bg-purple-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500 sm:ml-3 sm:w-auto sm:text-sm"
                          >
                            Claim
                          </button>
                        </template>
                      </td>
                    </tr>
                  </tbody>
                </table>
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
import { useUserStore } from '@/stores/user'
import type { IAccountFaucet, IFaucetProgram } from '@/types'
import { computed, onMounted, ref, watch } from 'vue'

const userStore = useUserStore()

const account = ref<IAccountFaucet | undefined>(undefined)
const faucets = ref<IFaucetProgram[]>([])

const getNow = () => Math.floor(new Date().getTime() / 1000)

const now = ref(getNow())

const actor = computed(() => userStore.actor)

const login = userStore.login
const transact = userStore.transact

const parseSeconds = (s: number, withSeconds = true) => {
  const hours = Math.floor(s / 3600)
  const minutes = Math.floor((s - hours * 3600) / 60)
  const seconds = s - hours * 3600 - minutes * 60

  let text = ''
  if (hours > 0) {
    text += ` ${hours}h`
  }
  if (minutes > 0) {
    text += ` ${minutes}m`
  }
  if (seconds > 0 && withSeconds) {
    text += ` ${seconds}s`
  }

  return text
}

const fetchFaucets = async () => {
  const { rows } = await rpc.get_table_rows<IFaucetProgram>({
    code: 'token.faucet',
    scope: 'token.faucet',
    table: 'programs',
    limit: -1,
  })
  faucets.value = rows
}

const fetchAccount = async () => {
  if (actor.value) {
    const { rows } = await rpc.get_table_rows<IAccountFaucet>({
      code: 'token.faucet',
      scope: 'token.faucet',
      table: 'accounts',
      lower_bound: actor.value,
      limit: 1,
    })

    if (rows && rows.length && rows[0] && rows[0].account === actor.value) {
      rows[0].expiryByProgramProcessed = rows[0].expiryByProgram.reduce((acc, { key, value }) => {
        acc.set(key, value)
        return acc
      }, new Map<number, number>())
      account.value = rows[0]
    }
  } else {
    account.value = undefined
  }
}

const claim = async (programId: IFaucetProgram['index']) => {
  if (!actor.value) {
    await login()
    if (!actor.value) {
      alert('Could not find user')
      return
    }
  }

  const actions = [
    {
      account: 'token.faucet',
      name: 'claim',
      data: {
        programId: programId,
        account: actor.value,
      },
    },
  ]

  await transact({ actions })

  setTimeout(() => {
    fetchAccount()
    fetchFaucets()
  }, 1500)
}

watch(
  actor,
  () => {
    fetchAccount()
  },
  {
    immediate: true,
  },
)

onMounted(() => {
  fetchFaucets()
  setInterval(() => {
    now.value = getNow()
  }, 1000)
})
</script>

<style></style>
