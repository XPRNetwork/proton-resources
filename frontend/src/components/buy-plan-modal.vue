<template>
  <div class="fixed z-10 inset-0 overflow-y-auto">
    <div
      class="flex items-end justify-center min-h-screen pt-4 px-4 pb-20 text-center sm:block sm:p-0"
    >
      <div class="fixed inset-0 transition-opacity z-0" aria-hidden="true" @click="close">
        <div class="absolute inset-0 bg-gray-500 opacity-75"></div>
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
          <div
            class="mx-auto flex items-center justify-center h-12 w-12 rounded-full bg-purple-100"
          >
            <!-- Heroicon name: check -->
            <svg
              class="h-6 w-6 text-purple-600"
              xmlns="http://www.w3.org/2000/svg"
              fill="none"
              viewBox="0 0 24 24"
              stroke="currentColor"
              aria-hidden="true"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M5 13l4 4L19 7"
              />
            </svg>
          </div>
          <div class="mt-3 text-center sm:mt-5">
            <h3 class="text-lg leading-6 font-medium text-gray-900" id="modal-headline">
              Buy {{ plan.name }} Plan for

              <div class="relative mx-2 inline-flex rounded-md">
                <button
                  type="button"
                  v-for="q of plan.max_quantity"
                  :key="q"
                  class="relative outline-none first:rounded-l-md last:rounded-r-md inline-flex items-center px-4 py-2 border border-gray-300 text-sm font-medium cursor-pointer focus:ring-1 focus:ring-purple-500 focus:border-purple-500"
                  :class="{
                    'text-white bg-purple-600 border-purple-600 hover:bg-purple-700 hover:border-purple-700':
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
              <p class="text-sm text-gray-500">This plan will last {{ lastTill }}</p>
            </div>
          </div>
        </div>
        <div class="mt-5 sm:mt-6 sm:grid sm:grid-cols-2 sm:gap-3 sm:grid-flow-row-dense">
          <button
            type="button"
            class="w-full inline-flex justify-center rounded-md border border-transparent shadow-sm px-4 py-2 bg-purple-600 text-base font-medium text-white hover:bg-purple-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500 sm:col-start-2 sm:text-sm cursor-pointer"
            @click="confirm"
          >
            Confirm {{ formatNumber(cost) }} XPR
          </button>
          <button
            type="button"
            class="mt-3 w-full inline-flex justify-center rounded-md border border-gray-300 shadow-sm px-4 py-2 bg-white text-base font-medium text-gray-700 hover:bg-gray-50 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500 sm:mt-0 sm:col-start-1 sm:text-sm cursor-pointer"
            @click="close"
          >
            Cancel
          </button>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import type { IPlan } from '@/types'
import { formatNumber, parseDate } from '@/utils'
import { computed, ref } from 'vue'

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
