<template>
  <div class="bg-purple-600">
    <div class="max-w-7xl mx-auto py-16 px-4 sm:py-20 sm:px-6 lg:px-8">
      <div class="lg:space-y-10">
        <h2 class="text-3xl font-extrabold text-white">
          The world's most innovative companies use Proton
        </h2>
        <div class="mt-8 flow-root lg:mt-0">
          <div class="-mt-4 -ml-8 flex flex-wrap justify-between lg:-ml-4">
            <div class="mt-4 ml-8 flex flex-grow flex-shrink-0 lg:flex-grow-0 lg:ml-4" v-for="testimonial of testimonials" :key="testimonial.alt">
              <img class="w-44 h-12 white-logo" :src="testimonial.src" :alt="testimonial.alt">
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'testimonials',
  data () {
    return {
      testimonials: [
        { src: '/img/metal.png', alt: 'Metal' },
        { src: '/img/audius.png', alt: 'Audius' },
        { src: '/img/moeda.png', alt: 'Moeda' },
        { src: '/img/ubitquity.png', alt: 'Ubitquity' },
        { src: '/img/equa.png', alt: 'Equa' }
      ]
    }
  }
}
</script>

<style>
.white-logo {
  filter: brightness(0) invert(1);
}
</style>
