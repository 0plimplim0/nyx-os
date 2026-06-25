param(
    [ValidateSet('gui','serial','net','debug')]
    [string]$Mode = 'gui',
    [switch]$Sound
)

$root = $PSScriptRoot
$kernel = "$root\kernel\nyx-kernel.bin"

if (!(Test-Path $kernel)) {
    Write-Host "[ERROR] nyx-kernel.bin not found. Run build.ps1 first." -ForegroundColor Red
    exit 1
}

# Try common QEMU install paths
$qemuPaths = @(
    "C:\Program Files\qemu",
    "C:\Program Files\qemu\i386",
    "$env:LOCALAPPDATA\Programs\qemu",
    "$env:USERPROFILE\scoop\apps\qemu\current"
)
foreach ($p in $qemuPaths) {
    if (Test-Path "$p\qemu-system-i386.exe") {
        $env:Path = "$p;$env:Path"
        break
    }
}

if (!(Get-Command qemu-system-i386 -ErrorAction SilentlyContinue)) {
    Write-Host "[ERROR] qemu-system-i386 not found." -ForegroundColor Red
    Write-Host "Install QEMU (https://www.qemu.org/download/#windows) or add it to PATH" -ForegroundColor Yellow
    exit 1
}

$argsList = @(
    "-kernel", $kernel,
    "-m", "256M",
    "-no-reboot"
)

switch ($Mode) {
    'gui'    { $argsList += "-display", "gtk" }
    'serial' { $argsList += "-nographic"; $argsList += "-serial", "stdio" }
    'net'    { $argsList += "-display", "gtk"; $argsList += "-nic", "user,model=rtl8139" }
    'debug'  { $argsList += "-display", "gtk"; $argsList += "-d", "cpu_reset,int" }
}

if ($Sound -or $Mode -eq 'net') {
    $argsList += "-audiodev", "dsound,id=audio0"
    $argsList += "-device", "sb16,audiodev=audio0"
}

Write-Host "=== Launching NyxOS (mode: $Mode) ===" -ForegroundColor Cyan
Write-Host "QEMU: $(qemu-system-i386 --version | Select-Object -First 1)" -ForegroundColor Gray

& qemu-system-i386 @argsList
