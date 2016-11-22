JS="/home/dannas/code/firefox/js/src/build_DBG.OBJ/dist/bin/js"
OPERATIONS="i32.add i64.add f32.add f64.add i32.sub i64.sub f32.sub f64.sub i32.shl i64.shl i32.shr_u i64.shr_u i32.shr_s"

# Print 10 instruction disassembly of the function body.
# Requires that an breakpoint has been added in 
# js::wasm::WasmBaseCompiler::emitBody()
for OP in ${OPERATIONS} 
do 
    gdb \
        -q \
        -batch \
        --ex "set pagination off" \
        --ex "set logging file ${OP}_before" \
        --ex "set logging overwrite" \
        --ex "run" \
        --ex "set logging on" \
        --ex 'x/10i $pc' \
        --ex "set logging off" \
        --ex 'continue' \
        --args ${JS} --wasm-always-baseline -f ${OP}.js
done

# We only want the function body.
# Remove lines after the int3 instruction (which marks the end of
# the function body).
# Append an empty line at the end
for OP in ${OPERATIONS} 
do
    sed -i '/int3/,$d' ${OP}_before
    sed -i '$a \ ' ${OP}_before
done

grep . *_before > assembly_not_optimized.txt

