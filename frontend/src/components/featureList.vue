<template>
  <div class="bg-gradient-to-b from-white to-gray-50">
    <div class="max-w-7xl mx-auto py-16 px-4 sm:px-6 lg:py-24 lg:px-8">
      <div class="max-w-3xl mx-auto text-center">
        <h2 class="text-3xl font-extrabold text-gray-900">All-in-one platform</h2>
        <p class="mt-4 text-lg text-gray-500">Ac euismod vel sit maecenas id pellentesque eu sed consectetur. Malesuada adipiscing sagittis vel nulla nec.</p>
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
          title: 'Invite team members',
          description: 'Tempor tellus in aliquet eu et sit nulla tellus. Suspendisse est, molestie blandit quis ac. Lacus.'
        },
        {
          title: 'Notifications',
          description: 'Tempor tellus in aliquet eu et sit nulla tellus. Suspendisse est, molestie blandit quis ac. Lacus.'
        },
        {
          title: 'List View',
          description: 'Tempor tellus in aliquet eu et sit nulla tellus. Suspendisse est, molestie blandit quis ac. Lacus.'
        },
        {
          title: 'Boards',
          description: 'Tempor tellus in aliquet eu et sit nulla tellus. Suspendisse est, molestie blandit quis ac. Lacus.'
        },
        {
          title: 'Reporting',
          description: 'Tempor tellus in aliquet eu et sit nulla tellus. Suspendisse est, molestie blandit quis ac. Lacus.'
        },
        {
          title: 'Keyboards',
          description: 'Tempor tellus in aliquet eu et sit nulla tellus. Suspendisse est, molestie blandit quis ac. Lacus.'
        },
        {
          title: 'Calendars',
          description: 'Tempor tellus in aliquet eu et sit nulla tellus. Suspendisse est, molestie blandit quis ac. Lacus.'
        },
        {
          title: 'Mobile',
          description: 'Tempor tellus in aliquet eu et sit nulla tellus. Suspendisse est, molestie blandit quis ac. Lacus.'
        }
      ]
    }
  }
}
</script>

<style>

</style>
