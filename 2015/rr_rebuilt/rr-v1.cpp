// First attempt at reproducing rr.
//
// A learning experiment to improve my understanding of the rr source code.
//
// This version records syscalls but does not allow multiple threads; does not
// have any optimizations for limiting number of traps; does not records
// signals;
// does not provide any scheduler; and does not optimize writing of trace files.
//
// Relies on address randomization being disabled.
//
// Example usage:
//
//      $ echo 0 | sudo tee /proc/sys/kernel/randomize_va_spaces
//      0
//
//      $ ./rr-v1 ./hello
//      Hello world
//
//      $ cat /tmp/rr-log.txt
//      ENTER_SYSCALL 59 rdi=603048, rsi=603080, rdx=7fffffffddd0,
//      rax=ffffffffffffffda
//      EXIT_SYSCALL 59 rdi=0, rsi=0, rdx=0, rax=0
//      ENTER_SYSCALL 1 rdi=1, rsi=7fffffffdda8, rdx=c, rax=ffffffffffffffda
//      EXIT_SYSCALL 1 rdi=1, rsi=7fffffffdda8, rdx=c, rax=c
//      ENTER_SYSCALL 60 rdi=0, rsi=7fffffffdda8, rdx=0, rax=ffffffffffffffda
//
//      $ ./rr-v1 replay
//      Hello world
//
// TODO(dannas): Figure out why there's a mismatch in registers between record
// and replay.

#include "util.h"

FILE* log_fp = nullptr;

void run_target(int argc, char* argv[]) {
  std::vector<std::string> args(argv + 1, argv + argc);
  xptrace(PTRACE_TRACEME, 0, nullptr, nullptr);

  // Signal to tracer that we've started.
  kill(getpid(), SIGSTOP);

  xexecvp(args[0].c_str(), StringVectorToCharArray(args).get());
}

int wait_for_syscall(pid_t child, __ptrace_request request) {
  int status;
  while (true) {
    xptrace(request, child, 0, 0);
    waitpid(child, &status, 0);

    // Syscalls have the 8'th bit set.
    if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
      return 0;
    if (WIFEXITED(status))
      return 1;
  }
}

user_regs_struct get_regs(pid_t child) {
  user_regs_struct regs;
  xptrace(PTRACE_GETREGS, child, nullptr, &regs);
  return regs;
}

void set_regs(pid_t child, user_regs_struct& regs) {
  xptrace(PTRACE_SETREGS, child, nullptr, &regs);
}

void write_event(FILE* fp, const char* event, const user_regs_struct& regs) {

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
  fprintf(fp, "%s %llu rdi=%llx rsi=%llx rdx=%llx rax=%llx\n", event,
          regs.orig_rax, regs.rdi, regs.rsi, regs.rdx, regs.rax);
}

void read_event(std::string& type, user_regs_struct* regs) {
  char buf[BUFSIZ];
  int nread =
      fscanf(log_fp, "%s %llu rdi=%llx rsi=%llx rdx=%llx rax=%llx\n", buf,
             &regs->orig_rax, &regs->rdi, &regs->rsi, &regs->rdx, &regs->rax);
  if (nread != 6) {
    die("fscanf parsing error");
  }
  type = buf;
}

void record(pid_t child) {

  log_fp = fopen("/tmp/rr-log.txt", "w");

  int status;
  // Wait for SIGSTOP
  waitpid(child, &status, 0);

  // Allow us to distinguish system calls from other events.
  ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

  while (true) {
    if (wait_for_syscall(child, PTRACE_SYSCALL) != 0)
      break;
    write_event(log_fp, "ENTER_SYSCALL", get_regs(child));

    if (wait_for_syscall(child, PTRACE_SYSCALL) != 0)
      break;
    write_event(log_fp, "EXIT_SYSCALL", get_regs(child));
  }
}

void replay(pid_t child) {
  log_fp = fopen("/tmp/rr-log.txt", "r");
  int status;
  // Wait for SIGSTOP
  waitpid(child, &status, 0);

  // Allow us to distinguish system calls from other events.
  ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

  while (true) {
    std::string type;
    user_regs_struct r_regs;

    if (wait_for_syscall(child, PTRACE_SYSEMU) != 0)
      break;

    read_event(type, &r_regs);

    if (wait_for_syscall(child, PTRACE_SYSEMU_SINGLESTEP) != 0)
      break;

    auto regs = get_regs(child);

    read_event(type, &r_regs);
    regs.orig_rax = r_regs.orig_rax;
    regs.rdi = r_regs.rdi;
    regs.rsi = r_regs.rsi;
    regs.rdx = r_regs.rdx;
    regs.rax = r_regs.rax;
    set_regs(child, regs);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    die("Expected a program name as parameter\n");
  }

  pid_t pid = xfork();
  if (pid == 0) {
    run_target(argc, argv);
  } else {
    if (strcmp(argv[1], "replay") == 0)
      replay(pid);
    else
      record(pid);
  }
}
