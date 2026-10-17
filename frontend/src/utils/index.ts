import numbro from 'numbro'
import dayjs, { Dayjs } from 'dayjs'
import relativeTime from 'dayjs/plugin/relativeTime'
import utcPlugin from 'dayjs/plugin/utc'
import type { IAsset } from '@/types'

dayjs.extend(relativeTime)
dayjs.extend(utcPlugin)

export const COMMON_DATE_FORMAT = 'MMM DD YYYY, hh:mm:ss A'

export const parseUtcDate = (date: Dayjs | Date | string) => {
  if (typeof date === 'string' && date[date.length - 1] !== 'Z') {
    date = date + 'Z'
  }

  return dayjs(date)
}

export const parseDate = (date: Dayjs | Date | string) => {
  return dayjs(date)
}

export const isUtcInFuture = (date: Dayjs | Date | string) => {
  const d = parseUtcDate(date)
  return d.isAfter(dayjs.utc())
}

export const formatUtcDate = (date: Dayjs | Date | string, format = COMMON_DATE_FORMAT) => {
  return dayjs(parseUtcDate(date)).format(format)
}

export const parseAsset = (asset: string): IAsset => {
  const [amount, symbol] = asset.split(' ')
  const [, afterPeriod] = amount ? amount.split('.') : ''
  const precision = (afterPeriod || '').length

  return {
    symbol,
    amount: amount ? +amount : 0,
    precision,
    original: asset,
  }
}

export function capitalize(value: string | number) {
  if (!value) return ''
  value = value.toString()
  const arr = value.split(' ')
  const capitalizedArray: string[] = []
  arr.forEach((word) => {
    const capitalized = word.charAt(0).toUpperCase() + word.slice(1)
    capitalizedArray.push(capitalized)
  })
  return capitalizedArray.join(' ')
}

export function formatNumber(number: number, precision = 0) {
  return numbro(number).format({
    trimMantissa: true,
    thousandSeparated: true,
    mantissa: precision,
  })
}
