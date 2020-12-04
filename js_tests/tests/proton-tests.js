const assert = require('assert');

const TOKEN_WASM_PATH = `${__dirname}/../../c++_tests/system_wasms/eosio.token.wasm`;
const TOKEN_ABI_PATH = `${__dirname}/../../c++_tests/system_wasms/eosio.token.abi`;

const ATOM_WASM_PATH = `${__dirname}/../../atom/atom.wasm`;
const ATOM_ABI_PATH = `${__dirname}/../../atom/atom.abi`;

describe("Atom", function (eoslime) {

    // Increase mocha(testing framework) time, otherwise tests fails
    this.timeout(15000);

    let atomContract;
    let tokenContract;
    let tokensIssuer;
    let tokensHolder;
    let plan0;

    before(async () => {
        let accounts = await eoslime.Account.createRandoms(3);
        tokensIssuer = accounts[0];
        tokensHolder = accounts[1];
    });

    beforeEach(async () => {
        tokenContract = await eoslime.Contract.deploy(TOKEN_WASM_PATH, TOKEN_ABI_PATH);
        await tokenContract.actions.create([tokensIssuer.name, "1000000000.0000 SYS"]);
        await tokenContract.actions.issue([tokensIssuer.name, `100000.0000 SYS`, ""], { from: tokensIssuer });
        await tokenContract.actions.transfer([tokensIssuer.name, tokensHolder.name, `100000.0000 SYS`, ""], { from: tokensIssuer });

        await tokenContract.actions.create([tokensIssuer.name, "1000000000.0000 XPR"]);
        await tokenContract.actions.issue([tokensIssuer.name, `100000.0000 XPR`, ""], { from: tokensIssuer });
        await tokenContract.actions.transfer([tokensIssuer.name, tokensHolder.name, `100000.0000 XPR`, ""], { from: tokensIssuer });

        await tokenContract.actions.create([tokensIssuer.name, "1000000000.0000 ABC"]);
        await tokenContract.actions.issue([tokensIssuer.name, `100000.0000 ABC`, ""], { from: tokensIssuer });
        await tokenContract.actions.transfer([tokensIssuer.name, tokensHolder.name, `100000.0000 ABC`, ""], { from: tokensIssuer });

        atomContract = await eoslime.Contract.deploy(ATOM_WASM_PATH, ATOM_ABI_PATH);
        await atomContract.makeInline();

        // Tables
        accountsTables = atomContract.tables.accounts;
        plansTables = atomContract.tables.plans;
        termsTables = atomContract.tables.terms;

        // Objects
        plan0Base = {
            cpu_credits: '10.0000 SYS',
            net_credits: '10.0000 SYS',
            ram_bytes: 100,
            price: `10.0000 XPR@${tokenContract.name}`
        }
        plan0 = {
            ...plan0Base,
            index: 0,
            plan_days: 30,
            max_quantity: 3,
            name: "",
            description: "",
            included: []
        }

        plan1Base = {
            ...plan0Base
        }
        plan1 = {
            ...plan1Base,
            index: 1,
            plan_days: 0,
            max_quantity: 3,
            name: "",
            description: "",
            included: []
        }

        plan2 = {
            ...plan0Base,
            index: 2,
            price: `50.0000 XPR@${tokenContract.name}`,
            plan_days: 30,
            max_quantity: 3,
            name: "",
            description: "",
            included: []
        }
    });

    it("Create Plan", async () => {
        await atomContract.actions.addplan([plan0])
        const plans = await plansTables.find();
        assert.deepStrictEqual(plans, [{
            ...plan0,
            price: {
                contract: tokenContract.name,
                quantity: "10.0000 XPR"
            }
        }])
    });

    it("Edit Plan", async () => {
        await atomContract.actions.addplan([plan0])
        await atomContract.actions.updateplan([{
            ...plan0,
            price: `20.0000 XPR@${tokenContract.name}`
        }])
        const plans = await plansTables.find();
        assert.deepStrictEqual(plans, [{
            ...plan0,
            price: {
                contract: tokenContract.name,
                quantity: "20.0000 XPR"
            }
        }])
    });

    it("Remove Plan", async () => {
        await atomContract.actions.addplan([plan0])
        await atomContract.actions.removeplan([0])
        const plans = await plansTables.find();
        assert.deepStrictEqual(plans, [])
    });

    it("Fails if plan created by not owner", async () => {
        await eoslime.tests.expectMissingAuthority(
            atomContract.actions.addplan([plan0], { from: tokensIssuer })
        )
    });

    it("Fails if plan removed by not owner", async () => {
        await atomContract.actions.addplan([plan0])
        await eoslime.tests.expectMissingAuthority(
            atomContract.actions.removeplan([0], { from: tokensIssuer })
        )
    });

    it("User deposit tokens", async () => {
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, ""], { from: tokensHolder });
        let accounts = await accountsTables.find()
        assert.deepStrictEqual(accounts, [{
            account: tokensHolder.name,
            balances: [
                {
                    key: {
                        contract: tokenContract.name,
                        sym: '4,XPR'
                    },
                    value: '100.0000 XPR'
                }
            ]
        }])

        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, "a"], { from: tokensHolder });
        accounts = await accountsTables.find()
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 ABC`, ""], { from: tokensHolder });
        accounts = await accountsTables.find()

        assert.deepStrictEqual(accounts, [{
            account: tokensHolder.name,
            balances: [
                {
                    key: {
                        contract: tokenContract.name,
                        sym: '4,ABC'
                    },
                    value: '100.0000 ABC'
                },
                {
                    key: {
                        contract: tokenContract.name,
                        sym: '4,XPR'
                    },
                    value: '200.0000 XPR'
                }
            ]
        }])
    });

    it("User withdraw partial tokens", async () => {
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, ""], { from: tokensHolder });
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 ABC`, ""], { from: tokensHolder });
        await atomContract.actions.withdraw([tokensHolder.name, `10.0000 XPR@${tokenContract.name}`], { from: tokensHolder });
        await atomContract.actions.withdraw([tokensHolder.name, `100.0000 ABC@${tokenContract.name}`], { from: tokensHolder });

        const accounts = await accountsTables.find()
        assert.deepStrictEqual(accounts, [{
            account: tokensHolder.name,
            balances: [
                {
                    key: {
                        contract: tokenContract.name,
                        sym: '4,XPR'
                    },
                    value: '90.0000 XPR'
                }
            ]
        }])
    });

    it("User withdraw all tokens", async () => {
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, ""], { from: tokensHolder });
        await atomContract.actions.withdraw([tokensHolder.name, `100.0000 XPR@${tokenContract.name}`], { from: tokensHolder });

        const accounts = await accountsTables.find()
        assert.deepStrictEqual(accounts, [])
    });

    it("Fail if user withdraws too many tokens", async () => {
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, ""], { from: tokensHolder });
        await eoslime.tests.expectAssert(
            atomContract.actions.withdraw([tokensHolder.name, `101.0000 XPR@${tokenContract.name}`], { from: tokensHolder })
        )
    });

    it("System deposit tokens with memo 'deposit' (no record)", async () => {
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, "deposit"], { from: tokensHolder });

        const accounts = await accountsTables.find()
        assert.deepStrictEqual(accounts, [])
    });

    it("Buy Plan", async () => {
        await atomContract.actions.addplan([plan0])
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, ""], { from: tokensHolder });
        await atomContract.actions.buyplan([tokensHolder.name, 0, 2], { from: tokensHolder })

        const terms = await termsTables.find();
        assert.deepStrictEqual(terms, [{
            ...plan0Base,
            price: {
                contract: tokenContract.name,
                quantity: '20.0000 XPR'
            },
            term_days: 60,
            account: tokensHolder.name,
            start_time: terms[0].start_time
        }])

        const accounts = await accountsTables.find();
        assert.deepStrictEqual(accounts, [{
            account: tokensHolder.name,
            balances: [
                {
                    key: {
                        contract: tokenContract.name,
                        sym: '4,XPR'
                    },
                    value: '80.0000 XPR'
                }
            ]
        }])
    });

    it("Fail if Buying more Terms than Plan Max", async () => {
        await atomContract.actions.addplan([plan0])
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, ""], { from: tokensHolder });
        await eoslime.tests.expectAssert(
            atomContract.actions.buyplan([tokensHolder.name, 0, 4], { from: tokensHolder })
        )
    })

    it("Upgrade Plan", async () => {
        await atomContract.actions.addplan([plan0])
        await atomContract.actions.addplan([plan2])
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `100.0000 XPR`, ""], { from: tokensHolder });
        await atomContract.actions.buyplan([tokensHolder.name, 0, 1], { from: tokensHolder })
        await atomContract.actions.buyplan([tokensHolder.name, 2, 1], { from: tokensHolder })

        const terms = await termsTables.find();
        assert.deepStrictEqual(terms, [{
            ...plan0Base,
            price: {
                contract: tokenContract.name,
                quantity: '50.0000 XPR'
            },
            account: tokensHolder.name,
            term_days: 30,
            start_time: terms[0].start_time
        }])

        const accounts = await accountsTables.find();
        assert.deepStrictEqual(accounts, [{
            account: tokensHolder.name,
            balances: [
                {
                    key: {
                        contract: tokenContract.name,
                        sym: '4,XPR'
                    },
                    value: '50.0000 XPR'
                }
            ]
        }])
    });

    it("Upgrade Plan to multi-month", async () => {
        await atomContract.actions.addplan([plan0])
        await atomContract.actions.addplan([plan2])
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `110.0000 XPR`, ""], { from: tokensHolder });
        await atomContract.actions.buyplan([tokensHolder.name, 0, 1], { from: tokensHolder })
        await atomContract.actions.buyplan([tokensHolder.name, 2, 2], { from: tokensHolder })

        const terms = await termsTables.find();
        assert.deepStrictEqual(terms, [{
            ...plan0Base,
            price: {
                contract: tokenContract.name,
                quantity: '100.0000 XPR'
            },
            account: tokensHolder.name,
            term_days: 60,
            start_time: terms[0].start_time
        }])

        const accounts = await accountsTables.find();
        assert.deepStrictEqual(accounts, [{
            account: tokensHolder.name,
            balances: [
                {
                    key: {
                        contract: tokenContract.name,
                        sym: '4,XPR'
                    },
                    value: '10.0000 XPR'
                }
            ]
        }])
    });

    it("Proces term that has ended", async () => {
        await atomContract.actions.addplan([plan1])
        await tokenContract.actions.transfer([tokensHolder.name, atomContract.name, `10.0000 XPR`, ""], { from: tokensHolder });
        await atomContract.actions.buyplan([tokensHolder.name, 1, 1], { from: tokensHolder })

        let terms = await termsTables.find()
        assert.deepStrictEqual(terms, [{
            ...plan1Base,
            account: tokensHolder.name,
            price: {
                contract: tokenContract.name,
                quantity: '10.0000 XPR'
            },
            term_days: 0,
            start_time: terms[0].start_time
        }])

        await atomContract.actions.process([10], { from: tokensHolder })
        terms = await termsTables.find()
        assert.deepStrictEqual(terms, [])
    })
});
