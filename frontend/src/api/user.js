import { ConnectWallet } from '@proton/web-sdk'
import { Api, JsonRpc } from '@proton/js'
import { requestAccount, endpoints, appName } from '@/constants'

export const rpc = new JsonRpc(endpoints)
export const api = new Api({ rpc })

let wallet = {
  link: undefined,
  session: undefined
}

export const login = async ({ restoreSession } = { restoreSession: false }) => {
  const connectedWallet = await ConnectWallet({
    linkOptions: {
      rpc,
      restoreSession
    },
    transportOptions: {
      requestAccount,
      requestStatus: true,
      backButton: true
    },
    selectorOptions: {
      appName
    }
  })
  wallet.link = connectedWallet.link
  wallet.session = connectedWallet.session
}

export const transact = async (actions, broadcast) => {
  if (!wallet.session) {
    throw new Error('Could not find user')
  }

  return wallet.session.transact({
    transaction: {
      actions
    }
  }, { broadcast })
}

export const logout = async () => {
  try {
    if (wallet && wallet.link && wallet.session) {
      await wallet.link.removeSession(requestAccount, wallet.session.auth)
    }
  } catch (e) {
    console.log('Cannot log out due to', e)
  }
}

export const reconnect = async () => {
  await login({
    restoreSession: true
  })
}

export default {
  login,
  transact,
  logout,
  reconnect,
  wallet
}
