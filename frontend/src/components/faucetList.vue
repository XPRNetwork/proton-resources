<template>
  <div class="bg-gradient-to-b from-white to-gray-50 pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="text-5xl font-extrabold text-gray-900 sm:text-center">
          Faucets
        </h1>
        <p class="mt-5 text-xl text-gray-500 sm:text-center">
          Free faucets on Proton Chain. <br/>
          For full list of tokens, contracts and precisions, check <a href="https://protonscan.io/tokens" target="_blank" class="text-purple-600">ProtonScan</a>
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
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                        Token
                      </th>
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                        Contract
                      </th>
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                        In faucet
                      </th>
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                        Can claim
                      </th>
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                        Claim timer
                      </th>
                      <th scope="col" class="relative px-6 py-3">
                        <span class="sr-only">CLAIM</span>
                      </th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr v-for="(faucet, faucetIdx) in faucets" :key="faucet.index" :class="faucetIdx % 2 === 0 ? 'bg-white' : 'bg-gray-50'">
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
                        <span v-if="!actor" @click="login" class="cursor-pointer">
                          Login
                        </span>
                        <span v-else-if="account && now < (account.expiryByProgram[faucet.index] || 0)">
                          Claim in {{ parseSeconds(account.expiryByProgram[faucet.index] - now) }}
                        </span>
                        <button
                          v-else
                          @click="claim(faucet.index)"
                          type="button"
                          class="cursor-pointer w-full inline-flex items-center justify-center px-4 py-2 border border-transparent shadow-sm font-medium rounded-md text-white bg-purple-600 hover:bg-purple-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500 sm:ml-3 sm:w-auto sm:text-sm"
                        >
                          Claim
                        </button>
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

<script>
import { mapActions, mapState } from 'vuex'
import { rpc } from '@/api/user'

export default {
  name: 'FaucetList',

  data () {
    return {
      account: undefined,
      faucets: [],
      now: Math.floor((new Date()).getTime() / 1000)
    }
  },

  watch: {
    actor: {
      immediate: true,
      handler: async function () {
        this.fetchAccount()
      }
    }
  },

  computed: {
    ...mapState({
      actor: state => state.user.actor
    })
  },

  methods: {
    ...mapActions({
      login: 'user/login',
      transact: 'user/transact'
    }),

    parseSeconds (s, withSeconds = true) {
      const hours = Math.floor(s / 3600)
      const minutes = Math.floor((s - (hours * 3600)) / 60)
      const seconds = (s - (hours * 3600) - (minutes * 60))

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
    },

    async fetchFaucets () {
      const { rows: faucets } = await rpc.get_table_rows({
        code: 'token.faucet',
        scope: 'token.faucet',
        table: 'programs',
        limit: -1
      })
      this.faucets = faucets
    },

    async fetchAccount () {
      if (this.actor) {
        const { rows } = await rpc.get_table_rows({
          code: 'token.faucet',
          scope: 'token.faucet',
          table: 'accounts',
          lower_bound: this.actor,
          limit: 1
        })

        if (rows && rows.length && rows[0].account === this.actor) {
          rows[0].expiryByProgram = rows[0].expiryByProgram.reduce((acc, { key, value }) => {
            acc[key] = value
            return acc
          }, {})
          this.account = rows[0]
        }
      } else {
        this.account = undefined
      }
    },

    async claim (programId) {
      if (!this.actor) {
        await this.login()
        if (!this.actor) {
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
            account: this.actor
          }
        }
      ]

      await this.transact({ actions })
      setTimeout(() => {
        this.fetchAccount()
        this.fetchFaucets()
      }, 1500)
    }
  },

  mounted () {
    this.fetchFaucets()
    setInterval(() => { this.now = Math.floor((new Date()).getTime() / 1000) }, 1000)
  }
}
</script>

<style>

</style>
