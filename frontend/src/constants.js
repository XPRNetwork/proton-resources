const subdomain = window.location.host.split('.')[1] ? window.location.host.split('.')[0] : false
const subdomainChain = subdomain === 'testnet' ? 'proton-test' : 'proton'
export const CHAIN = process.env.VUE_APP_CHAIN || subdomainChain

export const ATOM_CONTRACT = 'resources'
export const TOKEN_CONTRACT = 'eosio.token'
export const BOT_CONTRACT = 'bot'

export const requestAccount = 'resources'
export const endpoints = CHAIN === 'proton'
  ? ['https://proton.greymass.com', 'https://proton.eoscafeblock.com', 'https://proton.eosusa.news', 'https://proton.cryptolions.io', 'https://proton.pink.gg']
  : ['https://testnet.protonchain.com']
export const appName = 'Proton Resources'
