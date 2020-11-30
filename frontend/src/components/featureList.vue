<template>
  <div class="bg-gradient-to-b from-white to-gray-50">
    <div class="max-w-7xl mx-auto py-16 px-4 sm:px-6 lg:py-24 lg:px-8">
      <div class="max-w-3xl mx-auto text-center">
        <h2 class="text-3xl font-extrabold text-gray-900">All-in-one platform</h2>
        <p class="mt-4 text-lg text-gray-500">Proton is the robust, all-in-one platform that is ideal for consumer grade blockchain apps.</p>
      </div>

      <dl class="mt-12 space-y-10 sm:space-y-0 sm:grid sm:grid-cols-2 sm:gap-x-6 sm:gap-y-12 lg:grid-cols-4 lg:gap-x-8">
        <div class="flex space-x-3" v-for="feature of features" :key="feature.title">
          <svg class="flex-shrink-0 h-6 w-6 text-green-500" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" aria-hidden="true">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M5 13l4 4L19 7" />
          </svg>

          <div class="space-y-2">
            <dt class="text-lg leading-6 font-medium text-gray-900">{{ feature.title }}</dt>
            <dd class="flex space-x-3 lg:py-0 lg:pb-4">
              <span class="text-base text-gray-500">{{ feature.description }}</span>
            </dd>
          </div>
        </div>
      </dl>
    </div>
  </div>
</template>

<script>
export default {
  name: 'FeatureList',

  data () {
    return {
      features: [
        {
          title: 'Free account creation',
          description: 'Consumers can create free accounts using the Proton iOS or Android wallets. Or, get an API key and build it into your app.'
        },
        {
          title: 'Free On-Chain KYC',
          description: 'Proton has partnered with the best of breed KYC providers to offer optional free on-chain KYC.'
        },
        {
          title: 'Open Source SDKs',
          description: 'Use our SDKs to integrate full wallet capability into your app. Easily build web apps with our web SDKs.'
        },
        {
          title: 'Battle Tested',
          description: 'Our bots have been tested at up to 1 million daily transactions with steady resource consumption.'
        },
        {
          title: 'World Class Governance',
          description: 'Our independant overnance council pre-screens block producers, and checks for reputation before voting.'
        },
        {
          title: 'Built for Usability',
          description: 'Proton Apps feel just like web apps. No chrome plugins or desktop downloads. Intuitive. Simple. Fast.'
        },
        {
          title: 'Universal @names',
          description: 'Every Proton accoutn name is a human readable, 4+ char string, like a twitter name. Blue checkmark if verified.'
        },
        {
          title: 'Stablecoin ready',
          description: 'Proton supports fast wrapped stablecoins: XUSDT, XPAX and XUSDC. All send in 2 seconds, with zero gas fees.'
        }
      ]
    }
  }
}
</script>

<style>

</style>
