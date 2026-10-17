import ProtonSDK, {
  type Link,
  type LinkSession,
  type ProtonWebLink,
  type TransactResult,
} from '@proton/web-sdk'
import { Api, JsonRpc, Serialize } from '@proton/js'
import { requestAccount, endpoints, appName, chainId, CHAIN } from '@/constants'
import { ApiClass } from '@proton/api'
import { constants as bloksConstants } from '@bloks/constants'

bloksConstants.initialize(CHAIN)

export const proton = new ApiClass()
proton.initialize(bloksConstants)

export const rpc = new JsonRpc(endpoints)
export const api = new Api({ rpc })

export let link: ProtonWebLink | Link | null | undefined
export let session: LinkSession | null | undefined

export const createLink = async ({
  restoreSession = false,
}: {
  restoreSession?: boolean
}): Promise<void> => {
  const { link: localLink, session: localSession } = await ProtonSDK({
    linkOptions: {
      endpoints,
      chainId,
      restoreSession,
    },
    transportOptions: {
      requestAccount,
      requestStatus: true,
    },
    uiOptions: {
      appInfo: {
        name: appName,
      },
      theme: 'light',
    },
  })
  link = localLink
  session = localSession
}

export const login = async (): Promise<LinkSession> => {
  await createLink({ restoreSession: false })
  if (session) {
    return session
  } else {
    throw new Error('No Session')
  }
}

export const transact = async (
  actions: Serialize.Action[],
  broadcast: boolean,
): Promise<TransactResult> => {
  if (session) {
    return session.transact(
      {
        transaction: {
          actions,
        } as never,
      },
      { broadcast },
    )
  } else {
    throw new Error('No Session')
  }
}

export const logout = async (): Promise<void> => {
  if (link && session) {
    await link.removeSession(requestAccount, session.auth, chainId)
  }
  session = undefined
  link = undefined
}

export const reconnect = async (): Promise<LinkSession> => {
  if (!session) {
    await createLink({ restoreSession: true })
  }

  if (session) {
    return session
  } else {
    throw new Error('No Session')
  }
}

export default {
  link,
  login,
  transact,
  logout,
  reconnect,
}
