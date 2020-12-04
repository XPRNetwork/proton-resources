<template>
  <div class="bg-gradient-to-b from-white to-gray-50 pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="text-5xl font-extrabold text-gray-900 sm:text-center"><span v-if="CHAIN === 'proton-test'">Testnet</span> Resource Plans</h1>
        <p class="mt-5 text-xl text-gray-500 sm:text-center">
          Start building for free, then add a resource plan to go live. Each plan unlocks additional features.
        </p>
      </div>

      <!-- Pricing -->
      <div class="mt-12 space-y-4 sm:mt-16 sm:space-y-0 sm:grid sm:grid-cols-2 sm:gap-6 lg:max-w-4xl lg:mx-auto xl:max-w-none xl:mx-0 xl:grid-cols-4">
        <template v-if="!plans.length">
          <div class="border border-gray-200 rounded-lg shadow-sm divide-y divide-gray-200" v-for="num of [1,2,3,4]" :key="num">
            <SkeletonPricingTable :maxFeatures="num"/>
          </div>
        </template>
        <div class="border border-gray-200 rounded-lg shadow-sm divide-y divide-gray-200" v-for="plan of plans" :key="plan.name" v-else>
          <div class="p-6">
            <h2 class="text-lg leading-6 font-medium text-gray-900">{{ plan.name }}</h2>
            <p class="mt-4 text-sm text-gray-500 h-10">{{ plan.description }}</p>
            <p class="mt-8">
              <span class="text-4xl font-extrabold text-gray-900">{{ plan.price.quantity.amount | formatNumber }} </span>
              <span class="text-base font-medium text-gray-500">
                {{ plan.price.quantity.symbol }}
                <span v-if="plan.plan_days === 1">/ day</span>
                <span v-else-if="plan.plan_days === 7">/ week</span>
                <span v-else-if="plan.plan_days === 31">/ mo</span>
              </span>
            </p>
            <div
              @click="() => !(userTerm && plan.index < indexOfUserTerm) && selectPlan(plan)"
              :class="{
                'transform hover:scale-105 duration-500': !(userTerm && plan.index < indexOfUserTerm)
              }"
            >
              <!-- Active Plan -->
              <button
                class="mt-8 block w-full bg-blue-600 border border-transparent rounded-md py-2 text-sm font-semibold text-white text-center hover:bg-blue-700 cursor-pointer"
                v-if="userTerm && plan.index === indexOfUserTerm"
                @mouseover="activeHover = true"
                @mouseleave="activeHover= false"
              >
                {{ activeHover ? 'Renew?' : `Plan ends @ ${userTermActiveTill}` }}
              </button>
              <!-- Upgradeable to -->
              <button
                class="mt-8 block w-full bg-purple-600 border border-transparent rounded-md py-2 text-sm font-semibold text-white text-center hover:bg-purple-700 cursor-pointer"
                v-else-if="userTerm && plan.index > indexOfUserTerm"
              >
                Upgrade to {{ plan.name }}
              </button>
              <!-- Buy -->
              <button
                class="mt-8 block w-full bg-purple-600 border border-transparent rounded-md py-2 text-sm font-semibold text-white text-center hover:bg-purple-700"
                :class="{
                  'cursor-pointer': !(userTerm && plan.index < indexOfUserTerm),
                  'opacity-50 cursor-not-allowed': userTerm && plan.index < indexOfUserTerm
                }"
                v-else
              >
                Buy {{ plan.name }}
              </button>
            </div>
          </div>
          <div class="pt-6 pb-8 px-6">
            <h3 class="text-xs font-medium text-gray-900 tracking-wide uppercase">What's included</h3>
            <ul class="mt-6 space-y-4">
              <li class="flex space-x-3" v-for="included in plan.included" :key="included">
                <svg class="flex-shrink-0 h-5 w-5 text-green-500" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
                  <path fill-rule="evenodd" d="M16.707 5.293a1 1 0 010 1.414l-8 8a1 1 0 01-1.414 0l-4-4a1 1 0 011.414-1.414L8 12.586l7.293-7.293a1 1 0 011.414 0z" clip-rule="evenodd" />
                </svg>
                <span class="text-sm text-gray-500">{{ included }}</span>
              </li>
            </ul>
          </div>
        </div>
      </div>
    </div>

    <BuyPlanModal
      v-if="selectedPlan"
      @modal-close="selectedPlan = undefined"
      @confirm-quantity="quantity => buyPlan(quantity, selectedPlan)"
      :plan="selectedPlan"
    />
  </div>
