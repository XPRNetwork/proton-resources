<template>
  <div class="bg-gradient-to-b from-white to-gray-50 pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="text-5xl font-extrabold text-gray-900 sm:text-center">
          Create account
        </h1>
        <p class="mt-5 text-xl text-gray-500 sm:text-center">
          Create a new account with your own custom keys
        </p>
      </div>

      <div class="flex justify-center mt-6 py-6">
        <div class="max-w-xl mt-5 bg-white shadow sm:rounded-lg px-6 py-5 sm:flex sm:items-start sm:justify-between">
          <div class="w-full">
            <div class="font-medium text-gray-900">
              Can create:
              <span v-if="balance !== undefined && costOfAccountInXpr">
                {{ Math.floor(balance / costOfAccountInXpr) }} accounts
              </span>
              <span v-else>
                Please login
              </span>
            </div>
            <div class="mt-1 pr-16 text-sm text-gray-600 sm:flex sm:items-center">
              <div>
                Note: Non-vanity names cost {{ costOfAccountInXpr.toFixed(2) }} XPR to create. Vanity (.xpr, .btc, etc) names cost 50 XPR to create.
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
                >
              </div>

              <div class="w-full">
                Account Public Key:
                <input
                  type="text"
                  name="buybytes"
                  class="shadow-sm focus:ring-purple-500 focus:border-purple-500 block w-full sm:text-sm border-gray-300 rounded-md"
                  v-model="newAccountOwnerKey"
                  :placeholder="`Account Key (Starting with PUB_K1)`"
                >
              </div>

              <button
                type="button"
                class="cursor-pointer w-full inline-flex items-center justify-center px-4 py-2 border border-transparent shadow-sm font-medium rounded-md text-white bg-purple-600 hover:bg-purple-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500sm:w-auto sm:text-sm"
                @click="createAccount"
              >
                Create Account
              </button>
            </div>

            <div
              v-if="accountError"
              class="mt-2 text-red"
            >
              {{ accountError }}
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
import { getRamPrice } from '@/api/ram'

export default {
  name: 'CreateAccount',

  data () {
    return {
      balance: undefined,
      newAccountName: '',
      newAccountOwnerKey: '',
      accountError: '',
      ramPerAccount: 12000,
      ram_price_per_byte: 0,
      ram_price_per_byte_without_fee: 0,
      max_per_user_bytes: 0,
      ram_fee_percent: 0
    }
  },

  watch: {
    actor: {
      immediate: true,
      handler: async function () {
        this.fetchBalance()
      }
    }
  },

  computed: {
    ...mapState({
      actor: state => state.user.actor
    }),

    costOfAccountInXpr () {
      return this.ram_price_per_byte * this.ramPerAccount
    }
  },

  methods: {
    ...mapActions({
      login: 'user/login',
      transact: 'user/transact'
    }),

    async fetchBalance () {
      if (this.actor) {
        const [balance] = await rpc.get_currency_balance('eosio.token', this.actor, 'XPR')
        if (balance) {
          this.balance = +balance.split(' ')[0]
        } else {
          this.balance = undefined
        }
      } else {
        this.balance = undefined
      }
    },

    async updateRamPrice () {
      const stats = await getRamPrice()

      if (stats) {
        this.max_per_user_bytes = stats.max_per_user_bytes
        this.ram_fee_percent = stats.ram_fee_percent
        this.ram_price_per_byte_without_fee = stats.ram_price_per_byte_without_fee
        this.ram_price_per_byte = stats.ram_price_per_byte
      }
    },

    async createAccount () {
      if (!this.actor) {
        await this.login()
        if (!this.actor) {
          alert('Could not find user')
          return
        } else {
          setTimeout(() => this.createAccount(), 2000)
          return
        }
      }

      const creator = this.actor
      const newAccountName = this.newAccountName.trim().toLowerCase()
      const newAccountOwnerKey = this.newAccountOwnerKey.trim()
      const ramPerAccount = this.ramPerAccount

      if (!newAccountName) {
        this.accountError = 'Please enter a new account name'
        return
      }

      if (newAccountName.length > 12) {
        this.accountError = 'Account names can be maximum 12 characters + numbers long'
        return
      }

      if (!RegExp('^[a-zA-Z12345.]+$').test(newAccountName)) {
        this.accountError = 'Account names can only contain the characters a-z and number 1-5.'
        return
      }

      if (!newAccountOwnerKey) {
        this.accountError = 'Please enter a new account key'
        return
      }

      const newAccountData = {
        creator: creator,
        name: newAccountName,
        owner: {
          threshold: 1,
          keys: [{
            key: newAccountOwnerKey,
            weight: 1
          }],
          accounts: [],
          waits: []
        },
        active: {
          threshold: 1,
          keys: [{
            key: newAccountOwnerKey,
            weight: 1
          }],
          accounts: [],
          waits: []
        }
      }

      const suffix = newAccountName.indexOf('.') !== -1 && newAccountName.split('.')[1]
      const actions = suffix
        ? [
          {
            account: 'eosio.token',
            name: 'transfer',
            data: {
              from: creator,
              to: suffix,
              quantity: '50.0000 XPR',
              memo: ''
            }
          },
          {
            account: suffix,
            name: 'newaccount',
            data: newAccountData
          }
        ]
        : [
          {
            account: 'eosio',
            name: 'newaccount',
            data: newAccountData
          },
          {
            account: 'eosio',
            name: 'buyrambytes',
            data: {
              payer: creator,
              receiver: newAccountName,
              bytes: ramPerAccount
            }
          },
          {
            account: 'eosio.proton',
            name: 'newaccres',
            data: {
              account: newAccountName
            }
          }
        ]

      await this.transact({ actions })
      setTimeout(() => this.fetchBalance(), 1500)
      this.newAccountName = ''
      this.newAccountOwnerKey = ''
      this.accountError = ''
    }
  },

  mounted () {
    this.updateRamPrice()
  }
}
</script>

<style>

</style>
