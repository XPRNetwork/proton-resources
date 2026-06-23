<template>
  <div class="pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <h1 class="ui-title-1 sm:text-center">Faucets</h1>
      <div class="ui-text mt-5 sm:text-center">
        <p>
          Free faucets on XPR Network. <br />
          For full list of tokens, contracts and precisions, check
          <a href="https://explorer.xprnetwork.org/tokens" target="_blank" class="ui-link">
            XPR Network Explorer
          </a>
        </p>
      </div>

      <div class="sm:px-6 lg:px-8 min-w-full">
        <div class="mt-6 overflow-x-scroll min-w-0 w-full">
          <table class="min-w-full divide-y divide-neutral-200">
            <thead class="bg-neutral-50">
              <tr>
                <th
                  scope="col"
                  class="px-6 py-3 text-left text-xs font-medium text-neutral-500 uppercase tracking-wider"
                >
                  Token
                </th>
                <th
                  scope="col"
                  class="px-6 py-3 text-left text-xs font-medium text-neutral-500 uppercase tracking-wider"
                >
                  Contract
                </th>
                <th
                  scope="col"
                  class="px-6 py-3 text-left text-xs font-medium text-neutral-500 uppercase tracking-wider"
                >
                  In faucet
                </th>
                <th
                  scope="col"
                  class="px-6 py-3 text-left text-xs font-medium text-neutral-500 uppercase tracking-wider"
                >
                  Can claim
                </th>
                <th
                  scope="col"
                  class="px-6 py-3 text-left text-xs font-medium text-neutral-500 uppercase tracking-wider"
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
                :class="faucetIdx % 2 === 0 ? 'bg-white' : 'bg-neutral-50'"
              >
                <td class="px-6 py-4 whitespace-nowrap text-sm font-medium text-neutral-900">
                  {{ faucet.savedToken.quantity.split(' ')[1] }}
                </td>
                <td class="px-6 py-4 whitespace-nowrap text-sm text-neutral-500">
                  {{ faucet.savedToken.contract }}
                </td>
                <td class="px-6 py-4 whitespace-nowrap text-sm text-neutral-500">
                  {{ faucet.savedToken.quantity }}
                </td>
                <td class="px-6 py-4 whitespace-nowrap text-sm text-neutral-500">
                  {{ faucet.claimToken.quantity }}
                </td>
                <td class="px-6 py-4 whitespace-nowrap text-sm text-neutral-500">
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
                      class="ui-button ui-is-primary w-full sm:w-auto"
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
</template>

<script lang="ts" setup>
import { rpc } from '@/api/user'
import { useUserStore } from '@/stores/user'
import type { IAccountFaucet, IFaucetProgram } from '@/types'
import { useIntervalFn, useTimeoutFn } from '@vueuse/core'
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

const { start: doFetch } = useTimeoutFn(
  () => {
    fetchAccount()
    fetchFaucets()
  },
  1_500,
  {
    immediate: false,
  },
)

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

  doFetch()
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

useIntervalFn(() => {
  now.value = getNow()
}, 1_000)

onMounted(() => {
  fetchFaucets()
})
</script>

<style></style>
