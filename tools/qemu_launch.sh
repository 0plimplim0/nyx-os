#!/bin/bash
ISO="${1:-NyxOS.iso}"
qemu-system-x86_64 -cdrom "$ISO" -m 2G -smp 4 -display gtk