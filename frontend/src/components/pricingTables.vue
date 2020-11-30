<template>
  <div class="bg-gradient-to-b from-white to-gray-50 pb-20">
    <div class="max-w-7xl mx-auto pt-24 px-4 sm:px-6 lg:px-8">
      <div class="sm:flex sm:flex-col sm:align-center">
        <h1 class="text-5xl font-extrabold text-gray-900 sm:text-center">Resource Plans</h1>
        <p class="mt-5 text-xl text-gray-500 sm:text-center">Start building for free, then add a resource plan to go live. Each plan unlocks additional features.</p>
      </div>

      <!-- Pricing -->
      <div class="mt-12 space-y-4 sm:mt-16 sm:space-y-0 sm:grid sm:grid-cols-2 sm:gap-6 lg:max-w-4xl lg:mx-auto xl:max-w-none xl:mx-0 xl:grid-cols-4">
        <div class="border border-gray-200 rounded-lg shadow-sm divide-y divide-gray-200" v-for="pricing of pricingStrategies" :key="pricing.name">
          <div class="p-6">
            <h2 class="text-lg leading-6 font-medium text-gray-900">{{ pricing.name }}</h2>
            <p class="mt-4 text-sm text-gray-500">{{ pricing.description }}</p>
            <p class="mt-8">
              <span class="text-4xl font-extrabold text-gray-900">{{ pricing.price | formatNumber }} </span>
              <span class="text-base font-medium text-gray-500">XPR / mo</span>
            </p>
            <a href="#" class="mt-8 block w-full bg-purple-600 border border-transparent rounded-md py-2 text-sm font-semibold text-white text-center hover:bg-purple-700">
              Buy {{ pricing.name }}
            </a>
          </div>
          <div class="pt-6 pb-8 px-6">
            <h3 class="text-xs font-medium text-gray-900 tracking-wide uppercase">What's included</h3>
            <ul class="mt-6 space-y-4">
              <li class="flex space-x-3" v-for="included in pricing.included" :key="included">
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
  </div>
</template>

<script>
export default {
  name: 'PricingTables',

  data () {
    return {
      pricingStrategies: [
        {
          name: 'Hobby',
          description: 'All the basics for starting a new business',
          price: 100,
          included: [
            '100 CPU and 100 NET system credits',
            'Handles about 500 transactions per day'
          ]
        },
        {
          name: 'Freelancer',
          description: 'All the basics for starting a new business',
          price: 1000,
          included: [
            '1,000 CPU and 1,000 NET system credits',
            '10 KB extra RAM per month',
            'Handles about 5,000 transactions per day'
          ]
        },
        {
          name: 'Startup',
          description: 'All the basics for starting a new business',
          price: 10000,
          included: [
            '10,000 CPU and 10,000 NET system credits',
            '100 KB extra RAM per month',
            'Handles about 50,000 transactions per day',
            'Priority Support'
          ]
        },
        {
          name: 'Enterprise',
          description: 'All the basics for starting a new business',
          price: 100000,
          included: [
            '100,000 CPU and 100,000 NET system credits',
            '1 MB extra RAM per month',
            'Handles about 500,000 transactions per day',
            'Priority Support',
            'Surge resource availability'
          ]
        }
      ]
    }
  }
}
</script>

<style>

</style>
