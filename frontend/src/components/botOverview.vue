<template>
  <div class="mt-8">
    <BotClass @select-bot-index="(botIndex) => selectedBot = bots[botIndex]"/>

    <div class="max-w-6xl mx-auto px-4 sm:px-6 lg:px-8">
      <h2 class="text-lg leading-6 font-medium text-gray-900">
        Overview
      </h2>
      <div class="mt-2 grid grid-cols-1 gap-5 sm:grid-cols-2 lg:grid-cols-3">
        <OverviewCard title="Transactions per day">
         {{ transactionsPerDay | formatNumber }}
        </OverviewCard>
        <OverviewCard title="CPU Usage">
          {{ selectedBot ? ((selectedBot.acc.cpu_limit.used / selectedBot.acc.cpu_limit.max) * 100).toFixed(2) : 0 }}%
        </OverviewCard>
        <OverviewCard title="NET Usage">
          {{ selectedBot ? ((selectedBot.acc.net_limit.used / selectedBot.acc.net_limit.max) * 100).toFixed(2) : 0 }}%
        </OverviewCard>
      </div>
    </div>

    <h2 class="max-w-6xl mx-auto mt-8 px-4 text-lg leading-6 font-medium text-gray-900 sm:px-6 lg:px-8">
      Recent activity
    </h2>

    <div class="block" v-if="selectedBot">
      <div class="max-w-6xl mx-auto px-4 sm:px-6 lg:px-8">
        <div class="flex flex-col mt-2">
          <div class="align-middle min-w-full overflow-x-auto shadow overflow-hidden sm:rounded-lg">
            <table class="min-w-full divide-y divide-gray-200">
              <thead>
                <tr>
                  <th class="px-6 py-3 bg-gray-50 text-center text-xs font-medium text-gray-500 uppercase tracking-wider">
                    #
                  </th>
                  <th class="px-6 py-3 bg-gray-50 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Oracle Transaction
                  </th>
                  <th class="px-6 py-3 bg-gray-50 text-center text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Value
                  </th>
                  <th class="hidden px-6 py-3 bg-gray-50 text-center text-xs font-medium text-gray-500 uppercase tracking-wider md:block">
                    Status
                  </th>
                  <th class="px-6 py-3 bg-gray-50 text-center text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Date
                  </th>
                </tr>
              </thead>

              <transition-group name="list" class="bg-white divide-y divide-gray-200" tag="tbody">
                <tr class="bg-white" v-for="(tx, i) of selectedBot.history" :key="tx.id">
                  <td class="px-6 py-4 text-right whitespace-nowrap text-sm text-gray-500">
                    <span class="text-gray-900 font-medium">{{ (transactionsPerDay - i) | formatNumber }}</span>
                  </td>
                  <td class="max-w-0 w-full px-6 py-4 whitespace-nowrap text-sm text-gray-900">
                    <div class="flex">
                      <a href="#" class="group inline-flex space-x-2 truncate text-sm">
                        <!-- Heroicon name: cash -->
                        <svg class="flex-shrink-0 h-5 w-5 text-gray-400 group-hover:text-gray-500" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
                          <path fill-rule="evenodd" d="M4 4a2 2 0 00-2 2v4a2 2 0 002 2V6h10a2 2 0 00-2-2H4zm2 6a2 2 0 012-2h8a2 2 0 012 2v4a2 2 0 01-2 2H8a2 2 0 01-2-2v-4zm6 4a2 2 0 100-4 2 2 0 000 4z" clip-rule="evenodd" />
                        </svg>
                        <a :href="`https://${CHAIN}.bloks.io/tx/${tx.id}`" target="_blank">
                          <p class="text-gray-500 truncate group-hover:text-gray-900">
                            BTC/USDT Price
                          </p>
                        </a>
                      </a>
                    </div>
                  </td>
                  <td class="px-6 py-4 text-right whitespace-nowrap text-sm text-gray-500">
                    <span class="text-gray-900 font-medium">${{ tx.data.d_double | formatNumber(2) }} </span>
                  </td>
                  <td class="hidden px-6 py-4 whitespace-nowrap text-sm text-gray-500 md:block">
                    <span class="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium bg-green-100 text-green-800 capitalize">
                      success
                    </span>
                  </td>
                  <td class="px-6 py-4 text-right whitespace-nowrap text-sm text-gray-500">
                    {{ parseDate(tx.time).format('hh:mm:ss A, MMM DD YYYY') }}
                  </td>
                </tr>
              </transition-group>
            </table>

            <nav class="bg-white px-4 py-3 flex items-center justify-between border-t border-gray-200 sm:px-6" aria-label="Pagination">
              <div class="hidden sm:block">
                <p class="text-sm text-gray-700">
                  Showing latest
                  <span class="font-medium">{{ selectedBot.history.length }}</span>
                  transactions
                </p>
              </div>
            </nav>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import OverviewCard from '@/components/overviewCard'
import BotClass from '@/components/botClass.vue'
import { BOT_CONTRACT, CHAIN } from '@/constants'
import { rpc } from '@/api/user'
import { parseDate } from '@/utils'

const MAX_BOTS = 4

export default {
  components: {
    OverviewCard,
    BotClass
  },

  data () {
    return {
      bots: [],
      selectedBot: undefined,
      feedIndex: 1,
      CHAIN
    }
  },

  computed: {
    transactionsPerDay () {
      if (!this.selectedBot) return 0
      return this.selectedBot.tx_count_by_utc_hour.reduce((acc, txCount) => acc + txCount.value, 0)
    }
  },

  methods: {
    parseDate,
    async fetchBots () {
      const { rows } = await rpc.get_table_rows({
        code: BOT_CONTRACT,
        scope: BOT_CONTRACT,
        table: 'bots',
        limit: -1
      })

      if (rows && rows.length) {
        const promises = rows
          .filter(row => row.feed_index === this.feedIndex)
          .slice(0, MAX_BOTS)
          .map(async (bot, i) => {
            if (!this.selectedBot) {
              const account = await rpc.get_account(bot.account)
              bot.acc = account
            } else {
              bot.acc = this.bots[i].acc
            }

            return bot
          })

        this.bots = await Promise.all(promises)

        if (!this.selectedBot) {
          this.selectedBot = this.bots[0]
        } else {
          this.selectedBot = this.bots.find(bot => bot.index === this.selectedBot.index)
        }
      }
    }
  },

  created () {
    setInterval(() => this.fetchBots(), 5000)
    this.fetchBots()
  }
}
</script>

<style>
.list-enter-active, .list-leave-active {
  transition: all 0.75s;
}
.list-enter, .list-leave-to /* .list-leave-active below version 2.1.8 */ {
  opacity: 0;
  transform: translateX(20px);
}
/* .list-move {
  transition: transform 1s;
} */
</style>
