import Vue from 'vue'
import Router from 'vue-router'
import Resources from '@/pages/Resources'
import Storage from '@/pages/Storage'
import Faucet from '@/pages/Faucet'
import Account from '@/pages/Account'
import Bots from '@/pages/Bots'

Vue.use(Router)

export default new Router({
  mode: 'history',
  routes: [
    {
      path: '/',
      name: 'Resources',
      component: Resources
    },
    {
      path: '/storage',
      name: 'Storage',
      component: Storage
    },
    {
      path: '/create-account',
      name: 'Account',
      component: Account
    },
    {
      path: '/bots',
      name: 'Bots',
      component: Bots
    },
    {
      path: '/faucet',
      name: 'Faucet',
      component: Faucet
    }
  ]
})
