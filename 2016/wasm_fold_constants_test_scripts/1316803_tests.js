// Test script for bug 1316803
// 
// USAGE
//
//  Run all tests
//      js --wasm-always-baseline 1316803_tests.js -- all
//
//  List available tests
//      js --wasm-always-baseline  1316803_tests.js -- names
//      
//  Run individual test
//      js --wasm-always-baseline 1316803_tests.js -- <name-of-test>


// ========= Code copy-pasted from jit-test/lib/wasm.js ===============

// wasm can't return i64 since JS only has the "Number" type
// Some i64 values can't be exactly represented as ieee-754 doubles
// The wasm.test-mode is way to use *reified* i64 values, which are
// just object with two attributes 'low' (32 bits) and 'high' (32 bits)
setJitCompilerOption('wasm.test-mode', 1);

// Assert that the expected value is equal to the int64 value, as passed by
// Baldr: {low: int32, high: int32}.
// - if the expected value is in the int32 range, it can be just a number.
// - otherwise, an object with the properties "high" and "low".
function assertEqI64(observed, expect) {
    assertEq(typeof observed, 'object', "observed must be an i64 object");
    assertEq(typeof expect === 'object' || typeof expect === 'number', true,
             "expect must be an i64 object or number");

    let {low, high} = observed;
    if (typeof expect === 'number') {
        assertEq(expect, expect | 0, "in int32 range");
        assertEq(low, expect | 0, "low 32 bits don't match");
        assertEq(high, expect < 0 ? -1 : 0, "high 32 bits don't match"); // sign extension
    } else {
        assertEq(typeof expect.low, 'number');
        assertEq(typeof expect.high, 'number');
        assertEq(low, expect.low | 0, "low 32 bits don't match");
        assertEq(high, expect.high | 0, "high 32 bits don't match");
    }
}

function createI64(val) {
    let ret;
    if (typeof val === 'number') {
        assertEq(val, val|0, "number input to createI64 must be an int32");
        ret = {
            low: val,
            high: val < 0 ? -1 : 0 // sign extension
        };
    } else {
        assertEq(typeof val, 'string');
        assertEq(val.slice(0, 2), "0x");
        val = val.slice(2).padStart(16, '0');
        ret = {
            low: parseInt(val.slice(8, 16), 16),
            high: parseInt(val.slice(0, 8), 16)
        };
    }
    return ret;
}

function testInternal(assertValueFunc, text, expected, maybeImports, ...args) {
    let binary = wasmTextToBinary(text);
    assertEq(WebAssembly.validate(binary), true, "Must validate.");

    let module = new WebAssembly.Module(binary);
    let instance = new WebAssembly.Instance(module, maybeImports);
    assertEq(typeof instance.exports.run, 'function', "A 'run' function must be exported.");
    assertValueFunc(instance.exports.run(...args), expected, "Initial module must return the expected result.");
}

function test(text, expected, maybeImports, ...args) {
    testInternal(assertEq, text, expected, maybeImports, ...args);
}

function testI64(text, expected, maybeImports, ...args) {
    testInternal(assertEqI64, text, expected, maybeImports, ...args);
}

// ==============================================================

let testdata = [
    ['i32.add',   2, 1, 3 ],
    ['i64.add',   createI64(2), 1, createI64(3) ],
    ['i32.sub',   3, 2, 1 ],
    ['i64.sub',   createI64(3), 2, createI64(1) ],
    ['i32.shl',   1, 2, 4],
    ['i64.shl',   createI64(1), 2, createI64(4) ],
    ['i32.shr_s', 4, 2, 1],
    ['i64.shr_s', createI64(4), 2, createI64(1) ],
    ['i32.shr_u', 4, 2, 1],
    ['i64.shr_u', createI64(4), 2, createI64(1) ],
    ['i32.rotl',   1, 2, 4],
    ['i64.rotl',   createI64(1), 2, createI64(4) ],
    ['i32.rotr', 4, 2, 1],
    ['i64.rotr', createI64(4), 2, createI64(1) ],
    ['i32.and', 7, 2, 2],
    ['i64.and', createI64(7), 2, createI64(2) ],
    ['i32.or', 4, 2, 6],
    ['i64.or', createI64(4), 2, createI64(6) ],
    ['i32.xor', 7, 2, 5],
    ['i64.xor', createI64(7), 2, createI64(5) ],

    ['i32.div_s', 6, 2, 3],
    ['i32.div_u', 6, 2, 3],
    ['i32.rem_u', 7, 3, 1],
    ['i32.rem_s', 7, 3, 1],

    ['i64.div_s', createI64(6), 2, createI64(3)],
    ['i64.div_u', createI64(6), 2, createI64(3)],
    ['i64.rem_u', createI64(7), 3, createI64(1)],
    ['i64.rem_s', createI64(7), 3, createI64(1)],
];

function getNames() {
    let s = "";
    for (let t of testdata) {
        s += t[0] + " ";
    }
    return s;
}

function find(key) {
    for (let e of testdata) {
        if (e[0] == key)
            return e;
    }
    return null;
}

function runTest(testcase) {
    let [key, lhs, rhs, expected] = testcase;
    let [type, op] = key.split(/\./);

    let testfunc = type == 'i64' ? testI64 : test;

    testfunc(`
        (module 
           (func (param ${type}) (result ${type})
              (${type}.${op} (get_local 0) (${type}.const ${rhs})))
           (export "run" 0))`,
    expected, {}, lhs);
}

function runAllTests() {
    for (let t of testdata) {
        print(t);
        runTest(t);
    }
}

function runDivideInteractively() {
    String.prototype.isEmpty = function() {
        return this.length === 0 || !this.trim();
    }
    let testfunc = test;

    while (true) {
        let line = readline();
        if (line.isEmpty()) {
            break;
        }
        print("LINE: ", line);

        let parts = line.split(/ +/);
        let lhs = parseInt(parts[0], 10);
        let rhs = parseInt(parts[1], 10);
        let expected = parseInt(parts[2], 10);
        print(lhs, rhs);

        testfunc(`
            (module 
               (func (param i32) (result i32)
                  (i32.div_s (get_local 0) (i32.const ${rhs})))
               (export "run" 0))`,
        expected, {}, lhs);
    }
}


// Commandline args
let cmd = scriptArgs[scriptArgs.length - 1];

if (cmd == "all") {
    runAllTests();
    print("All tests pass!!!");;
} else if (cmd == "names") {
    print(getNames());
} else if (cmd == "divide") {
    runDivideInteractively();
} else {
    let t = find(cmd);
    if (t) {
        runTest(t);
        print(`${cmd} passed!`);
    } else {
        print(`ERROR: test ${cmd} not found`);
    }
}

