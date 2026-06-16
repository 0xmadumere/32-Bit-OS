export ASM=			nasm
export TARGET=		i686-elf
export CC=			$(TARGET)-gcc
export BIN_DIR=		$(abspath bin)
export DRIVE_DIR=   $(abspath img)
export MAP_DIR= 	$(abspath map)

define GRUB_CFG
set default=0
set timeout=10
menuentry "32-Bit OS" {
    insmod part_msdos
    insmod chain
    chainloader +1
}
endef
export GRUB_CFG

.PHONY: all drive stage1

all: stage1 stage2

drive: $(DRIVE_DIR)/drive.img


$(DRIVE_DIR)/drive.img: always
	@-sudo umount /mnt/grub || true
	@-sudo losetup -d /dev/loop0 || true

	@dd if=/dev/zero of=$@ bs=1M count=300 >/dev/null 
	@printf "n\np\n1\n2048\n+200M\na\nw" | fdisk "$@" 
	@sudo losetup -fP $(DRIVE_DIR)/drive.img 
	@sudo partprobe /dev/loop0
	@sudo mkfs.fat -F 16 -R 16 -n "32-BIT OS" -h 2048 /dev/loop0p1 -S 512 -s 8 -M 0xF8 -i 0x7A5B8723 
	@sudo mkdir -p /mnt/grub 
	@sudo mount /dev/loop0p1 /mnt/grub 
	@sudo grub-install --target=i386-pc \
    --boot-directory=/mnt/grub/boot \
    --no-floppy \
    --modules="biosdisk part_msdos fat chain bufio crypto datetime extcmd gettext net terminal verifiers" \
    /dev/loop0 
	@echo "$$GRUB_CFG" | sudo tee /mnt/grub/boot/grub/grub.cfg
	@sudo umount /mnt/grub
	@sudo losetup -d /dev/loop0 
	@echo "--> created hdd image"


stage1: 
	@$(MAKE) -B -C src/ stage1


stage2: 
	@$(MAKE) -B -C src/ stage2


always:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DRIVE_DIR)


run:
	@sudo qemu-system-i386 -enable-kvm -drive format=raw,file=$(DRIVE_DIR)/drive.img,if=ide -boot c -cpu 486 -m 3.49G \
	-rtc base=localtime,clock=host,driftfix=slew


clean:
	@rm -rf $(BIN_DIR)
	@-sudo umount /mnt/grub 2>/dev/null || true
	@-sudo losetup -d /dev/loop0 2>/dev/null || true


debug:
	rm -rf img/drive.img.lock
	bochs -f bochs.txt