</template>

<script>
import { ATOM_CONTRACT, TOKEN_CONTRACT, CHAIN } from '@/constants'
import { mapActions, mapState } from 'vuex'
import BuyPlanModal from '@/components/buyPlanModal'
import SkeletonPricingTable from '@/components/skeletonPricingTable'
import { rpc } from '@/api/user'
import { parseAsset, parseUtcDate, isUtcInFuture } from '@/utils'

export default {
  name: 'PricingTables',
  components: {
    BuyPlanModal,
    SkeletonPricingTable
  },

  data () {
    return {
      CHAIN,
      selectedPlan: undefined,
      userTerm: undefined,
      activeHover: false,
      plans: []
    }
  },

  watch: {
    actor: {
      immediate: true,
      handler: function () {
        if (this.actor) {
          this.getTermForUser(this.actor)
        } else {
          this.selectedPlan = undefined
          this.userTerm = undefined
        }
      }
    }
  },

  computed: {
    ...mapState({
      actor: state => state.user.actor
    }),

    indexOfUserTerm () {
      if (!this.userTerm) return -1
      return this.plans.findIndex(plan =>
        plan.cpu_credits.original === this.userTerm.cpu_credits &&
        plan.net_credits.original === this.userTerm.net_credits &&
        plan.price.quantity.original === this.userTerm.price.quantity &&
        plan.price.contract === this.userTerm.price.contract
      )
    },

    userTermActiveTill () {
      if (!this.userTerm) return undefined
      return parseUtcDate(this.userTerm.start_time).add(this.userTerm.term_days, 'days').format('MMM DD, hh:mm A')
    }
  },

  methods: {
    ...mapActions({
      login: 'user/login',
      transact: 'user/transact'
    }),

    selectPlan (plan) {
      this.selectedPlan = plan
    },

    async getTermForUser (actor) {
      const { rows } = await rpc.get_table_rows({
        code: ATOM_CONTRACT,
        scope: ATOM_CONTRACT,
        table: 'terms',
        lower_bound: actor,
        limit: 1
      })

      if (rows && rows.length && rows[0].account === actor) {
        const isInFuture = isUtcInFuture(parseUtcDate(rows[0].start_time).add(rows[0].term_days, 'days'))
        if (isInFuture) {
          this.userTerm = rows[0]
        }
      }
    },

    async getPlans () {
      const { rows } = await rpc.get_table_rows({
        code: ATOM_CONTRACT,
        scope: ATOM_CONTRACT,
        table: 'plans',
        limit: -1
      })

      if (rows && rows.length) {
        this.plans = rows.map(row => ({
          ...row,
          net_credits: parseAsset(row.net_credits),
          cpu_credits: parseAsset(row.cpu_credits),
          price: {
            contract: row.price.contract,
            quantity: parseAsset(row.price.quantity)
          }
        }))
      }
    },

    async buyPlan (planQuantity, plan) {
      if (!this.actor) {
        await this.login()
        if (!this.actor) {
          alert('Could not find user')
          return
        } else {
          setTimeout(() => this.buyPlan(planQuantity, plan), 2000)
          return
        }
      }

      const actions = [
        {
          account: TOKEN_CONTRACT,
          name: 'transfer',
          data: {
            from: this.actor,
            to: ATOM_CONTRACT,
            quantity: `${plan.price.quantity.amount.toFixed(plan.price.quantity.precision)} ${plan.price.quantity.symbol}`,
            memo: ''
          }
        },
        {
          account: ATOM_CONTRACT,
          name: 'buyplan',
          data: {
            account: this.actor,
            plan_index: plan.index,
            plan_quantity: planQuantity
          }
        }
      ]

      await this.transact({ actions })

      this.getTermForUser(this.actor)

      this.selectedPlan = undefined
    }
  },

  mounted () {
    this.getPlans()
  }
}
</script>

<style>

</style>
