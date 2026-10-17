# Security review: `resources` (atom) contract

Scope: `resources/src/{atom,balance,transfer}.cpp` and `resources/include/atom/*.hpp`.
Method: manual code review, plus VeRT tests in `js_tests/` for the fixed items. Not tested against XPR Network mainnet or testnet.

No finding lets an outside attacker take funds directly. The main risks are in fund accounting, admin powers and user-facing logic that can lose funds or block deposits.

## Status summary

| ID | Severity | Title | Status |
|---|---|---|---|
| H-1 | High | User deposits and contract funds are commingled | Open (fix designed, not applied) |
| H-2 | High | `cleanup` wipes all balances and subscriptions | Mitigated (removed from build), follow-ups open |
| M-1 | Medium | Re-buying a plan forfeits remaining paid time | **Fixed** |
| M-2 | Medium | One failing unstake blocks all deposits | **Fixed** |
| M-3 | Medium | Deposits with memo `deposit` are never credited | Open |
| M-4 | Medium | Contract pays RAM for dust deposit rows | Open |
| M-5 | Medium | `newaccount` authority derived from user-chosen name | Open |
| M-6 | Medium | No price limit on `buyplan` | Open |
| L-1 | Low | Unchecked price multiplication | Open |
| L-2 | Low | No plan validation | Open |
| L-3 | Low | Expired stake persists until `process` is called | Open (more important after M-2 fix) |
| L-4 | Low | `process` loop counter narrower than `max` | Open |
| I-* | Info | Code quality and centralisation notes | Open |

---

## High

### H-1: User deposits and contract funds are commingled

**Where:** `atom::newaccount`, `atom::buyplan` (`src/atom.cpp`)

The `accounts` table tracks what users can withdraw, but the contract also spends its own token balance:

- `newaccount` charges the creator a fixed 50 XPR, but the contract pays for 12,000 bytes of RAM at the market price. If RAM costs more than 50 XPR, the difference comes out of other users' deposits.
- In `buyplan`, if `eosio.ramfee` doesn't exist, the plan payment stays in the contract without being recorded anywhere.

Nothing checks that the contract's real balance covers the total owed to users, so it can become insolvent, and the last users to withdraw lose out.

**Recommendation:** keep a `liabilities` table with the total owed to users per token, updated in `add_balance` and `substract_balance`. Queue an internal `checksolvent` action as the last inline action of `newaccount` and `buyplan`, and have it assert that the `eosio.token` balance is at least the total owed. Make the account-creation fee configurable, or tie it to the RAM price.

### H-2: `cleanup` wipes all balances and subscriptions

**Where:** `atom::cleanup` (`include/atom/atom.hpp`), `commands.sh`

`cleanup` erased the `accounts`, `subscription` and `plans` tables:
- User deposits were deleted but the tokens stayed in the contract.
- Delegated stake was never undelegated.
- The loops had no limit.

`commands.sh` runs it as its first step.

**Status:** the action is commented out and no longer in the ABI. Follow-ups:
- Remove the `cleanup` call from `commands.sh`, and switch that script's `updateplan` calls to `addplan` for plans that don't exist yet.
- Remove the `cleanup` entry from `ricardian/atom.contracts.md`.
- If it's needed for testnet, put it behind a `TESTNET` compile flag, refuse to run while subscriptions exist, and limit the number of rows erased per call.
- Change table layouts with `binary_extension` or a migration to a new table instead of wiping data.

---

## Medium

### M-1: Re-buying a plan forfeits remaining paid time (Fixed)

**Where:** `atom::buyplan`

An existing subscription was ended and replaced without credit, so upgrading or renewing early lost the remaining paid time.

**Fix:**
- Unused value (price × time left ÷ total time) is now converted into extra hours on the new plan at the new plan's hourly rate. The calculation uses 128-bit integers and rounds down to whole hours.
- No tokens are refunded, so the contract never pays out of its own balance.
- The stored `price` is now total value (amount paid plus carried-over value), so later switches use the right rate.
- Switching between plans priced in different tokens is rejected (`cannot carry over a subscription paid in a different token`).

**Tests:** `subscriptions.spec.ts`, section "switching plans".

### M-2: One failing unstake blocks all deposits (Fixed)

**Where:** `atom::ontransfer`, `atom::process`

Every deposit called `process(5)`, which always takes the earliest-expiring subscription first. If that subscription's inline `undelegatebw` failed, every deposit and every `process` call failed with it, and the only admin remedy was `cleanup`.

**Fix:**
- `ontransfer` no longer calls `process`.
- New admin action `endsub(account, skip_undelegate)` ends a single subscription. `skip_undelegate = true` removes a stuck row without unstaking.

