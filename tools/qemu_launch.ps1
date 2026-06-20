# tools/qemu_launch.ps1
$iso = Join-Path $PSScriptRoot ".." "NyxOS.iso"
if (!(Test-Path $iso)) {
    Write-Error "No se encuentra $iso. Compila primero."
    exit 1
}
qemu-system-x86_64 -cdrom $iso -m 2G -smp 4 -netdev user,id=net0 -device e1000,netdev=net0 -display gtk