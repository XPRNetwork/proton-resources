cleosp push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "a.resources",
        "to": "bot1",
        "quantity": "300.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "a.resources",
          "permission": "owner"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "a.resources",
        "to": "bot2",
        "quantity": "3000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "a.resources",
          "permission": "owner"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "a.resources",
        "to": "bot3",
        "quantity": "30000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "a.resources",
          "permission": "owner"
        }
      ]
    },
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "a.resources",
        "to": "bot4",
        "quantity": "300000.0000 XPR",
        "memo": ""
      },
      "authorization": [
        {
          "actor": "a.resources",
          "permission": "owner"
        }
      ]
    }
  ]
}' -p admin.proton@light