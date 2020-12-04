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

  async logout ({ commit, dispatch }) {
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
      const fieldError = e.json && e.json.fields && e.json.fields.length && e.json.fields[0].error
      console.log(e)
      console.log(fieldError || e.message || e.toString() || e)

      return e
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
