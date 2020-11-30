<template>
  <div>
    <PricingTables/>
    <FeatureList/>
    <Testimonials/>
    <Faq/>
  </div>
</template>

<script>
import Faq from '@/components/faq'
import PricingTables from '@/components/pricingTables'
import FeatureList from '@/components/featureList'
import Testimonials from '@/components/testimonials'

export default {
  components: {
    Faq,
    PricingTables,
    FeatureList,
    Testimonials
  }
}
</script>

<style>

</style>
