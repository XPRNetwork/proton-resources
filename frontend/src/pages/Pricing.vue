<template>
  <div>
    <PricingTables/>
    <BuyRam/>
    <CreateAccount/>
    <FeatureList/>
    <!-- <Testimonials/> -->
    <Faq/>
  </div>
</template>

<script>
import Faq from '@/components/faq'
import PricingTables from '@/components/pricingTables'
import FeatureList from '@/components/featureList'
import BuyRam from '@/components/buyRam'
import CreateAccount from '@/components/createAccount'

export default {
  components: {
    Faq,
    PricingTables,
    FeatureList,
    BuyRam,
    CreateAccount
  }
}
</script>

<style>

</style>
