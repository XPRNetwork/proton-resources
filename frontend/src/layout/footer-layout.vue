<template>
  <footer class="bg-neutral-900">
    <div class="max-w-7xl mx-auto py-12 px-4 sm:px-6 lg:py-16 lg:px-8">
      <div class="md:flex md:items-center md:justify-between">
        <div class="flex space-x-6 md:order-2 items-center">
          <a href="https://x.com/XPRNetwork" target="_blank" class="text-white hover:text-white/80">
            <span class="sr-only">X</span>
            <TwitterX class="h-5 w-5" />
          </a>
          <a
            href="https://github.com/XPRNetwork"
            target="_blank"
            class="text-white hover:text-white/80"
          >
            <span class="sr-only">GitHub</span>
            <GitHub class="h-6 w-6" />
          </a>
        </div>
        <p class="mt-8 text-base text-white md:mt-0 md:order-1">
          &copy; {{ currentYear }} XPR Network. All rights reserved.
        </p>
      </div>
    </div>
  </footer>
</template>

<script lang="ts" setup>
import GitHub from '@/components/icons/github-icon.vue'
import TwitterX from '@/components/icons/twitter-x-icon.vue'

const currentYear = new Date().getFullYear()
</script>
