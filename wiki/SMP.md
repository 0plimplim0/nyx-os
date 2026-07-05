# SMP (Symmetric Multi-Processing)

## Overview
- **Max CPUs:** 8 (`MAX_CPUS`)
- **Detection:** CPUID leaf 1 or leaf 0x80000008
- **APIC mode:** Local + I/O APIC
- **AP state:** All APs park in `cli; hlt`

## Bringup sequence

1. `smp_init()` records BSP as CPU 0, detects AP count
2. For each AP: sends INIT IPI → waits 10 ms → sends SIPI to 0x8000 (trampoline)
3. AP starts in real mode, climbs to long mode via the trampoline
4. AP calls `ap_main(cpu_id)` which records its APIC id via CPUID and parks

## Trampoline (`trampoline.asm`)
Position-independent real-mode stub compiled as flat binary and embedded via `ld -r -b binary`. Sets up segments, A20, protected mode, long mode, then jumps to `ap_main`.

## CPU topology (`cpus` command)
```sh
SMP: 4 CPU(s) online (max 8)
CPU  ROLE  APIC  SELF  STATE
0    BSP   0     0     online
1    AP    1     1     online
2    AP    2     2     online
3    AP    3     3     online
```

## Per-CPU data
```c
typedef struct {
    uint32_t apic_id;       // assigned by BSP
    uint32_t apic_id_self;  // read by core via CPUID
    uint32_t cpu_number;
    uint64_t stack_base;
    uint64_t stack_top;
    volatile int started;
    volatile int running;
} cpu_info_t;
```

## Future work
- Per-CPU run queues, reschedule IPIs, TLB shootdown, spinlocks, IRQ affinity
