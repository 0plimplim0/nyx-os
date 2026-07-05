# Drivers

## Keyboard (`keyboard.c`)
PS/2 IRQ 1, circular buffer, US/ES layouts with AltGr, Set 2 scancodes.

## Mouse (`mouse.c`)
PS/2 IRQ 12, 3-byte packets, relative movement, 4 counts/mm.

## Timer (`timer.c`)
PIT channel 0, 1000 Hz via I/O APIC pin 2 → vector 32. Drives `tick_count`, scheduler, wait queue.

## RTC (`rtc.c`)
CMOS ports 0x70/0x71, BCD decoded. Used by `date` command.

## Sound Blaster 16 (`sb16.c`)
I/O 0x220, IRQ 5, DMA channel 1, 8-bit PCM, auto-init mode. Mixer control.

## PC Speaker (`speaker.c`)
PIT channel 2 square wave. `beep()`, `play_melody()`, shell commands `beep`/`play`.

## RTL8139 (`rtl8139.c`)
PCI NIC, I/O + MMIO BAR, 4 TX descriptors, circular RX ring, 10/100 Mbps, link detection.

## VBE (`vbe.c`)
Bochs VBE, up to 1024×768×32, LFB at 0xE0000000, custom CRTC.

## ATA (`ata.c`)
PIO mode, primary channel 0x1F0, IRQ 14, 28-bit LBA, polled I/O. Used by EXT2.

## APIC (`apic.c`)
Local APIC at 0xFEE00000, I/O APIC at 0xFEC00000, 24 redirection entries, ISA IRQ overrides.
