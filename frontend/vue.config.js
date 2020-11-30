module.exports = {
  productionSourceMap: false,

  // For hot reload
  devServer: {
    host: 'localhost',
    disableHostCheck: true
  },

  configureWebpack: {
    resolve: {
      symlinks: false
    }
  },

  chainWebpack: (config) => {
    const isLegacyBundle = process.env.VUE_CLI_MODERN_MODE && !process.env.VUE_CLI_MODERN_BUILD
    if (!isLegacyBundle) {
      config.plugin('preload').tap(() => {
        return [{
          rel: 'preload',
          include: 'allAssets',
          fileWhitelist: [/.*\.css/],
          fileBlacklist: [/hot-update\.js$/]
        }]
      })
    }
  }
}
