import userApi from '@/api/user'

const getDefaultState = () => ({
  actor: undefined,
  permission: undefined,
  accountData: undefined
})

const state = () => getDefaultState()

const getters = {}

const actions = {
  async login ({ commit }) {
    await userApi.login()
    console.log(userApi.wallet)
    if (userApi.wallet.session) {
      commit('setUser', userApi.wallet.session)
    }
  },

  async reconnect ({ commit }) {
    await userApi.reconnect()
    if (userApi.wallet.session) {
      commit('setUser', userApi.wallet.session)
    }
  },

  async logout ({ commit }) {
    await userApi.logout()
    commit('resetUser')
  },

  async transact ({ state }, { actions, broadcast = true }) {
    actions = actions.map(action => {
      action.authorization = [{ actor: state.actor, permission: state.permission }]
      return action
    })
    try {
      const success = await userApi.transact(actions, broadcast)
      return success
    } catch (e) {
      // eslint-disable-next-line no-extra-boolean-cast
      const detailsError = e.error && e.error.details && e.error.details.length && e.error.details[0].message
      const fieldError = e.json && e.json.fields && e.json.fields.length && e.json.fields[0].error
      const error = detailsError || fieldError || e.message || e.toString() || e

      console.log(e)
      console.log(error)

      alert(error)
      throw error
    }
  }
}

const mutations = {
  setUser (state, session) {
    if (session.auth) {
      state.actor = session.auth.actor
      state.permission = session.auth.permission
    }

    if (session.accountData && session.accountData.length) {
      state.accountData = session.accountData[0]
    }
  },

  resetUser (state) {
    Object.assign(state, getDefaultState())
  }
}

export default {
  namespaced: true,
  state,
  getters,
  actions,
  mutations
}
