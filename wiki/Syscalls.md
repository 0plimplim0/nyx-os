# System Calls

Ring-3 → ring-0 interface via `syscall`/`sysret` (x86_64 fast syscall).

## Convention
| Register | Purpose |
|----------|---------|
| RAX | Syscall number |
| RDI | arg0 |
| RSI | arg1 |
| RDX | arg2 |
| R10 | arg3 |
| R8 | arg4 |
| R9 | arg5 |
| Return | RAX |

## Syscall table
| # | Name | Description |
|---|------|-------------|
| 0 | `exit(code)` | Exit process, wake parent |
| 1 | `write(fd, buf, len)` | Write to fd |
| 2 | `print(msg)` | Debug print (legacy) |
| 3 | `open(path, flags)` | Open file |
| 4 | `read(fd, buf, len)` | Read from fd |
| 5 | `close(fd)` | Close fd |
| 6 | `getpid()` | Return PID |
| 7 | `sbrk(inc)` | Extend heap |
| 8 | `fsize(fd)` | File size |
| 9 | `exec(path)` | Spawn + wait |

## Safety
- **Pointer validation:** `user_ptr_ok` rejects kernel addresses
- **Opaque fds:** Small integers mapped to VFS handles (kernel pointers never exposed)
- **copy_from_user/copy_to_user:** Walks user page tables safely
- **Interrupt masking:** Syscalls run with IF=0
