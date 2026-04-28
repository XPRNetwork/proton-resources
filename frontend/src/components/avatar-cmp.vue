<template>
  <div>
    <a
      @click="login"
      v-if="!actor"
      class="cursor-pointer whitespace-nowrap bg-purple-100 border border-transparent rounded-md py-2 px-4 inline-flex items-center justify-center text-base font-medium text-purple-600 hover:bg-purple-200"
    >
      Login
    </a>

    <div class="relative" v-else>
      <div ref="menu-parent">
        <button
          class="max-w-xs bg-white rounded-full flex items-center text-sm focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-purple-500 lg:p-2 lg:rounded-md lg:hover:bg-gray-50"
          id="user-menu"
          aria-haspopup="true"
          @click="toggleMenu()"
        >
          <img class="hidden sm:block h-8 w-8 rounded-full" :src="avatar" alt="Profile Photo" />

          <span class="ml-1 sm:ml-3 text-gray-700 text-sm font-medium lg:block">
            {{ actor }}
          </span>

          <svg
            class="hidden shrink-0 ml-1 h-5 w-5 text-gray-400 lg:block mt-1.5"
            xmlns="http://www.w3.org/2000/svg"
            viewBox="0 0 20 20"
            fill="currentColor"
            aria-hidden="true"
          >
            <path
              fill-rule="evenodd"
              d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z"
              clip-rule="evenodd"
            />
          </svg>
        </button>

        <transition
          enter-active-class="transition ease-out duration-100 transform"
          enter-class="opacity-0 scale-95"
          enter-to-class="opacity-100 scale-100"
          leave-active-class="transition ease-in duration-75 transform"
          leave-class="opacity-100 scale-100"
          leave-to-class="opacity-0 scale-95"
        >
          <div
            class="origin-top-right absolute right-0 mt-2 w-48 rounded-md shadow-lg py-1 bg-white ring-1 ring-gray-300 ring-opacity-5"
            role="menu"
            aria-orientation="vertical"
            aria-labelledby="user-menu"
            v-if="active"
          >
            <span
              class="block px-4 py-2 text-sm text-gray-700 hover:bg-gray-100 cursor-pointer"
              role="menuitem"
              @click="logout"
            >
              Logout
            </span>
          </div>
        </transition>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { useUserStore } from '@/stores/user'
import { onClickOutside } from '@vueuse/core'
import { computed, ref, useTemplateRef } from 'vue'

const userStore = useUserStore()
const menuParent = useTemplateRef('menu-parent')

const active = ref(false)

const actor = computed(() => userStore.actor)

const avatar = computed(() => userStore.profileImage)

onClickOutside(menuParent, () => {
  active.value = false
})

const toggleMenu = () => {
  active.value = !active.value
}

const login = () => {
  userStore.login()
}

const logout = async () => {
  await userStore.logout()
  active.value = false
}
</script>

<style></style>
