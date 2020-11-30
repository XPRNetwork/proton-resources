<template>
  <div class="max-w-7xl mx-auto py-16 px-4 sm:px-6 lg:py-20 lg:px-8">
    <div class="lg:grid lg:grid-cols-3 lg:gap-8">
      <div class="space-y-4">
        <h2 class="text-3xl font-extrabold text-gray-900">
          Frequently asked questions
        </h2>
        <p class="text-lg text-gray-500">
          Can’t find the answer you’re looking for? Reach out to our <a href="http://support.protonchain.com/" target="_blank" class="font-medium text-purple-600 hover:text-purple-500">customer support</a> team.
        </p>
      </div>
      <div class="mt-12 lg:mt-0 lg:col-span-2">
        <dl class="space-y-12">
          <div v-for="faq of faqs" :key="faq.question">
            <dt class="text-lg leading-6 font-medium text-gray-900">
              {{ faq.question }}
            </dt>
            <dd class="mt-2 text-base text-gray-500">
              {{ faq.answer }}
            </dd>
          </div>
        </dl>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'faq',
  data () {
    return {
      faqs: [
        {
          question: 'Can I buy RAM independantly?',
          answer: 'Yes, we do offer extra RAM independantly if you are deploying a RAM-intensive application. Prices start at $1 per MB.'
        },
        {
          question: 'What is the more scarce resource: NET or CPU?',
          answer: 'CPU is the more scarce resource and we allocate system resources at a 80:20 CPU:NET ratio.'
        },
        {
          question: 'Can I upgrade from one plan to the other?',
          answer: 'Absolutely! You can migrate between plans at any time, and remaining time will be credited towards a new plan.'
        },
        {
          question: 'How can I verify the performance claims made here?',
          answer: 'Our BOTS are stress testing the resource model in real-time. Click on the bots tab to see our enterprise BOT operating at 1 million transactions per day.'
        }
      ]
    }
  }
}
</script>

<style>

</style>
