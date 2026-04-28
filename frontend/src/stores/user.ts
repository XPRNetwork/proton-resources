import userApi, { proton } from '@/api/user'
import type { Serialize } from '@proton/js'
import type { LinkSession } from '@proton/web-sdk'
import { defineStore } from 'pinia'

type Omit<T, K extends keyof T> = Pick<T, Exclude<keyof T, K>>
type PartialBy<T, K extends keyof T> = Omit<T, K> & Partial<Pick<T, K>>

const getDefaultState = (): {
  actor?: string
  permission?: string

  accountData?: unknown & {
    avatar?: string
  }
} => ({
  actor: undefined,
  permission: undefined,
  accountData: undefined,
})

export const useUserStore = defineStore('user', {
  state: () => getDefaultState(),

  actions: {
    async login() {
      const session = await userApi.login()
      if (session) {
        this.setUser(session)
      }
    },

    async reconnect() {
      const session = await userApi.reconnect()
      if (session) {
        this.setUser(session)
      }
    },

    async logout() {
      await userApi.logout()
      this.resetUser()
    },

    async transact({
      actions,
      broadcast = true,
    }: {
      actions: PartialBy<Serialize.Action, 'authorization'>[]
      broadcast?: boolean
    }) {
      if (!this.actor || !this.permission) {
        throw new Error(
          `Cannot transact as actor or permission are missing. Current: ${this.actor}@${this.permission}`,
        )
      }

      actions = actions.map((action) => {
        action.authorization = [
          {
            actor: this.actor as string,
            permission: this.permission as string,
          },
        ]
        return action
      })

      try {
        const success = await userApi.transact(actions as Serialize.Action[], broadcast)
        return success
      } catch (e: any) {
        const detailsError =
          e.error && e.error.details && e.error.details.length && e.error.details[0].message
        const fieldError = e.json && e.json.fields && e.json.fields.length && e.json.fields[0].error
        const error = detailsError || fieldError || e.message || e.toString() || e
        alert(error)
        throw error
      }
    },

    async setUser(session: LinkSession) {
      if (session.auth) {
        this.actor = session.auth.actor.toString()
        this.permission = session.auth.permission.toString()

        this.accountData = await proton.getProtonAvatar(this.actor)
      }
    },

    resetUser() {
      this.$reset()
    },
  },

  getters: {
    profileImage(): string | undefined {
      const avatar = this.accountData?.avatar
      if (avatar) {
        if (avatar.indexOf('/9j/') !== -1) {
          return `data:image/jpeg;base64,${avatar}`
        } else if (avatar.indexOf('iVBORw0KGgo') !== -1) {
          return `data:image/png;base64,${avatar}`
        }
      }
      return 'https://explorer.xprnetwork.org/img/proton_avatar.png'
    },
  },
})
