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
alias i='./build/interpreter-test'
alias ast='./build/assembler-test'
alias e='./build/exec-test'
alias cg='./build/codegenerator-test'
alias c='./build/compiler-test'
