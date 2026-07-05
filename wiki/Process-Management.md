# Process Management

Preemptive weighted round-robin scheduler. Fixed process table (64 entries).

## Process states
`PROC_RUN` → `PROC_BLOCKED` → `PROC_ZOMBIE` → `PROC_EMPTY`

## Scheduler
- Each process has `sched_weight` (1–64) and `sched_quantum` countdown
- PIT fires at 1000 Hz; ticks decrement quantum, switch when quantum hits 0
- Compositor gets weight 4 for responsive GUI

## Kernel threads
Created via `create_task(func, stack)`. Run in ring 0 with kernel PML4.

## User processes
Ring 3 with own PML4. Lifecycle: spawn → schedule → exit → reap.

## Blocking
- `kwait(pid)`: block until child exits
- `sleep(ms)`: block with `wake_tick`, scheduler wakes on timer tick
- `cli;hlt` yield pattern ensures atomic check-then-sleep

## Preemption safety
`preempt_disable/enable` guards kmalloc, kfree, EXT2 ops, VFS node pool.
