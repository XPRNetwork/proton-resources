Run nodeos before starting tests

```
nodeos \
  --delete-all-blocks \
  --contracts-console \
  -e \
  -p eosio \
  --plugin eosio::chain_api_plugin \
  --plugin eosio::history_api_plugin
````