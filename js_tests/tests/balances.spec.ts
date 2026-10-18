import { expect } from "chai";
import { expectAssert, expectMissingAuth, setupChain } from "./helpers";

const chain = setupChain();
const { atom, xtokens, deposit, withdraw, internalBalances, internalXpr, tokenBalance } = chain;

describe("balances", () => {
  beforeEach(async () => {
    await chain.reset();
  });

  describe("deposit", () => {
    it("credits eosio.token deposits to the sender", async () => {
      await deposit("user1", "100.0000 XPR");
      expect(internalXpr("user1")).to.equal("100.0000 XPR");
      expect(tokenBalance("user1")).to.equal("99900.0000 XPR");
      expect(tokenBalance("resources")).to.equal("100.0000 XPR");

      await deposit("user1", "50.0000 XPR", "any memo");
      expect(internalXpr("user1")).to.equal("150.0000 XPR");
    });

    it("tracks each eosio.token symbol separately", async () => {
      await deposit("user1", "100.0000 XPR");
      await deposit("user1", "5.000000 XUSDC");
      expect(internalBalances("user1")).to.deep.equal({
        "4,XPR@eosio.token": "100.0000 XPR",
        "6,XUSDC@eosio.token": "5.000000 XUSDC",
      });
    });

    it("credits transfers with the 'deposit' memo like any other deposit", async () => {
      await deposit("user1", "100.0000 XPR", "deposit");
      expect(internalXpr("user1")).to.equal("100.0000 XPR");
      expect(tokenBalance("resources")).to.equal("100.0000 XPR");
    });

    it("accepts an XPR deposit equal to the minimum", async () => {
      await deposit("user1", "1.0000 XPR");
      expect(internalXpr("user1")).to.equal("1.0000 XPR");
    });

    it("rejects XPR deposits below the minimum", async () => {
      await expectAssert(deposit("user1", "0.9999 XPR"), "minimum deposit is 1.0000 XPR");
      await expectAssert(deposit("user1", "0.0001 XPR"), "minimum deposit is 1.0000 XPR");
      expect(atom.tables.accounts().getTableRows()).to.deep.equal([]);
    });

    it("applies the minimum to top-ups of an existing balance", async () => {
      await deposit("user1", "100.0000 XPR");
      await expectAssert(deposit("user1", "0.5000 XPR"), "minimum deposit is 1.0000 XPR");
      expect(internalXpr("user1")).to.equal("100.0000 XPR");
    });

    it("rejects tokens from contracts other than eosio.token", async () => {
      await expectAssert(
        xtokens.actions.transfer(["user1", "resources", "1.0000 XPR", ""]).send("user1@active"),
        "only eosio.token tokens are supported"
      );
      expect(tokenBalance("user1", "XPR", xtokens)).to.equal("1000.0000 XPR");
    });
  });

  describe("withdraw", () => {
    beforeEach(async () => {
      await deposit("user1", "100.0000 XPR");
    });

    it("returns part of the balance", async () => {
      await withdraw("user1", "40.0000 XPR");
      expect(internalXpr("user1")).to.equal("60.0000 XPR");
      expect(tokenBalance("user1")).to.equal("99940.0000 XPR");
      expect(tokenBalance("resources")).to.equal("60.0000 XPR");
    });

    it("removes the account row when fully withdrawn", async () => {
      await withdraw("user1", "100.0000 XPR");
      expect(atom.tables.accounts().getTableRows()).to.deep.equal([]);
      expect(tokenBalance("user1")).to.equal("100000.0000 XPR");
    });

    it("rejects withdrawing more than the balance", async () => {
      await expectAssert(
        withdraw("user1", "101.0000 XPR"),
        "user token balance too low. Balance is 100.0000 XPR (eosio.token) but you need 101.0000 XPR (eosio.token)"
      );
    });

    it("rejects withdrawing a token the user does not hold", async () => {
      await expectAssert(withdraw("user1", "1.000000 XUSDC"), "user user1 does not own token eosio.token XUSDC");
    });

    it("rejects withdrawing a non-positive amount", async () => {
      await expectAssert(withdraw("user1", "0.0000 XPR"), "balance to substract must be positive");
    });

    it("rejects accounts without a balance", async () => {
      await expectAssert(withdraw("user2", "1.0000 XPR"), "account does not exist");
    });

    it("requires the account's authority", async () => {
      await expectMissingAuth(
        atom.actions.withdraw(["user1", { quantity: "1.0000 XPR", contract: "eosio.token" }]).send("user2@active"),
        "user1"
      );
    });
  });
});
