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
    ['i32.rotl',   1, 2, 4],
    ['i64.rotl',   createI64(40), 34, createI64(160) ],
    ['i32.rotr', 4, 2, 1],
    ['i64.rotr', createI64(4), 2, createI64(1) ],
];

function runTest(key, lhs, rhs, expected) {
    let [type, op] = key.split(/\./);

    let testfunc = type == 'i64' ? testI64 : test;
    if (type === 'i64') {
        lhs = createI64(lhs);
        expected = createI64(expected);
    }

    testfunc(`
        (module 
           (func (param ${type}) (result ${type})
              (${type}.${op} (get_local 0) (${type}.const ${rhs})))
           (export "run" 0))`,
    expected, {}, lhs);
}


// Commandline args
let cmd = scriptArgs[scriptArgs.length - 4];
let lhs_s = scriptArgs[scriptArgs.length - 3];
let rhs_s = scriptArgs[scriptArgs.length - 2];
let expected_s = scriptArgs[scriptArgs.length - 1];

//let lhs = parseInt(lhs_s, 16);
let lhs = lhs_s;
let rhs = parseInt(rhs_s, 10);
//let expected = parseInt(expected_s, 16);
let expected = expected_s;

print(lhs, rhs, expected);

runTest(cmd, lhs, rhs, expected);

print(`${cmd} passed!`);

