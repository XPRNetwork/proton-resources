import Vue from 'vue'
import Router from 'vue-router'
import Pricing from '@/pages/Pricing'
import Bots from '@/pages/Bots'

Vue.use(Router)

export default new Router({
  mode: 'history',
  routes: [
    {
      path: '/',
      name: 'Pricing',
      component: Pricing
    },
    {
      path: '/bots',
      name: 'Bots',
      component: Bots
    }
  ]
})
