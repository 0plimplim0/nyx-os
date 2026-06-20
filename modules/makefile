# ============================================================
# modules/Makefile - Compilación de módulos NyxOS
# ============================================================

TARGET = i686-elf
CC = $(TARGET)-gcc

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra \
         -nostdlib -nostartfiles -nodefaultlibs \
         -fno-stack-protector -fno-pie -m32 \
         -I../kernel

all: rootkit.o backdoor.o keylogger.o injector.o scanner.o \
     reaver.o exploit_loader.o hydra_brute.o c2_server.o \
     ransomware.o cryptominer.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o