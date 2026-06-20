# ============================================================
# Makefile - NyxOS
# ============================================================

.PHONY: all clean toolchain kernel initramfs iso run test

all: toolchain kernel initramfs iso

toolchain:
	./tools/build.sh toolchain

kernel:
	./tools/build.sh kernel

initramfs:
	./tools/build.sh initramfs

iso:
	./tools/build.sh iso

run:
	./tools/qemu_launch.sh

test:
	cd testing && docker-compose up --build

clean:
	cd kernel && make clean
	cd modules && make clean
	rm -f iso/boot/nyx-kernel.bin
	rm -f iso/boot/initramfs.cpio.gz
	rm -f NyxOS.iso

distclean: clean
	rm -rf cross/