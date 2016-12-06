// NAME
//      disasm - prints assembly for a wast function
//
// SYNOPSIS
//      js --wasm-always-baseline disasm.js -- FNAME
//
// DESCRIPTION
//      Print the assembly generated for the supplied Wast file.
//
//  BUGS
//      Should take a commandline parameter for wast file
//      Add  arrows for jumps
//
// SEE ALSO
//      https://github.com/mbebenita/WasmExplorer/blob/gh-pages/explorer.js#L342
//      https://bugzilla.mozilla.org/show_bug.cgi?id=1309583
//
load('capstone.min.js');

function padRight(s, n, c) {
    while (s.length < n) {
        s = s + c;
    }
    return s;
}

function padLeft(s, n, c) {
    while (s.length < n) {
        s = c + s;
    }
    return s;
}

function toAddress(n) {
    var s = n.toString(16);
    while (s.length < 6) {
        s = "0" + s;
    }
    return "0x" + s;
}

function toBytes(a) {
    return a.map(function (x) { return padLeft(Number(x).toString(16), 2, "0"); }).join(" ");
}


function disassemble(text) {
    let binary = wasmTextToBinary(text);
    let module = new WebAssembly.Module(binary);

    let buf = wasmExtractCode(module);

    // Initialize the decoder
    var cs = new capstone.Cs(capstone.ARCH_X86, capstone.MODE_64);

    for (let segment of buf.segments) {

        if (segment.kind != 0) {  // TODO(dannas): What do the different kinds represent?
            continue;
        }

        // Output: Array of capstone.Instruction objects
        var instructions = cs.disasm(buf.code, segment.funcBodyBegin, segment.funcBodyEnd);

        let s = "";
        instructions.forEach(function(instr) {
                  s += padRight(instr.mnemonic + " " + instr.op_str, 38, " ");
                  s += "; " + toAddress(instr.address) + " " + toBytes(instr.bytes) + "\n";
                });
        print(s);
    }

    cs.delete();
}

let fname = scriptArgs[scriptArgs.length - 1];
let text = read(fname);
disassemble(text);

