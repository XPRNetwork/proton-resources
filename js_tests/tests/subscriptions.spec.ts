import { expect } from "chai";
import { basicPlan, expectAssert, plusPlan, setupChain, usdcPlan } from "./helpers";

const chain = setupChain();
const { atom, deposit, buyplan, advanceHours, internalXpr, tokenBalance, subscription, delegated } = chain;

const process = (max = 10, actor = "user1") => atom.actions.process([max]).send(`${actor}@active`);

describe("subscriptions", () => {
  beforeEach(async () => {
    await chain.reset();
    await atom.actions.addplan([basicPlan]).send();
    await atom.actions.addplan([plusPlan]).send();
    await atom.actions.addplan([usdcPlan]).send();
  });

  describe("buyplan", () => {
    it("charges the balance, pays the fee account and delegates resources", async () => {
      await deposit("user1", "100.0000 XPR");
      await buyplan("user1", 0, 2);

      const sub = subscription("user1");
      expect(sub.account).to.equal("user1");
      expect(sub.subscription_hours).to.equal(1440);
      expect(sub.price).to.deep.equal({ quantity: "20.0000 XPR", contract: "eosio.token" });
      expect(sub.cpu_credits).to.equal("100.0000 SYS");
      expect(sub.net_credits).to.equal("10.0000 SYS");

      expect(internalXpr("user1")).to.equal("80.0000 XPR");
      expect(tokenBalance("eosio.ramfee")).to.equal("20.0000 XPR");
      expect(delegated("user1")).to.deep.include({ cpu_weight: "100.0000 SYS", net_weight: "10.0000 SYS" });
    });

    it("rejects a zero quantity", async () => {
      await deposit("user1", "100.0000 XPR");
      await expectAssert(buyplan("user1", 0, 0), "plan_quantity must be positive");
    });

    it("rejects more than the plan maximum", async () => {
      await deposit("user1", "100.0000 XPR");
      await expectAssert(buyplan("user1", 0, 4), "too many plans. Maximum allowed is 3 but you entered 4");
    });

    it("rejects an unknown plan", async () => {
      await deposit("user1", "100.0000 XPR");
      await expectAssert(buyplan("user1", 9, 1), "plan not found");
    });

    it("rejects accounts that never deposited", async () => {
      await expectAssert(buyplan("user2", 0, 1), "account user2 not found");
    });

    it("rejects an insufficient balance", async () => {
      await deposit("user1", "5.0000 XPR");
      await expectAssert(
        buyplan("user1", 0, 1),
        "user token balance too low. Balance is 5.0000 XPR (eosio.token) but you need 10.0000 XPR (eosio.token)"
      );
    });
  });

  describe("switching plans", () => {
    it("carries unused value over as extra hours on the new plan", async () => {
      await deposit("user1", "100.0000 XPR");
      await buyplan("user1", 0, 1); // 10 XPR for 720h
      advanceHours(360); // half used: 5 XPR of value left

      await buyplan("user1", 1, 1); // 50 XPR for 720h, so 5 XPR buys 72h

      const sub = subscription("user1");
      expect(sub.subscription_hours).to.equal(720 + 72);
      expect(sub.price).to.deep.equal({ quantity: "55.0000 XPR", contract: "eosio.token" });
      expect(internalXpr("user1")).to.equal("40.0000 XPR");
      expect(tokenBalance("eosio.ramfee")).to.equal("60.0000 XPR");
      expect(delegated("user1")).to.deep.include({ cpu_weight: "1000.0000 SYS", net_weight: "100.0000 SYS" });
    });

    it("carries nothing over from an expired subscription", async () => {
      await deposit("user1", "100.0000 XPR");
      await buyplan("user1", 0, 1);
      advanceHours(721);

      await buyplan("user1", 1, 1);

      const sub = subscription("user1");
      expect(sub.subscription_hours).to.equal(720);
      expect(sub.price).to.deep.equal({ quantity: "50.0000 XPR", contract: "eosio.token" });
    });

    it("rejects carrying over between plans priced in different tokens", async () => {
      await deposit("user1", "100.0000 XPR");
      await deposit("user1", "100.000000 XUSDC");
      await buyplan("user1", 0, 1);

      await expectAssert(buyplan("user1", 2, 1), "cannot carry over a subscription paid in a different token");
    });
  });

  describe("process", () => {
    beforeEach(async () => {
      await deposit("user1", "100.0000 XPR");
      await deposit("user2", "100.0000 XPR");
      await buyplan("user1", 0, 1); // ends after 720h
      await buyplan("user2", 0, 2); // ends after 1440h
    });

    it("ends expired subscriptions and undelegates their resources", async () => {
      advanceHours(721);
      await process();

      expect(subscription("user1")).to.equal(undefined);
      expect(delegated("user1")).to.equal(undefined);
      expect(subscription("user2")).to.not.equal(undefined);
      expect(delegated("user2")).to.not.equal(undefined);
    });

    it("leaves active subscriptions alone", async () => {
      advanceHours(719);
      await process();
      expect(subscription("user1")).to.not.equal(undefined);
      expect(subscription("user2")).to.not.equal(undefined);
    });

    it("respects the max batch size", async () => {
      advanceHours(1441);
      await process(1);
      expect(subscription("user1")).to.equal(undefined);
      expect(subscription("user2")).to.not.equal(undefined);
    });

    it("can be called by any account", async () => {
      advanceHours(721);
      await process(10, "user2");
      expect(subscription("user1")).to.equal(undefined);
    });

    it("is not triggered by deposits", async () => {
      advanceHours(721);
      await deposit("user2", "1.0000 XPR");
      expect(subscription("user1")).to.not.equal(undefined);
      expect(delegated("user1")).to.not.equal(undefined);
    });
  });
});
