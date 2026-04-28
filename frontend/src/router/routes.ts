export const routes = [
  {
    path: '/',
    component: () => import('@/pages/resources-page.vue'),
  },
  {
    path: '/storage',
    component: () => import('@/pages/storage-page.vue'),
  },
  {
    path: '/create-account',
    component: () => import('@/pages/account-page.vue'),
  },
  {
    path: '/bots',
    component: () => import('@/pages/bots-page.vue'),
  },
  {
    path: '/faucet',
    component: () => import('@/pages/faucet-page.vue'),
  },
]
