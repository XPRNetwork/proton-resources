<template>
  <div>
    <button
      v-if="!actor"
      type="button"
      class="cursor-pointer rounded-2xl min-h-10 px-4 text-base border border-neutral-200 -tracking-own-x4 text-black"
      @click="login()"
    >
      Login
    </button>

    <div class="relative" v-else>
      <NameTemplate v-slot="{ user_name, user_actor }">
        <div class="text-left shrink-0">
          <span class="block text-sm font-semibold text-black -tracking-own-x3 leading-none">
            {{ user_name }}
          </span>
          <span
            v-if="user_actor"
            class="block text-sm font-normal text-neutral-500 -tracking-own-x1 leading-none"
          >
            @{{ user_actor }}
          </span>
        </div>
      </NameTemplate>

      <div ref="menu-parent">
        <button
          class="cursor-pointer min-h-12 max-w-xs flex items-center gap-2 text-sm px-3 hover:ring hover:ring-neutral-200 rounded-2xl font-inter"
          id="user-menu"
          aria-haspopup="true"
          @click="toggleMenu()"
        >
          <img
            class="hidden sm:block h-8 w-8 rounded-full object-cover"
            :src="avatar"
            alt="Profile Photo"
          />

          <UseNameTemplate v-if="nameIsSame" :user_name="actor" />
          <UseNameTemplate v-else :user_name="name" :user_actor="actor" />

          <ChevronDown class="hidden shrink-0 ml-1 h-5 w-5 text-neutral-400 lg:block mt-1" />
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
            class="origin-top-right absolute right-0 mt-2 w-48 rounded-md shadow-lg py-1 bg-white ring-1 ring-neutral-200 ring-opacity-5 font-semibold"
            role="menu"
            aria-orientation="vertical"
            aria-labelledby="user-menu"
            v-if="active"
          >
            <button
              type="button"
              class="px-4 py-2 flex items-center text-sm gap-2 cursor-pointer -tracking-own-x3 text-red-500 hover:text-red-700 leading-none whitespace-nowrap"
              role="menuitem"
              @click.stop="logout"
            >
              <LogoutIcon class="shrink-0 size-4" /> <span>Logout</span>
            </button>
          </div>
        </transition>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { useUserStore } from '@/stores/user'
import { createReusableTemplate, onClickOutside } from '@vueuse/core'
import { computed, ref, useTemplateRef } from 'vue'
import ChevronDown from './icons/chevron-down-icon.vue'
import LogoutIcon from './icons/logout-icon.vue'

const userStore = useUserStore()
const menuParent = useTemplateRef('menu-parent')

const active = ref(false)

const [NameTemplate, UseNameTemplate] = createReusableTemplate<{
  user_name?: string
  user_actor?: string
}>()

const actor = computed(() => userStore.actor)
const accountData = computed(() => userStore.accountData)
const name = computed(() => (accountData.value && accountData.value.name) ?? actor.value)
const nameIsSame = computed(() => name.value === actor.value)

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
