<template>
  <div class="fixed z-10 inset-0 overflow-y-auto">
    <div
      class="flex items-end justify-center min-h-screen pt-4 px-4 pb-20 text-center sm:block sm:p-0"
    >
      <div class="fixed inset-0 transition-opacity z-0" aria-hidden="true" @click="close">
        <div class="absolute inset-0 bg-neutral-500 opacity-75"></div>
      </div>

      <!-- This element is to trick the browser into centering the modal contents. -->
      <span class="hidden sm:inline-block sm:align-middle sm:h-screen" aria-hidden="true"
        >&#8203;</span
      >

      <div
        class="relative z-10 inline-block align-bottom bg-white rounded-lg px-4 pt-5 pb-4 text-left overflow-hidden shadow-xl transform transition-all sm:my-8 sm:align-middle sm:max-w-lg sm:w-full sm:p-6"
        role="dialog"
        aria-modal="true"
        aria-labelledby="modal-headline"
      >
        <div>
          <div class="mx-auto flex items-center justify-center h-12 w-12 rounded-full bg-blue-100">
            <CheckmarkIcon class="h-6 w-6 text-blue-800" />
          </div>

          <div class="mt-3 text-center sm:mt-5">
            <h3 class="text-lg leading-6 font-medium text-neutral-900" id="modal-headline">
              Buy {{ plan.name }} Plan for

              <div class="relative mx-2 inline-flex rounded-md">
                <button
                  type="button"
                  v-for="q of plan.max_quantity"
                  :key="q"
                  class="relative outline-none first:rounded-l-md last:rounded-r-md inline-flex items-center px-4 py-2 border border-neutral-300 text-sm font-medium cursor-pointer focus:ring-1 focus:ring-outline-800 focus:border-blue-800"
                  :class="{
                    'text-white bg-blue-800 border-blue-800 hover:bg-blue-800/90 hover:border-blue-800/90':
                      quantity === q,
                  }"
                  @click="quantity = q"
                >
                  {{ q }}
                </button>
              </div>

              <span v-if="plan.plan_hours === 24">day</span>
              <span v-else-if="plan.plan_hours === 168">week</span>
              <span v-else-if="plan.plan_hours === 744">month</span>
              <span v-if="quantity > 1">s</span>
            </h3>
            <div class="mt-6">
              <p class="text-sm text-neutral-500">This plan will last {{ lastTill }}</p>
            </div>
          </div>
        </div>
        <div class="mt-5 sm:mt-6 sm:grid sm:grid-cols-2 sm:gap-3 sm:grid-flow-row-dense">
          <button type="button" class="ui-button ui-is-primary" @click="confirm">
            Confirm {{ formatNumber(cost) }} XPR
          </button>
          <button type="button" class="ui-button ui-is-secondary" @click="close">Cancel</button>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import type { IPlan } from '@/types'
import { formatNumber, parseDate } from '@/utils'
import { computed, ref } from 'vue'
import CheckmarkIcon from './icons/checkmark-icon.vue'

const props = defineProps<{
  plan: IPlan
}>()

const emit = defineEmits<{
  (e: 'confirm-quantity', quantity: number): void
  (e: 'modal-close'): void
}>()

const quantity = ref(1)

const cost = computed(() => quantity.value * props.plan.price.quantity.amount)

const lastTill = computed(() => {
  if (props.plan.plan_hours === 0) {
    return 'infinitely'
  }

  const hours = quantity.value * props.plan.plan_hours
  return 'till ' + parseDate(new Date()).add(hours, 'hours').format('MMM DD YYYY, hh:mm A')
})

const close = () => {
  emit('modal-close')
}

const confirm = () => {
  emit('confirm-quantity', quantity.value)
}
</script>

<style>
.days-input {
  width: 33px;
  padding: 10px;
  margin: 0px 10px 10px 10px;
}
</style>
