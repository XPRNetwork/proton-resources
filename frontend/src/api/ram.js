/* eslint-disable camelcase */
import { rpc } from '@/api/user'

export const getRamPrice = async () => {
  const global = await rpc.get_table_rows({
    code: 'eosio',
    scope: 'eosio',
    table: 'globalram',
    limit: 1
  })

  if (global && global.rows && global.rows.length) {
    const ram_fee_percent = global.rows[0].ram_fee_percent / 10000
    const ram_price_per_byte_without_fee = Number(global.rows[0].ram_price_per_byte.split(' ')[0])
    return {
      ram_fee_percent,
      ram_price_per_byte_without_fee,
      max_per_user_bytes: global.rows[0].max_per_user_bytes,
      ram_price_per_byte: Number((ram_price_per_byte_without_fee / (1 - ram_fee_percent)).toFixed(4))
    }
  } else {
    return undefined
  }
}
