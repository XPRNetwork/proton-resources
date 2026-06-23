<template>
  <div class="relative bg-white">
    <div class="max-w-7xl mx-auto px-4 sm:px-6">
      <div
        class="flex justify-between items-center border-b border-neutral-200 py-6 md:justify-start md:space-x-10"
      >
        <router-link to="/" class="sm:block flex justify-start shrink-0">
          <span class="sr-only">XPR Network</span>
          <img class="w-auto h-8 sm:h-10" src="@/assets/logo.svg" alt="" />
        </router-link>

        <nav class="flex gap-1 sm:gap-4 md:gap-6 lg:gap-8 justify-center pr-2">
          <app-router-link
            to="/"
            :class="linkClass"
            :activeClass="linkActiveClass"
            :inactiveClass="linkInactiveClass"
          >
            Resources
          </app-router-link>

          <app-router-link
            to="/storage"
            :class="linkClass"
            :activeClass="linkActiveClass"
            :inactiveClass="linkInactiveClass"
          >
            Storage
          </app-router-link>

          <app-router-link
            to="/create-account"
            :class="linkClass"
            :activeClass="linkActiveClass"
            :inactiveClass="linkInactiveClass"
          >
            <span class="hidden md:inline">Create</span> Account
          </app-router-link>

          <app-router-link
            to="/faucet"
            :class="linkClass"
            :activeClass="linkActiveClass"
            :inactiveClass="linkInactiveClass"
          >
            Faucet
          </app-router-link>

          <app-router-link
            to="https://github.com/XPRNetwork/proton-resources"
            class="hidden hover:text-black group rounded-2xl p-2 px-2 sm:flex text-base items-center gap-1 -tracking-own-x3 text-neutral-500 font-medium whitespace-nowrap"
          >
            Github
            <ArrowTopRightOnSquare class="h-3 w-3 inline-block align-super -mt-1" />
          </app-router-link>
        </nav>

        <div class="flex items-center justify-end space-x-8 md:flex-1">
          <Avatar />
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import AppRouterLink from '@/components/app-router-link.vue'
import Avatar from '@/components/avatar-cmp.vue'
import ArrowTopRightOnSquare from '@/components/icons/arrow-top-right-on-square-icon.vue'

const linkClass =
  'hover:text-black group rounded-2xl p-2 px-2 text-base -tracking-own-x3 whitespace-nowrap'
const linkActiveClass = 'text-black font-semibold'
const linkInactiveClass = 'text-neutral-500 font-medium'
</script>
