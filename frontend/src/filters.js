
import Vue from 'vue'
import numbro from 'numbro'

Vue.filter('capitalize', function (value) {
  if (!value) return ''
  value = value.toString()
  const arr = value.split(' ')
  const capitalizedArray = []
  arr.forEach((word) => {
    const capitalized = word.charAt(0).toUpperCase() + word.slice(1)
    capitalizedArray.push(capitalized)
  })
  return capitalizedArray.join(' ')
})

Vue.filter('formatNumber', (number, precision = 0) => numbro(number).format({
  trimMantissa: true,
  thousandSeparated: true,
  mantissa: precision
}))
