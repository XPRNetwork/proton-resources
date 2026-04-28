<template>
  <div class="bg-linear-to-b from-white to-gray-50 pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="text-5xl font-extrabold text-gray-900 sm:text-center">
          <span v-if="CHAIN === 'proton-test'">Testnet</span> Resource Plans
        </h1>

        <p class="mt-5 text-xl text-gray-500 sm:text-center">
          Start building for free, then add a resource plan to go live. Each plan unlocks additional
          features.
        </p>
      </div>

      <!-- Pricing -->
      <div
        class="mt-12 space-y-4 sm:mt-16 sm:space-y-0 sm:grid sm:grid-cols-2 sm:gap-6 lg:max-w-4xl lg:mx-auto xl:max-w-none xl:mx-0 xl:grid-cols-4"
      >
        <template v-if="!plans.length">
          <div
            class="border border-gray-200 rounded-lg shadow-sm divide-y divide-gray-200"
            v-for="num of [1, 2, 3, 4]"
            :key="num"
          >
            <SkeletonPricingTable :maxFeatures="num" />
          </div>
        </template>
        <div
          class="border border-gray-200 rounded-lg shadow-sm divide-y divide-gray-200"
          v-for="plan of plans"
          :key="plan.name"
          v-else
        >
          <div class="p-6">
            <h2 class="text-lg leading-6 font-medium text-gray-900">{{ plan.name }}</h2>
            <p class="mt-4 text-sm text-gray-500 h-10">{{ plan.description }}</p>
            <p class="mt-8">
              <span class="text-4xl font-extrabold text-gray-900"
                >{{ formatNumber(plan.price.quantity.amount) }}
              </span>
              <span class="text-base font-medium text-gray-500">
                {{ plan.price.quantity.symbol }}
                <span v-if="plan.plan_hours === 24">/ day</span>
                <span v-else-if="plan.plan_hours === 168">/ week</span>
                <span v-else-if="plan.plan_hours === 744">/ mo</span>
              </span>
            </p>
            <div
              @click="
                () =>
                  !(userSubscription && plan.index < indexOfUserSubscription) && selectPlan(plan)
              "
              :class="{
                'transform hover:scale-105 duration-500': !(
                  userSubscription && plan.index < indexOfUserSubscription
                ),
              }"
            >
              <!-- Active Plan -->
              <button
                class="mt-8 block w-full bg-blue-600 border border-transparent rounded-md py-2 text-sm font-semibold text-white text-center hover:bg-blue-700 cursor-pointer"
                v-if="userSubscription && plan.index === indexOfUserSubscription"
                @mouseover="activeHover = true"
                @mouseleave="activeHover = false"
              >
                {{ activeHover ? 'Renew?' : `Plan ends @ ${userSubscriptionActiveTill}` }}
              </button>
              <!-- Upgradeable to -->
              <button
                class="mt-8 block w-full bg-purple-600 border border-transparent rounded-md py-2 text-sm font-semibold text-white text-center hover:bg-purple-700 cursor-pointer"
                v-else-if="userSubscription && plan.index > indexOfUserSubscription"
              >
                Upgrade to {{ plan.name }}
              </button>
              <!-- Buy -->
              <button
                class="mt-8 block w-full bg-purple-600 border border-transparent rounded-md py-2 text-sm font-semibold text-white text-center hover:bg-purple-700"
                :class="{
                  'cursor-pointer': !(userSubscription && plan.index < indexOfUserSubscription),
                  'opacity-50 cursor-not-allowed':
                    userSubscription && plan.index < indexOfUserSubscription,
                }"
                v-else
              >
                Buy {{ plan.name }}
              </button>
            </div>
          </div>
          <div class="pt-6 pb-8 px-6">
            <h3 class="text-xs font-medium text-gray-900 tracking-wide uppercase">
              What's included
            </h3>
            <ul class="mt-6 space-y-4">
              <li class="flex space-x-3" v-for="included in plan.included" :key="included">
                <svg
                  class="shrink-0 h-5 w-5 text-green-500"
                  xmlns="http://www.w3.org/2000/svg"
                  viewBox="0 0 20 20"
                  fill="currentColor"
                  aria-hidden="true"
                >
                  <path
                    fill-rule="evenodd"
                    d="M16.707 5.293a1 1 0 010 1.414l-8 8a1 1 0 01-1.414 0l-4-4a1 1 0 011.414-1.414L8 12.586l7.293-7.293a1 1 0 011.414 0z"
                    clip-rule="evenodd"
                  />
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
      @confirm-quantity="(quantity) => buyPlan(quantity, selectedPlan)"
      @modal-close="selectedPlan = undefined"
      :plan="selectedPlan"
    />
  </div>
