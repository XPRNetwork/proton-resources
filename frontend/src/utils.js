import dayjs from 'dayjs'
import relativeTime from 'dayjs/plugin/relativeTime'
import utcPlugin from 'dayjs/plugin/utc'

dayjs.extend(relativeTime)
dayjs.extend(utcPlugin)

export const COMMON_DATE_FORMAT = 'MMM DD YYYY, hh:mm:ss A'

export const parseUtcDate = (date) => {
  if (typeof date === 'string' && date[date.length - 1] !== 'Z') {
    date = date + 'Z'
  }

  return dayjs(date)
}

export const parseDate = (date) => {
  return dayjs(date)
}

export const isUtcInFuture = (date) => {
  date = parseUtcDate(date)
  return date.isAfter(dayjs.utc())
}

export const formatUtcDate = (date, format = COMMON_DATE_FORMAT) => {
  return dayjs(parseUtcDate(date)).format(format)
}

export const parseAsset = (asset) => {
  const [amount, symbol] = asset.split(' ')
  const [, afterPeriod] = amount.split('.')
  const precision = (afterPeriod || '').length

  return {
    symbol,
    amount: +amount,
    precision,
    original: asset
  }
}
