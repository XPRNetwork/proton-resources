<template>
  <a v-if="isExternalLink" v-bind="$attrs" :href="toHref" target="_blank">
    <slot />
  </a>
  <router-link v-else v-bind="$props" custom :to="to" v-slot="{ isActive, href, navigate }">
    <a
      v-bind="$attrs"
      :href="href"
      @click="navigate"
      :class="isActive ? activeClass : inactiveClass"
    >
      <slot />
    </a>
  </router-link>
</template>

<script setup lang="ts">
import { computed, type PropType } from 'vue'
import { RouterLink, type RouteLocationRaw, type RouterLinkProps } from 'vue-router'

defineOptions({
  inheritAttrs: false,
})

const props = defineProps({
  to: {
    type: [String, Object] as PropType<RouteLocationRaw>,
    required: true,
  },
  replace: Boolean,
  activeClass: String,
  inactiveClass: String,
  exactActiveClass: String,
  custom: Boolean,
  ariaCurrentValue: {
    type: String as PropType<RouterLinkProps['ariaCurrentValue']>,
    default: 'page',
  },
})

const toHref = computed(() => {
  if (typeof props.to === 'string' && props.to.startsWith('http')) {
    return props.to
  }
  return ''
})

const isExternalLink = computed(() => !!toHref.value)
</script>
