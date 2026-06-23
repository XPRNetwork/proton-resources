<template>
  <div class="border rounded-md p-5 max-w-sm w-full mx-auto">
    <div class="animate-pulse flex space-x-4">
      <div class="flex-1 py-1 divide-y divide-neutral-200">
        <div class="mb-4">
          <div class="h-4 bg-neutral-400 rounded w-1/4"></div>

          <div class="mt-4 space-y-2">
            <div class="h-4 bg-neutral-400 rounded"></div>
            <div class="h-4 bg-neutral-400 rounded w-5/6"></div>
          </div>

          <div class="my-8">
            <span class="text-4xl font-extrabold text-neutral-900">X</span>
            <span class="text-base font-medium text-neutral-500 ml-4">XPR / mo</span>
          </div>

          <button type="button" class="ui-button ui-is-primary block w-full" disabled>Buy</button>
        </div>

        <div class="space-y-4 pt-4">
          <h3 class="text-xs font-medium text-neutral-900 tracking-wide uppercase">
            What's included
          </h3>

          <div class="flex" v-for="num of Array.from(Array(maxFeatures).keys())" :key="num">
            <svg
              class="shrink-0 h-5 text-green-500 w-1/6"
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
            <div class="space-y-2 w-5/6">
              <div class="h-4 bg-neutral-400 rounded"></div>
              <div class="h-4 bg-neutral-400 rounded w-5/6"></div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
defineProps<{
  maxFeatures: number
}>()
</script>
