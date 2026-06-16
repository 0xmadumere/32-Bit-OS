ASM?=
TARGET?=
CC?=
BIN_DIR?=
DRIVE_DIR?=
STAGE1_DIR?= 	boot/stage1

.PHONY: stage1

stage1: $(BIN_DIR)/stage1.bin

$(BIN_DIR)/stage1.bin: $(STAGE1_DIR)/vbr.asm
	@$(ASM) $< -f bin -o $@ -I.
	@-sudo losetup -d /dev/loop0 || true
	@sudo losetup -fP $(DRIVE_DIR)/drive.img
	@sudo partprobe /dev/loop0
	@sudo dd if=$@ of=/dev/loop0p1 bs=1 seek=62 count=448 conv=notrunc >/dev/null
	@sudo losetup -d /dev/loop0
	@echo "--> created  stage1.bin"