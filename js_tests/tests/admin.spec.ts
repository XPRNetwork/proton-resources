import { expect } from "chai";
import { accountAuthority, basicPlan, expectAssert, expectMissingAuth, setupChain } from "./helpers";

const chain = setupChain();
const { atom, system, proton, deposit, withdraw, buyplan, advanceHours, internalXpr, tokenBalance, subscription, delegated, config } = chain;

const endsub = (account: string, skipUndelegate = false, actor = "resources") =>
  atom.actions.endsub([account, skipUndelegate]).send(`${actor}@active`);
const setpaused = (paused: boolean, actor = "resources") => atom.actions.setpaused([paused]).send(`${actor}@active`);
const newaccount = (creator: string, name: string) =>
  atom.actions.newaccount([creator, name, accountAuthority(creator), accountAuthority(creator)]).send(`${creator}@active`);

describe("admin", () => {
  beforeEach(async () => {
    await chain.reset();
    await atom.actions.addplan([basicPlan]).send();
  });

  describe("endsub", () => {
    beforeEach(async () => {
      await deposit("user1", "100.0000 XPR");
      await buyplan("user1", 0, 1);
    });

    it("ends a subscription and undelegates its resources", async () => {
      await endsub("user1");
      expect(subscription("user1")).to.equal(undefined);
      expect(delegated("user1")).to.equal(undefined);
      expect(internalXpr("user1")).to.equal("90.0000 XPR");
    });

    it("can remove the row without undelegating", async () => {
      await endsub("user1", true);
      expect(subscription("user1")).to.equal(undefined);
      expect(delegated("user1")).to.deep.include({ cpu_weight: "100.0000 SYS", net_weight: "10.0000 SYS" });
    });

    it("rejects unknown subscriptions", async () => {
      await expectAssert(endsub("user2"), "subscription not found");
    });

    it("is admin only", async () => {
      await expectMissingAuth(endsub("user1", false, "user1"), "resources");
    });
  });

  describe("pause", () => {
    it("starts unpaused with no config row", async () => {
      expect(config()).to.equal(undefined);
    });

    it("admin can pause and unpause", async () => {
      await setpaused(true);
      expect(config()).to.deep.equal({ paused: true });
      await setpaused(false);
      expect(config()).to.deep.equal({ paused: false });
    });

    it("rejects setting the current state again", async () => {
      await expectAssert(setpaused(false), "contract is not paused");
      await setpaused(true);
      await expectAssert(setpaused(true), "contract is already paused");
    });

    it("is admin only", async () => {
      await expectMissingAuth(setpaused(true, "user1"), "resources");
    });

    describe("while paused", () => {
      beforeEach(async () => {
        await deposit("user1", "100.0000 XPR");
        await buyplan("user1", 0, 1);
        await setpaused(true);
      });

      it("blocks deposits", async () => {
        await expectAssert(deposit("user1", "1.0000 XPR"), "contract is paused");
        expect(internalXpr("user1")).to.equal("90.0000 XPR");
      });

      it("blocks withdrawals", async () => {
        await expectAssert(withdraw("user1", "1.0000 XPR"), "contract is paused");
      });

      it("blocks buying plans", async () => {
        await expectAssert(buyplan("user1", 0, 1), "contract is paused");
      });

      it("blocks account creation", async () => {
        await expectAssert(newaccount("user1", "al.resources"), "contract is paused");
      });

      it("blocks deposits with the 'deposit' memo too", async () => {
        await expectAssert(deposit("user2", "10.0000 XPR", "deposit"), "contract is paused");
        expect(tokenBalance("resources")).to.equal("90.0000 XPR");
      });

      it("still processes expired subscriptions", async () => {
        advanceHours(721);
        await atom.actions.process([10]).send("user2@active");
        expect(subscription("user1")).to.equal(undefined);
      });

      it("resumes normally after unpausing", async () => {
        await setpaused(false);
        await deposit("user1", "1.0000 XPR");
        await withdraw("user1", "91.0000 XPR");
        expect(internalXpr("user1")).to.equal(undefined);
      });
    });
  });

  describe("newaccount", () => {
    it("charges 50 XPR, creates the account, buys RAM and requests resources", async () => {
      await deposit("user1", "100.0000 XPR");
      await newaccount("user1", "al.resources");

      expect(internalXpr("user1")).to.equal("50.0000 XPR");
      expect(system.tables.newaccts().getTableRows()).to.deep.equal([{ account: "al.resources", creator: "resources" }]);
      expect(system.tables.rambuys().getTableRows()).to.deep.equal([
        { id: 0, payer: "resources", receiver: "al.resources", bytes: 12000 },
      ]);
      expect(proton.tables.newaccres().getTableRows()).to.deep.equal([{ account: "al.resources" }]);
    });

    it("rejects an insufficient balance", async () => {
      await deposit("user1", "10.0000 XPR");
      await expectAssert(
        newaccount("user1", "al.resources"),
        "user token balance too low. Balance is 10.0000 XPR (eosio.token) but you need 50.0000 XPR (eosio.token)"
      );
    });

    // Audit M-5: the inline newaccount is authorised by name.suffix(), so names without a dot cannot be created.
    it("fails for names without a .resources suffix (known issue)", async () => {
      await deposit("user1", "100.0000 XPR");
      try {
        await newaccount("user1", "plainacct");
        expect.fail("newaccount should have failed");
      } catch (e: any) {
        expect(e.message).to.equal("Account plainacct is missing for inline action");
      }
    });
  });
});
