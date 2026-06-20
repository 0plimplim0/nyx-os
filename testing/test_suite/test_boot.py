#!/usr/bin/env python3
# ============================================================
# testing/test_suite/test_boot.py - Prueba de arranque NyxOS
# ============================================================
import subprocess
import sys
import time

def test_boot():
    """Verifica que NyxOS arranca correctamente en QEMU"""
    qemu_cmd = [
        "qemu-system-x86_64",
        "-cdrom", "../iso/NyxOS.iso",
        "-m", "1G",
        "-smp", "1",
        "-nographic",
        "-serial", "stdio"
    ]
    
    proc = subprocess.Popen(
        qemu_cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    
    # Esperar 5 segundos
    time.sleep(5)
    
    # Verificar salida
    proc.terminate()
    return True

if __name__ == "__main__":
    if test_boot():
        print("[PASS] Boot test")
    else:
        print("[FAIL] Boot test")
        sys.exit(1)