**Tests:** `subscriptions.spec.ts` ("process", "is not triggered by deposits") and `admin.spec.ts` ("endsub").

### M-3: Deposits with memo `deposit` are never credited

**Where:** `atom::ontransfer`

Transfers with the memo `deposit` are accepted but not credited to the sender. That's the memo users are most likely to type, and those funds can't be recovered.

**Recommendation:** use a reserved memo users won't type, or only skip crediting when the sender is an allowlisted treasury or admin account.

### M-4: Contract pays RAM for dust deposit rows

**Where:** `atom::add_balance`

Account rows are paid for by `get_self()`. 0.0001 XPR deposits from many accounts can use up the contract's RAM, after which `buyplan`, `newaccount` and deposits fail.

**Recommendation:** require a minimum deposit, or add an `open` action so users pay for their own row.

### M-5: `newaccount` authority derived from user-chosen name

**Where:** `atom::newaccount`

The inline `newaccount` is signed with `{name.suffix(), "active"}`, which comes from the caller's chosen name. Names without a dot fail, and only `*.resources` names work. If a third-party suffix account had granted `resources@eosio.code`, the contract would be signing with that account's permission.

**Recommendation:** sign with `{get_self(), "active"}` and validate the name format explicitly.
**Tests:** `admin.spec.ts`, "fails for names without a .resources suffix (known issue)". Update that test when this is fixed.

### M-6: No price limit on `buyplan`

**Where:** `atom::buyplan`, `atom::updateplan`

`buyplan` only takes the plan index. The admin key, or anyone who compromises it, can raise the price just before a user's `buyplan` runs and take the user's whole deposited balance.

**Recommendation:** add a `max_price` (or expected price) parameter to `buyplan`.

---

## Low

### L-1: Unchecked price multiplication
`adjusted_price.quantity.amount *= plan_quantity` multiplies the raw `int64` amount with no overflow check and no `is_valid()` afterwards. The values come from the admin, but a large price combined with a large `max_quantity` could wrap around to a small price. Use `asset * int64` and validate the result.

### L-2: No plan validation
`addplan` and `updateplan` don't check that `plan_hours` and `max_quantity` are above 0, that the price is positive and from `eosio.token`, or that the CPU/NET symbols are the core symbol. A bad plan is only noticed when `buyplan` fails.

### L-3: Expired stake persists until `process` is called
Unstaking only happens when someone calls `process`. Since the M-2 fix removed the call from deposits, a scheduled bot is **required**: for example `process [20]` every few minutes, signed by any account. Until it runs, users keep their staked resources after their plan ends.

### L-4: `process` loop counter narrower than `max`
`uint16_t i < max` with a `uint64_t max` overflows and wraps if `max > 65535`. In practice it stops at the end of the table or at the CPU limit, but the types should match.

---

## Informational

- `ram_bytes` is stored in plans but never delivered (subscriptions always set it to 0).
- The 50 XPR account fee and the 12,000 bytes of RAM are hard-coded. `MAX_TERM_MONTHS` and `HOURS_IN_DAY` are unused.
- The `*::transfer` wildcard handler rejects transfers from every token contract except `eosio.token`, NFTs included.
- `planreceipt` is sent from inside the `emplace` lambda. Deposits and withdrawals emit no receipt.
- `buyplan` looks up `_accounts` itself and `substract_balance` looks it up again.
- The admin is fully trusted: a single key can upgrade the contract or change prices, with no timelock. Consider putting the `resources` permissions behind a multisig.
- After the M-1 fix, switching to a cheaper plan can produce more hours than the plan's `max_quantity` allows. Value is preserved, so this is acceptable, but it's worth knowing.

---

## Other changes in this branch

- **Pause switch:** new admin action `setpaused(bool)`, stored in a `config` singleton (default unpaused).
  - Blocks: deposits, `withdraw`, `buyplan`, `newaccount`.
  - Still works: `process`, admin actions, `deposit`-memo top-ups, and refunds from system accounts.
  - Blocking `withdraw` is a deliberate emergency-stop choice; remove that check if users should always be able to withdraw.
  - Tests: `admin.spec.ts`, "pause".
- **Tests:** the old eoslime suite was replaced with a VeRT suite (`js_tests/`, 50 tests). It uses a mock for `eosio` and `eosio.proton` that records stake without moving tokens, so real system-contract token transfers aren't covered.
- **CI:** `.github/workflows/ci.yml` builds both contracts with eosio.cdt 1.7.0, checks that the committed ABIs match the source, and runs the VeRT tests against the fresh build.
