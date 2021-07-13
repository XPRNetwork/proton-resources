<template>
  <div>
    <PricingTables/>
    <Faq/>
    <FeatureList/>
  </div>
</template>

<script>
import Faq from '@/components/faq'
import PricingTables from '@/components/pricingTables'
import FeatureList from '@/components/featureList'

export default {
  components: {
    Faq,
    PricingTables,
    FeatureList
  }
}
</script>

<style>

</style>
