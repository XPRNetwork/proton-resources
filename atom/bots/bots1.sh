cleos -u https://testnet.protonchain.com push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "syedjafri",
        "to": "bot1",
        "quantity": "300.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "syedjafri",
          "permission": "active"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "syedjafri",
        "to": "bot2",
        "quantity": "3000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "syedjafri",
          "permission": "active"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "syedjafri",
        "to": "bot3",
        "quantity": "30000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "syedjafri",
          "permission": "active"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "syedjafri",
        "to": "bot4",
        "quantity": "300000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "syedjafri",
          "permission": "active"
        }
      ]
    }
  ]
}'