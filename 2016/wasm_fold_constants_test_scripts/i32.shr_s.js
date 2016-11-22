function wasmExecute(text, expected, ...args) {
    let binary = wasmTextToBinary(text);
    assertEq(WebAssembly.validate(binary), true, "Must validate.");

    let module = new WebAssembly.Module(binary);
    let instance = new WebAssembly.Instance(module, {});
    assertEq(typeof instance.exports.run, 'function', "A 'run' function must be exported.");
    assertEq(instance.exports.run(...args), expected, "Initial module must return the expected result.");
}

wasmExecute(`
    (module 
       (func (param i32) (result i32)
          (i32.shr_s (get_local 0) (i32.const 1)))
       (export "run" 0))`,
0xC0000000, 0x80000000)
print('ALL TESTS PASS!!!');

