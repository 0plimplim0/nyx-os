#!/bin/bash
export PATH="/mnt/c/Users/kzh/Desktop/Proyectos/nyx-os/cross/bin:$PATH"
cd /mnt/c/Users/kzh/Desktop/Proyectos/nyx-os/kernel && make clean && make 2>&1
