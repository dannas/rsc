function assemble_and_disassemble() {
    local base=$(basename -s .asm $*)
    yasm -felf64 $base.asm -o $base.o
    objdump -S -Mintel $base.o | sed -n '/^0000/,$p'
}

function build_and_run() {
    local base=$(basename -s .cpp $*)
    g++ -Wall -g $base.cpp -o $base
    ./$base
}

alias a='assemble_and_disassemble experiments.asm'
alias c='build_and_run codegenerator.cpp'
