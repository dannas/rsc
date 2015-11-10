// First attempt at reproducing rr.
//
// A learning experiment to improve my understanding of the rr source code.
//
// This version records syscalls but does not allow multiple threads; does not
// have any optimizations for limiting number of traps; does not records signals;
// does not provide any scheduler; and does not optimize writing of trace files.
//
// TODO(dannas): Add replay command

#include "util.h"

void run_target(int argc, char* argv[])
{
    xpersonality(ADDR_NO_RANDOMIZE);
    std::vector<std::string> args(argv+1, argv+argc);
    xptrace(PTRACE_TRACEME, 0, 0, 0);

    // Signal to tracer that we've started.
    kill(getpid(), SIGSTOP);

    // TODO(dannas): The addresses for execvp are not consistent across runs,
    // despite ADDR_NO_RANDOMIZE being set. Why? Because the stack is allocated
    // before the personality() call? Tried calling personality from main() but
    // the addresses were still unconsistent.
    xexecvp(args[0].c_str(), StringVectorToCharArray(args).get());
}

int wait_for_syscall(pid_t child)
{
    int status;
    while (true) {
        xptrace(PTRACE_SYSCALL, child, 0, 0);
        waitpid(child, &status, 0);

        // Syscalls have the 8'th bit set.
        if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
            return 0;
        if (WIFEXITED(status))
            return 1;
    }
}

user_regs_struct get_regs(pid_t child)
{
    user_regs_struct regs;
    xptrace(PTRACE_GETREGS, child, nullptr, &regs);
    return regs;
}

void write_event(const char* event, const user_regs_struct& regs)
{

    // The Linux/x86-64 kernel expects the system call parameters in
    // registers according to the following table:
    //
    //  syscall number	rax
    //  arg 1		rdi
    //  arg 2		rsi
    //  arg 3		rdx
    //  arg 4		r10
    //  arg 5		r8
    //  arg 6		r9
    //
    //  The Linux kernel uses and destroys internally these registers:
    //  return address from
    //  syscall		rcx
    //  eflags from syscall	r11
    //
    printf("%s %llu rdi=%llx, rsi=%llx, rdx=%llx, rax=%llx\n",
           event, regs.orig_rax, regs.rdi, regs.rsi, regs.rdx, regs.rax);
}

void record(pid_t child)
{
    int status;

    // Wait for SIGSTOP
    waitpid(child, &status, 0);

    // Allow us to distinguish system calls from other events.
    ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

    while (true) {
        if (wait_for_syscall(child) != 0) 
            break;
        write_event("ENTER_SYSCALL", get_regs(child));

        if (wait_for_syscall(child) != 0) 
            break;
        write_event("EXIT_SYSCALL", get_regs(child));
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        die("Expected a program name as parameter\n");
    }

    pid_t pid = xfork();
    if (pid == 0) {
        run_target(argc, argv);
    } else {
        record(pid);
    }
}

