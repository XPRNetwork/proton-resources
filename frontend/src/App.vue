<template>
  <div class="grid grid-rows-[max-content_auto_max-content] min-h-dvh">
    <Header />

    <main>
      <router-view />
    </main>

    <Footer />
  </div>
</template>

<script lang="ts" setup>
import Header from '@/layout/header-layout.vue'
import Footer from '@/layout/footer-layout.vue'
import { useUserStore } from './stores/user'

const userStore = useUserStore()

userStore.reconnect()
</script>
