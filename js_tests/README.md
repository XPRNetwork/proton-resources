# Resources contract tests

Tests for the `resources` (atom) contract, run on [VeRT](https://www.npmjs.com/package/@proton/vert), which emulates the chain in Node.js. No `nodeos` needed.

```
npm install
npm test                  # all specs
npm run test:plans        # or test:balances, test:subscriptions, test:admin
```

By default the tests load `../resources/atom.wasm` / `atom.abi`, so rebuild the contract first (`make` in the repo root). To test a different build, set `RESOURCES_CONTRACT` to its path without the extension:

```
RESOURCES_CONTRACT=../build/resources/atom npm test
```

## Layout

- `tests/*.spec.ts`: specs grouped by area.
- `tests/helpers.ts`: chain setup, plan fixtures, table readers and assertion helpers.
- `tests/wasm.ts`: adds a `memory` export to eosio.cdt 1.7 wasm at load time, since VeRT needs it.
- `mocks/mocksystem.cpp`: stand-in for `eosio` and `eosio.proton`. It records `delegatebw` / `undelegatebw` in a `delband` table matching eosio.system, and logs `buyrambytes`, `newaccount` and `newaccres`. It does not move tokens. The built `wasm/mocksystem.*` is committed; rebuild with `npm run build:mocks` after editing.
- `eosio.token` and `xtokens` come from `proton-tsc`.
- `patches/`: `patch-package` fix for `@proton/vert` 0.3.24, applied on install.
