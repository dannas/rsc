#/bin/bash

JS="/home/dannas/code/firefox/js/src/build_DBG.OBJ/dist/bin/js"

OPERATIONS=$(${JS} 1316803_tests.js -- names)

SUFFIX=after

# Print 10 instruction disassembly of the function body.
# Requires that an breakpoint has been added in 
# js::wasm::WasmBaseCompiler::emitBody()
for OP in ${OPERATIONS} 
do 
    gdb \
        -q \
        -batch \
        --ex "set pagination off" \
        --ex "set logging file ${OP}_${SUFFIX}" \
        --ex "set logging overwrite" \
        --ex "run" \
        --ex "set logging on" \
        --ex 'x/10i $pc' \
        --ex "set logging off" \
        --ex 'continue' \
        --args ${JS} --wasm-always-baseline 1316803_tests.js -- ${OP}
done

# We only want the function body.
# Remove lines after the int3 instruction (which marks the end of
# the function body).
# Append an empty line at the end
for OP in ${OPERATIONS} 
do
    sed -i '/int3/,$d' ${OP}_${SUFFIX}
    sed -i '$a \ ' ${OP}_${SUFFIX}
done

grep . *_${SUFFIX} > assembly_optimized.txt

