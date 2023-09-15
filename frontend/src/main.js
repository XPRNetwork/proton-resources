// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from '@/App'
import router from '@/router'
import store from '@/store'

import '@/assets/styles/index.css'
import './filters'

// Redirects
if (window.location.href.indexOf('protonresources.com') !== -1) {
  window.location.href = window.location.href.replace('protonresources.com', 'resources.xprnetwork.org')
}

Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
  router,
  store,
  render: h => h(App)
}).$mount('#app')
