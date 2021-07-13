<template>
  <div class="bg-gradient-to-b from-white to-gray-50 pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="text-5xl font-extrabold text-gray-900 sm:text-center">
          Buy Blockchain Storage
        </h1>
        <p class="mt-5 text-xl text-gray-500 sm:text-center">
          Blockchain Storage (RAM) allows you to store data on the blockchain such as NFTs, marketplace sales and tokens.
        </p>
      </div>

      <div class="flex justify-center mt-6 sm:p-6">
        <div class="max-w-xl mt-5 bg-white shadow sm:rounded-lg px-6 py-5 sm:flex sm:items-start sm:justify-between">
          <h4 class="sr-only">RAM</h4>
          <div class="sm:flex sm:items-start">
            <div class="mt-3 sm:mt-0 sm:ml-4">
              <div class="font-medium text-gray-900">
                Account Usage:
                <span v-if="account">{{ account.ram_usage }} / {{ account.ram_quota }} bytes</span>
              </div>
              <div class="mt-1 text-sm text-gray-600 sm:flex sm:items-center">
                <div>
                  Note: Proton Wallet provides 12,000 unsellable bytes to every account
                </div>
              </div>

              <div class="mt-6 sm:items-center">
                Buy Storage Bytes ({{ ram_price_per_byte }} XPR / byte):

                <div class="mt-2 flex">
                  <div class="w-full sm:max-w-xs">
                    <input
                      type="text"
                      name="buybytes"
                      class="shadow-sm focus:ring-purple-500 focus:border-purple-500 block w-full sm:text-sm border-gray-300 rounded-md"
                      v-model="buyRamBytes"
                      :placeholder="`Maximum ${account ? max_per_user_bytes - ram : max_per_user_bytes}`"
                    >
                  </div>
                  <button
                    type="button"
                    class="cursor-pointer w-full inline-flex items-center justify-center px-4 py-2 border border-transparent shadow-sm font-medium rounded-md text-white bg-purple-600 hover:bg-purple-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500 sm:ml-3 sm:w-auto sm:text-sm"
                    @click="buyRam"
                  >
                    Buy
                  </button>
                </div>

                <div
                  v-if="buyRamBytes"
                  class="mt-2"
                >
                  = {{ (ram_price_per_byte * buyRamBytes).toFixed(4) }} XPR
                </div>
              </div>

              <div v-if="ram" class="mt-5 sm:items-center">
                Sell Storage Bytes ({{ ram_price_per_byte_without_fee }} XPR / byte):

                <div class="mt-2 flex">
                  <div class="w-full sm:max-w-xs">
                    <input
                      type="text"
                      name="sellbytes"
                      class="shadow-sm focus:ring-purple-500 focus:border-purple-500 block w-full sm:text-sm border-gray-300 rounded-md"
                      v-model="sellRamBytes"
                      :placeholder="`Maximum ${account ? Math.min(ram, account.ram_quota - account.ram_usage ) : max_per_user_bytes}`"
                    >
                  </div>
                  <button
                    type="button"
                    class="cursor-pointer w-full inline-flex items-center justify-center px-4 py-2 border border-transparent shadow-sm font-medium rounded-md text-white bg-purple-600 hover:bg-purple-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500 sm:ml-3 sm:w-auto sm:text-sm"
                    @click="sellRam"
                  >
                    Sell
                  </button>
                </div>

                <div
                  v-if="sellRamBytes"
                  class="mt-2"
                >
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

<script>
import { mapActions, mapState } from 'vuex'
import { rpc } from '@/api/user'
import { getRamPrice } from '@/api/ram'

export default {
  name: 'BuyRam',

  data () {
    return {
      buyRamBytes: 0,
      sellRamBytes: 0,
      account: undefined,
      ram: 0,
      quantity: 0,
      ramlimit: 0,
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
        this.updateUserRam()
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

    async updateUserRam () {
      if (this.actor) {
        this.account = await rpc.get_account(this.actor)
        const user = await rpc.get_table_rows({
          code: 'eosio',
          scope: this.actor,
          table: 'usersram',
          limit: 1
        })

        if (user && user.rows && user.rows.length) {
          this.ram = user.rows[0].ram
          this.quantity = user.rows[0].quantity.split(' ')[0]
          this.ramlimit = user.rows[0].ramlimit
        }
      } else {
        this.account = undefined
        this.ram = 0
        this.quantity = 0
        this.ramlimit = 0
        this.ram_price_per_byte_without_fee = 0
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

    async buyRam () {
      if (!this.actor) {
        await this.login()
        if (!this.actor) {
          alert('Could not find user')
          return
        } else {
          setTimeout(() => this.buyRam(), 2000)
          return
        }
      }

      const actions = [
        {
          account: 'eosio',
          name: 'buyrambytes',
          data: {
            payer: this.actor,
            receiver: this.actor,
            bytes: this.buyRamBytes
          }
        }
      ]

      await this.transact({ actions })
      setTimeout(() => this.updateUserRam(), 1000)
      this.buyRamBytes = 0
    },

    async sellRam () {
      if (!this.actor) {
        await this.login()
        if (!this.actor) {
          alert('Could not find user')
          return
        } else {
          setTimeout(() => this.sellRam(), 2000)
          return
        }
      }

      const actions = [
        {
          account: 'eosio',
          name: 'sellram',
          data: {
            account: this.actor,
            bytes: this.sellRamBytes
          }
        }
      ]

      await this.transact({ actions })
      setTimeout(() => this.updateUserRam(), 1000)
      this.sellRamBytes = 0
    }
  },

  mounted () {
    this.updateRamPrice()
  }
}
</script>

<style>

</style>
