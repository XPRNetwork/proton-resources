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

cleosp push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "a.resources",
        "to": "eosio.token",
        "quantity": "0.0001 XPR",
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
}' -p admin.proton@light


cleosp push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "a.resources",
        "to": "eosio.token",
        "quantity": "0.0001 XPR",
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
      "account": "resources",
      "name": "buyplan",
      "data": {
        "account": "bot1",
        "plan_index": 0,
        "plan_quantity": 3
      },
      "authorization": [
        {
          "actor": "bot1",
          "permission": "active"
        }
      ]
    },
    {
      "account": "resources",
      "name": "buyplan",
      "data": {
        "account": "bot2",
        "plan_index": 1,
        "plan_quantity": 3
      },
      "authorization": [
        {
          "actor": "bot2",
          "permission": "active"
        }
      ]
    },
    {
      "account": "resources",
      "name": "buyplan",
      "data": {
        "account": "bot3",
        "plan_index": 2,
        "plan_quantity": 3
      },
      "authorization": [
        {
          "actor": "bot3",
          "permission": "active"
        }
      ]
    },
    {
      "account": "resources",
      "name": "buyplan",
      "data": {
        "account": "bot4",
        "plan_index": 3,
        "plan_quantity": 3
      },
      "authorization": [
        {
          "actor": "bot4",
          "permission": "active"
        }
      ]
    }
  ]
}' -p admin.proton@light