</template>

<script lang="ts" setup>
import { computed, onMounted, ref, watch } from 'vue'
import { ATOM_CONTRACT, TOKEN_CONTRACT, CHAIN } from '@/constants'
import { parseAsset, parseUtcDate, isUtcInFuture, formatNumber } from '@/utils'
import type { IPlan, IPlanSource, IUserSubscription } from '@/types'
import { rpc } from '@/api/user'
import { useUserStore } from '@/stores/user'
import BuyPlanModal from '@/components/buy-plan-modal.vue'
import SkeletonPricingTable from '@/components/skeleton-pricing-table.vue'

const userStore = useUserStore()

const selectedPlan = ref<IPlan | undefined>(undefined)
const userSubscription = ref<IUserSubscription | undefined>(undefined)
const activeHover = ref(false)
const plans = ref<IPlan[]>([])

const actor = computed(() => userStore.actor)

const indexOfUserSubscription = computed(() => {
  if (!userSubscription.value) return -1
  const uSubscription = userSubscription.value
  return plans.value.findIndex(
    (plan) =>
      plan.cpu_credits.original === uSubscription.cpu_credits &&
      plan.net_credits.original === uSubscription.net_credits &&
      plan.price.quantity.original === uSubscription.price.quantity &&
      plan.price.contract === uSubscription.price.contract,
  )
})

const userSubscriptionActiveTill = computed(() => {
  if (!userSubscription.value) return undefined
  return parseUtcDate(userSubscription.value.start_time)
    .add(userSubscription.value.subscription_hours, 'hours')
    .format('MMM DD, hh:mm A')
})

const login = userStore.login
const transact = userStore.transact

const selectPlan = (plan: IPlan) => {
  selectedPlan.value = plan
}

const getSubscriptionForUser = async (actor: string) => {
  const { rows } = await rpc.get_table_rows<IUserSubscription>({
    code: ATOM_CONTRACT,
    scope: ATOM_CONTRACT,
    table: 'subscription',
    lower_bound: actor,
    limit: 1,
  })

  if (rows && rows.length && rows[0] && rows[0].account === actor) {
    const isInFuture = isUtcInFuture(
      parseUtcDate(rows[0].start_time).add(rows[0].subscription_hours, 'hours'),
    )
    if (isInFuture) {
      userSubscription.value = rows[0]
    }
  }
}

const getPlans = async () => {
  const { rows } = await rpc.get_table_rows<IPlanSource>({
    code: ATOM_CONTRACT,
    scope: ATOM_CONTRACT,
    table: 'plans',
    limit: -1,
  })

  if (rows && rows.length) {
    plans.value = rows.map((row) => ({
      ...row,
      net_credits: parseAsset(row.net_credits),
      cpu_credits: parseAsset(row.cpu_credits),
      price: {
        contract: row.price.contract,
        quantity: parseAsset(row.price.quantity),
      },
    }))
  }
}

const buyPlan = async (planQuantity: number, planValue?: IPlan) => {
  if (!planValue) {
    return
  }
  if (!actor.value) {
    await login()
    if (!actor.value) {
      alert('Could not find user')
      return
    } else {
      setTimeout(() => buyPlan(planQuantity, planValue), 2000)
      return
    }
  }

  const actions = [
    {
      account: TOKEN_CONTRACT,
      name: 'transfer',
      data: {
        from: actor.value,
        to: ATOM_CONTRACT,
        quantity: `${planValue.price.quantity.amount.toFixed(planValue.price.quantity.precision)} ${planValue.price.quantity.symbol}`,
        memo: '',
      },
    },
    {
      account: ATOM_CONTRACT,
      name: 'buyplan',
      data: {
        account: actor.value,
        plan_index: planValue.index,
        plan_quantity: planQuantity,
      },
    },
  ]

  await transact({ actions })

  setTimeout(() => {
    if (actor.value) {
      getSubscriptionForUser(actor.value)
    }
  }, 1500)

  selectedPlan.value = undefined
}

watch(
  actor,
  () => {
    if (actor.value) {
      getSubscriptionForUser(actor.value)
    } else {
      selectedPlan.value = undefined
      userSubscription.value = undefined
    }
  },
  {
    immediate: true,
  },
)

onMounted(() => {
  getPlans()
})
</script>
