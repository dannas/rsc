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

function wasmExecute(text, expected, ...args) {
    let binary = wasmTextToBinary(text);
    assertEq(WebAssembly.validate(binary), true, "Must validate.");

    let module = new WebAssembly.Module(binary);
    let instance = new WebAssembly.Instance(module, {});
    assertEq(typeof instance.exports.run, 'function', "A 'run' function must be exported.");
    assertEqI64(instance.exports.run(...args), expected, "Initial module must return the expected result.");
}

// wasm can't return i64 since JS only has the "Number" type
// Some i64 values can't be exactly represented as ieee-754 doubles
// The wasm.test-mode is way to use *reified* i64 values, which are
// just object with two attributes 'low' (32 bits) and 'high' (32 bits)
setJitCompilerOption('wasm.test-mode', 1);

wasmExecute(`
    (module 
       (func (result i64) (local i64) 
          (i64.add (get_local 0) (i64.const 2))) 
       (export "run" 0))`,
    {low: 2, high: 0});
print('ALL TESTS PASS!!!');

