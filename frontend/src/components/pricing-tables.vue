<template>
  <div class="pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="ui-title-1 sm:text-center">
          <span v-if="CHAIN === 'proton-test'">Testnet</span> Resource Plans
        </h1>

        <div class="ui-text mt-5 sm:text-center">
          <p>
            Start building for free, then add a resource plan to go live. <br />
            Each plan unlocks additional features.
          </p>
        </div>
      </div>

      <!-- Pricing -->
      <div
        class="mt-12 space-y-4 sm:mt-16 sm:space-y-0 sm:grid sm:grid-cols-2 sm:gap-6 lg:max-w-4xl lg:mx-auto xl:max-w-none xl:mx-0 xl:grid-cols-4"
      >
        <template v-if="!plans.length">
          <div
            class="border border-neutral-200 rounded-lg shadow-sm divide-y divide-neutral-200"
            v-for="num of [1, 2, 3, 4]"
            :key="num"
          >
            <SkeletonPricingTable :maxFeatures="num" />
          </div>
        </template>
        <div
          class="border border-neutral-200 rounded-lg shadow-sm divide-y divide-neutral-200"
          v-for="plan of plans"
          :key="plan.name"
          v-else
        >
          <div class="p-6">
            <h2 class="text-lg leading-6 font-medium text-neutral-900">{{ plan.name }}</h2>
            <p class="mt-4 text-sm text-neutral-500 h-10">{{ plan.description }}</p>
            <p class="mt-8">
              <span class="text-4xl font-extrabold text-neutral-900"
                >{{ formatNumber(plan.price.quantity.amount) }}
              </span>
              <span class="text-base font-medium text-neutral-500">
                {{ plan.price.quantity.symbol }}
                <span v-if="plan.plan_hours === 24">/ day</span>
                <span v-else-if="plan.plan_hours === 168">/ week</span>
                <span v-else-if="plan.plan_hours === 744">/ mo</span>
              </span>
            </p>
            <div
              class="mt-8"
              @click="
                () =>
                  !(userSubscription && plan.index < indexOfUserSubscription) && selectPlan(plan)
              "
            >
              <!-- Active Plan -->
              <button
                v-if="userSubscription && plan.index === indexOfUserSubscription"
                class="ui-button ui-is-primary block w-full"
                type="button"
                @mouseover="activeHover = true"
                @mouseleave="activeHover = false"
              >
                <span v-if="activeHover">Renew?</span>
                <span v-else class="text-sm">Plan ends @ {{ userSubscriptionActiveTill }}</span>
              </button>

              <!-- Upgradeable to -->
              <button
                v-else-if="userSubscription && plan.index > indexOfUserSubscription"
                class="ui-button ui-is-primary block w-full"
                type="button"
              >
                Upgrade to {{ plan.name }}
              </button>

              <!-- Buy -->
              <button
                v-else
                class="ui-button ui-is-primary block w-full"
                :disabled="userSubscription && plan.index < indexOfUserSubscription"
                type="button"
              >
                Buy {{ plan.name }}
              </button>
            </div>
          </div>
          <div class="pt-6 pb-8 px-6">
            <h3 class="text-xs font-medium text-neutral-900 tracking-wide uppercase">
              What's included
            </h3>
            <ul class="mt-6 space-y-4">
              <li class="flex space-x-3" v-for="included in plan.included" :key="included">
                <CheckmarkIcon class="shrink-0 h-5 w-5 text-green-500" />
                <span class="text-sm text-neutral-500">{{ included }}</span>
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
import CheckmarkIcon from '@/components/icons/checkmark-icon.vue'

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
