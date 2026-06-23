<template>
  <div>
    <div class="max-w-7xl mx-auto py-16 px-4 sm:px-6 lg:py-24 lg:px-8">
      <div class="max-w-3xl mx-auto text-center">
        <h2 class="ui-title-2">All-in-one platform</h2>
        <div class="ui-text mt-4">
          <p>
            XPR Network is the robust, all-in-one platform that is ideal for consumer grade
            blockchain apps.
          </p>
        </div>
      </div>

      <dl
        class="mt-12 space-y-10 sm:space-y-0 sm:grid sm:grid-cols-2 sm:gap-x-6 sm:gap-y-12 lg:grid-cols-4 lg:gap-x-8"
      >
        <div class="flex space-x-3" v-for="feature of FEATURES" :key="feature.title" v-once>
          <CheckmarkIcon class="shrink-0 h-6 w-6 text-green-500" />

          <div class="space-y-2">
            <dt class="text-lg leading-6 font-medium text-neutral-900">{{ feature.title }}</dt>
            <dd class="flex space-x-3 lg:py-0 lg:pb-4">
              <span class="text-base text-neutral-500">{{ feature.description }}</span>
            </dd>
          </div>
        </div>
      </dl>
    </div>
  </div>
</template>

<script lang="ts" setup>
import CheckmarkIcon from '@/components/icons/checkmark-icon.vue'

const FEATURES = [
  {
    title: 'Free Account Creation',
    description: 'Consumers can create free accounts using the WebAuth iOS or Android wallets.',
  },
  {
    title: 'Free On-Chain KYC',
    description:
      'XPR Network has partnered with the best of breed KYC providers to offer optional free on-chain KYC.',
  },
  {
    title: 'Open Source SDKs',
    description:
      'Use our SDKs to integrate full wallet capability into your app. Easily build web apps with our web SDKs.',
  },
  {
    title: 'Battle Tested',
    description:
      'XPR Network has been tested at up to 1 million+ daily transactions with steady resource consumption.',
  },
  {
    title: 'World Class Governance',
    description:
      'Our independant governance council pre-screens block producers, and checks for reputation before voting.',
  },
  {
    title: 'Built for Usability',
    description:
      'XPR Network Apps feel just like web apps. No chrome plugins or desktop downloads. Intuitive. Simple. Fast.',
  },
  {
    title: 'Universal @names',
    description:
      'Every XPR Network account name is a human readable, 4+ char string, like a twitter name. Blue checkmark if verified.',
  },
  {
    title: 'Stablecoin Ready',
    description:
      'XPR Network supports fast wrapped stablecoins: XUSDT, XPAX and XUSDC. All sent in 2 seconds, with zero gas fees.',
  },
]
</script>
