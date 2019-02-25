# TODO(dannas): cset command appears to not be accepted. Is it because I've run it more than once?
# TODO(dannas): When I set scaling_governor, write fails for the two last cpus. Why? Because I've already ran the script once?

set -x
# Taken from https://llvm.org/docs/Benchmarking.html

# Disable address space randomization
echo 0 > /proc/sys/kernel/randomize_va_space

# Set scaling_governor to performance
for i in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
do
  echo performance > ${i}
done

# Disable turbo mode
echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo

# Use https://github.com/lpechacek/cpuset to reserve cpus for just the program
# you are benchmarking. If using perf, leave at least 2 cores so that perf runs
# in one and your program in another:
# This will move all threads out of N1 and N2. The -k on means that even kernel
# threads are moved out.
cset shield -c 1,2 -k on

# Disable the SMT pair of the cpus you will use for the benchmark. The pair of
# cpu N can be found in /sys/devices/system/cpu/cpuN/topology/thread_siblings_list 
# and disabled with:
#
# DANNAS: Gives 1,5 and 2,6 on my Dell XPS 15 laptop.
echo 0 > /sys/devices/system/cpu/cpu5/online
echo 0 > /sys/devices/system/cpu/cpu6/online

# =====
# Taken from pmu-tools
# https://github.com/andikleen/pmu-tools/wiki/toplev-manual
# Make sure we don't need to use sudo when invoking perf
echo -1 >/proc/sys/kernel/perf_event_paranoid

# The toplev tool requires the nmi_watchdog to be disabled
echo 0 > /proc/sys/kernel/nmi_watchdog
