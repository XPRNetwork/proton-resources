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
export const chainId = CHAIN === 'proton'
  ? '384da888112027f0321850a169f737c33e53b388aad48b5adace4bab97f437e0'
  : '71ee83bcf52142d61019d95f9cc5427ba6a0d7ff8accd9e2088ae2abeaf3d3dd'
export const appName = 'Proton Resources'
