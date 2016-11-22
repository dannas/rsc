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
       (func (result f64) (local f64) 
          (f64.sub (get_local 0) (f64.const 2)))
       (export "run" 0))`,
-2)
print('ALL TESTS PASS!!!');

