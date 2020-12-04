cleosp push action resources addplan '{
        "plan": {
                "index": 0,
                "plan_days": 31,
                "max_quantity": 3,
                "cpu_credits": "100.0000 SYS",
                "net_credits": "25.0000 SYS",
                "ram_bytes": 0,
                "price": {
                        "contract": "eosio.token",
                        "quantity": "100.0000 XPR"
                },
                "name": "Basic",
                "description": "Enough for small demo apps",
                "included": [
                        "100 CPU and 25 NET system credits",
                        "Handles about 500 transactions per day"
                ]
        }
}' -p resources;

cleosp push action resources addplan '{
        "plan": {
                "index": 1,
                "plan_days": 31,
                "max_quantity": 3,
                "cpu_credits": "1000.0000 SYS",
                "net_credits": "250.0000 SYS",
                "ram_bytes": 10000,
                "price": {
                        "contract": "eosio.token",
                        "quantity": "1000.0000 XPR"
                },
                "name": "Plus",
                "description": "A perfect plan for average games and small volume apps",
                "included": [
                        "1,000 CPU and 250 NET system credits",
                        "10 KB extra RAM per month",
                        "Handles about 5,000 transactions per day"
                ]
        }
}' -p resources;


cleosp push action resources addplan '{
        "plan": {
                "index": 2,
                "plan_days": 31,
                "max_quantity": 3,
                "cpu_credits": "10000.0000 SYS",
                "net_credits": "2500.0000 SYS",
                "ram_bytes": 100000,
                "price": {
                        "contract": "eosio.token",
                        "quantity": "10000.0000 XPR"
                },
                "name": "Pro",
                "description": "Enough resources for all but the most high volume apps",
                "included": [
                        "10,000 CPU and 2,500 NET system credits",
                        "100 KB extra RAM per month",
                        "Handles about 50,000 transactions per day",
                        "Priority Support"
                ]
        }
}' -p resources;


cleosp push action resources addplan '{
        "plan": {
                "index": 3,
                "plan_days": 31,
                "max_quantity": 3,
                "cpu_credits": "100000.0000 SYS",
                "net_credits": "25000.0000 SYS",
                "ram_bytes": 1000000,
                "price": {
                        "contract": "eosio.token",
                        "quantity": "100000.0000 XPR"
                }
                "name": "Enterprise",
                "description": "Our highest tier for enterprise customers",
                "included": [
                        "100,000 CPU and 25,000 NET system credits",
                        "1 MB extra RAM per month",
                        "Handles about 500,000 transactions per day",
                        "Priority Support",
                        "Surge resource availability"
                ]
        }
}' -p resources;