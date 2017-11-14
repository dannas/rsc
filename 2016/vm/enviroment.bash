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

alias gs='git status -s .'
alias gd='git diff .'

alias gd='git diff .'
alias m='make -C build -j8 -s'
alias t='make -C build -j8 -s test'
alias i='./build/interpreter_unittest'
alias ast='./build/assembler_unittest'
alias e='./build/exec_integrationtest'
alias cg='./build/codegenerator_unittest'
alias c='./build/compiler_unittest'
