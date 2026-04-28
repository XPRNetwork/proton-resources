import type { RpcInterfaces } from '@proton/js'

export interface IAsset {
  symbol?: string
  amount: number
  precision: number
  original: string
}
export interface IHistory {
  id: string
  time: string
  data: {
    d_string: string | null
    d_uint64_t: string | null
    d_double: string | null
  }
}

export interface IBot {
  index: number
  account: string
  acc?: RpcInterfaces.GetAccountResult
  description: string
  oracle_contract: string
  feed_index: number
  tx_count_by_utc_hour: {
    key: number
    value: number
  }[]
  max_history: number
  history: IHistory[]
}

export interface IPlanSource {
  cpu_credits: string
  net_credits: string
  ram_bytes: number
  price: {
    quantity: string
    contract: string
  }
  index: number
  plan_hours: number
  max_quantity: number
  name: string
  description: string
  included: string[]
}

export interface IPlan extends Omit<IPlanSource, 'cpu_credits' | 'net_credits' | 'price'> {
  cpu_credits: IAsset
  net_credits: IAsset
  price: {
    contract: string
    quantity: IAsset
  }
}

export interface IUserSubscription {
  cpu_credits: string
  net_credits: string
  ram_bytes: number
  price: {
    quantity: string
    contract: string
  }
  account: string
  subscription_hours: number
  start_time: string
}

export interface IAccountFaucet {
  account: string
  nextExpiry: number
  expiryByProgram: {
    key: number
    value: number
  }[]
  expiryByProgramProcessed?: Map<number, number>
}

export interface IFaucetProgram {
  index: number
  creator: string
  savedToken: {
    quantity: string
    contract: string
  }
  claimToken: {
    quantity: string
    contract: string
  }
  duration: number
}
