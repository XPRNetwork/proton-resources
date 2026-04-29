<template>
  <div class="relative bg-white">
    <div class="max-w-7xl mx-auto px-4 sm:px-6">
      <div
        class="flex justify-between items-center border-b border-gray-200 py-6 md:justify-start md:space-x-10"
      >
        <router-link to="/" class="sm:block flex justify-start">
          <span class="sr-only">XPR Network</span>
          <img class="w-auto h-8 sm:h-10" src="@/assets/logo.svg" alt="" />
        </router-link>

        <nav class="flex gap-4 md:gap-6 lg:gap-10 justify-center pr-2">
          <router-link to="/" class="text-base font-medium text-gray-500 hover:text-purple-900">
            Resources
          </router-link>
          <router-link
            to="/storage"
            class="text-base font-medium text-gray-500 hover:text-purple-900"
          >
            Storage
          </router-link>
          <router-link
            to="/create-account"
            class="text-base font-medium text-gray-500 hover:text-purple-900"
          >
            <span class="hidden md:inline-block">Create</span> Account
          </router-link>
          <!-- <router-link to="/bots" class="hidden sm:block text-base font-medium text-gray-500 hover:text-purple-900">
            Bots
          </router-link> -->
          <router-link
            to="/faucet"
            class="text-base font-medium text-gray-500 hover:text-purple-900"
          >
            Faucet
          </router-link>
          <a
            href="https://github.com/XPRNetwork/proton-resources"
            target="_blank"
            class="hidden sm:block text-base font-medium text-gray-500 hover:text-purple-900"
          >
            Github
            <svg
              xmlns="http://www.w3.org/2000/svg"
              class="h-3 w-3 inline-block align-super"
              viewBox="0 0 640 640"
            >
              <!-- !Font Awesome Free v7.2.0 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license/free Copyright 2026 Fonticons, Inc. -->
              <path
                d="M384 64c-17.7 0-32 14.3-32 32s14.3 32 32 32h82.7L265.3 329.4c-12.5 12.5-12.5 32.8 0 45.3s32.8 12.5 45.3 0L512 173.3V256c0 17.7 14.3 32 32 32s32-14.3 32-32V96c0-17.7-14.3-32-32-32zm-240 96c-44.2 0-80 35.8-80 80v256c0 44.2 35.8 80 80 80h256c44.2 0 80-35.8 80-80v-80c0-17.7-14.3-32-32-32s-32 14.3-32 32v80c0 8.8-7.2 16-16 16H144c-8.8 0-16-7.2-16-16V240c0-8.8 7.2-16 16-16h80c17.7 0 32-14.3 32-32s-14.3-32-32-32z"
                fill="currentColor"
              />
            </svg>
          </a>
        </nav>

        <div class="flex items-center justify-end space-x-8 md:flex-1 lg:w-0">
          <Avatar />
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import Avatar from '@/components/avatar-cmp.vue'
</script>
