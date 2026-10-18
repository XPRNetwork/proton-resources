import { expect } from "chai";
import { Asset, Name, TimePoint } from "@greymass/eosio";
import { Account, Blockchain, mintTokens, nameToBigInt, protonAssert, protonAssertMessage, symbolCodeToBigInt } from "@proton/vert";
import { createCdtContract } from "./wasm";

// Path (without extension) to the atom.wasm / atom.abi under test. CI points this at the fresh build output.
export const RESOURCES_CONTRACT = process.env.RESOURCES_CONTRACT ?? "../resources/atom";

export const START_TIME = TimePoint.fromMilliseconds(Date.UTC(2026, 0, 1));
export const HOUR_MS = 60 * 60 * 1000;

export const basicPlan = {
  index: 0,
  plan_hours: 720,
  max_quantity: 3,
  cpu_credits: "100.0000 SYS",
  net_credits: "10.0000 SYS",
  ram_bytes: 0,
  price: { quantity: "10.0000 XPR", contract: "eosio.token" },
  name: "Basic",
  description: "Enough for small demo apps",
  included: ["100 CPU and 10 NET system credits"],
};

export const plusPlan = {
  ...basicPlan,
  index: 1,
  cpu_credits: "1000.0000 SYS",
  net_credits: "100.0000 SYS",
  price: { quantity: "50.0000 XPR", contract: "eosio.token" },
  name: "Plus",
};

export const usdcPlan = {
  ...basicPlan,
  index: 2,
  price: { quantity: "10.000000 XUSDC", contract: "eosio.token" },
  name: "USDC",
};

export const accountAuthority = (actor: string) => ({
  threshold: 1,
  keys: [],
  accounts: [{ permission: { actor, permission: "active" }, weight: 1 }],
  waits: [],
});

export const setupChain = () => {
  const blockchain = new Blockchain();
  const atom = createCdtContract(blockchain, "resources", RESOURCES_CONTRACT);
  const token = blockchain.createContract("eosio.token", "node_modules/proton-tsc/external/eosio.token/eosio.token");
  const xtokens = blockchain.createContract("xtokens", "node_modules/proton-tsc/external/xtokens/xtokens");
  const system = createCdtContract(blockchain, "eosio", "wasm/mocksystem");
  const proton = createCdtContract(blockchain, "eosio.proton", "wasm/mocksystem");
  const [user1, user2, feeAccount]: Account[] = blockchain.createAccounts("user1", "user2", "eosio.ramfee");

  const reset = async () => {
    blockchain.resetTables();
    blockchain.setTime(START_TIME);
    await mintTokens(token, "XPR", 4, 1_000_000_000, 100_000, [user1, user2]);
    await mintTokens(token, "XUSDC", 6, 1_000_000_000, 1_000, [user1]);
    await mintTokens(xtokens, "XPR", 4, 1_000_000, 1_000, [user1]);
  };

  const advanceHours = (hours: number) => blockchain.addTime(TimePoint.fromMilliseconds(hours * HOUR_MS));

  /* Actions */
  const deposit = (from: string, quantity: string, memo = "") =>
    token.actions.transfer([from, atom.name, quantity, memo]).send(`${from}@active`);
  const withdraw = (account: string, quantity: string, contract = "eosio.token") =>
    atom.actions.withdraw([account, { quantity, contract }]).send(`${account}@active`);
  const buyplan = (account: string, planIndex: number, quantity: number) =>
    atom.actions.buyplan([account, planIndex, quantity]).send(`${account}@active`);

  /* Table readers */
  const nameKey = (name: string) => nameToBigInt(Name.from(name));

  const internalBalances = (account: string): Record<string, string> => {
    const row = atom.tables.accounts().getTableRow(nameKey(account));
    if (!row) return {};
    return Object.fromEntries(row.balances.map((b: any) => [`${b.key.sym}@${b.key.contract}`, b.value]));
  };
  const internalXpr = (account: string): string | undefined => internalBalances(account)["4,XPR@eosio.token"];

  const tokenBalance = (account: string, symcode = "XPR", contract: Account = token): string | undefined =>
    contract.tables.accounts(nameKey(account)).getTableRow(symbolCodeToBigInt(Asset.SymbolCode.from(symcode)))?.balance;

  const subscription = (account: string) => atom.tables.subscription().getTableRow(nameKey(account));
  const delegated = (account: string) => system.tables.delband(nameKey(atom.name.toString())).getTableRow(nameKey(account));
  const config = () => atom.tables.config().getTableRows()[0];

  return {
    blockchain, atom, token, xtokens, system, proton, user1, user2, feeAccount,
    reset, advanceHours, deposit, withdraw, buyplan,
    internalBalances, internalXpr, tokenBalance, subscription, delegated, config,
  };
};

// Matches both check(cond, "literal") and check(cond, std::string) failures.
export const expectAssert = async (promise: Promise<any>, message: string) => {
  try {
    await promise;
  } catch (e: any) {
    expect(e.message).to.be.oneOf([protonAssert(message), protonAssertMessage(message)]);
    return;
  }
  throw new Error(`Was expecting to fail with ${message}`);
};

export const expectMissingAuth = async (promise: Promise<any>, actor: string) => {
  try {
    await promise;
  } catch (e: any) {
    expect(e.message).to.equal(`missing required authority ${actor}`);
    return;
  }
  throw new Error(`Was expecting missing authority ${actor}`);
};
