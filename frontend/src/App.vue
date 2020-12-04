<template>
  <div id="app">
    <Header/>
    <keep-alive>
      <router-view/>
    </keep-alive>
    <Footer/>
  </div>
</template>

<script>
import Footer from '@/components/footer'
import Header from '@/components/header'
import { mapActions } from 'vuex'

export default {
  name: 'App',
  components: {
    Footer,
    Header
  },
  methods: {
    ...mapActions({
      reconnect: 'user/reconnect'
    })
  },

  created () {
    this.reconnect()
  }
}
</script>
