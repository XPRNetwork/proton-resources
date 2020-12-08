cleos -u https://testnet.protonchain.com push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "bot1",
        "to": "resources",
        "quantity": "300.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "bot1",
          "permission": "active"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "bot2",
        "to": "resources",
        "quantity": "3000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "bot2",
          "permission": "active"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "bot3",
        "to": "resources",
        "quantity": "30000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "bot3",
          "permission": "active"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "bot4",
        "to": "resources",
        "quantity": "300000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "bot4",
          "permission": "active"
        }
      ]
    }
  ]
}'