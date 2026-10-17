import { expect } from "chai";
import { basicPlan, expectAssert, expectMissingAuth, plusPlan, setupChain } from "./helpers";

const chain = setupChain();
const { atom } = chain;

const planRows = () => atom.tables.plans().getTableRows();

describe("plans", () => {
  beforeEach(async () => {
    await chain.reset();
  });

  it("admin can add a plan", async () => {
    await atom.actions.addplan([basicPlan]).send();
    expect(planRows()).to.deep.equal([basicPlan]);
  });

  it("rejects a duplicate plan index", async () => {
    await atom.actions.addplan([basicPlan]).send();
    await expectAssert(atom.actions.addplan([basicPlan]).send(), "plan id 0 already exists");
  });

  it("admin can update a plan", async () => {
    await atom.actions.addplan([basicPlan]).send();
    const updated = { ...basicPlan, price: { quantity: "20.0000 XPR", contract: "eosio.token" } };
    await atom.actions.updateplan([updated]).send();
    expect(planRows()).to.deep.equal([updated]);
  });

  it("rejects updating a missing plan", async () => {
    await expectAssert(atom.actions.updateplan([basicPlan]).send(), "plan does not exist");
  });

  it("admin can remove a plan", async () => {
    await atom.actions.addplan([basicPlan]).send();
    await atom.actions.addplan([plusPlan]).send();
    await atom.actions.removeplan([0]).send();
    expect(planRows()).to.deep.equal([plusPlan]);
  });

  it("rejects removing a missing plan", async () => {
    await expectAssert(atom.actions.removeplan([5]).send(), "plan id : 5 not found");
  });

  it("only the contract can manage plans", async () => {
    await expectMissingAuth(atom.actions.addplan([basicPlan]).send("user1@active"), "resources");
    await atom.actions.addplan([basicPlan]).send();
    await expectMissingAuth(atom.actions.updateplan([basicPlan]).send("user1@active"), "resources");
    await expectMissingAuth(atom.actions.removeplan([0]).send("user1@active"), "resources");
  });
});
