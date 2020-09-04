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

    const TOTAL_SUPPLY = "1000000000.0000 SYS";
    const HOLDER_SUPPLY = "100.0000 SYS";

    before(async () => {
        let accounts = await eoslime.Account.createRandoms(2);
        tokensIssuer = accounts[0];
        tokensHolder = accounts[1];
    });

    beforeEach(async () => {
        tokenContract = await eoslime.Contract.deploy(TOKEN_WASM_PATH, TOKEN_ABI_PATH);
        await tokenContract.actions.create([tokensIssuer.name, TOTAL_SUPPLY]);

        atomContract = await eoslime.Contract.deploy(ATOM_WASM_PATH, ATOM_ABI_PATH);
    });

    it("Should print account name in transaction console (hi)", async () => {
        const { processed: { action_traces }} = await atomContract.actions.hi([tokensIssuer.name]);

        assert.equal(action_traces.length, 1, "Incorrect number of action traces");
        assert.equal(action_traces[0].console, tokensIssuer.name, "Incorrect printed name");
    });
});
