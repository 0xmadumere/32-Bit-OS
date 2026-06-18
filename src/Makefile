ASM?=
TARGET?=
CC?=
TARGET_CC= 			$(TARGET)-gcc
TARGET_CFLAGS= 		-ffreestanding -nostdlib -nostartfiles -fno-pic \
					-I~/opt/cross/lib/gcc/i686-elf/13.2.0/include -I.
TARGET_LD= 			$(TARGET)-gcc
TARGET_LIBS= 		-lgcc
STAGE2_LINKFLAGS=	-T$(STAGE2_DIR)/stage2.ld -nostdlib

KERNEL_TARGET?=
KERNEL_CC?=
KERNEL_CFLAGS=      -ffreestanding -s -nostdlib -fno-pic -I.
KERNEL_LD=          $(KERNEL_TARGET)-gcc
KERNEL_LINKFLAGS=   -nostdlib -T$(KERNEL_DIR)/kernel.ld -Wl,-e,bootstrap_entry -Wl,--subsystem,native -Wl,-s -Wl,--image-base=0xC0000000
KERNEL_LIBS= 		-lgcc

RWILDCARD=			$(foreach d,$(wildcard $1*),$(call RWILDCARD,$d/,$2) $(filter $(subst *,%,$2),$d))

STAGE2_SOURCES_C=   $(call RWILDCARD,,$(STAGE2_DIR)/*.c)
STAGE2_SOURCES_ASM= $(call RWILDCARD,,$(STAGE2_DIR)/*.asm)
STAGE2_OBJECTS_C=   $(patsubst $(STAGE2_DIR)/%.c, $(BIN_DIR)/stage2/c/%.obj, $(STAGE2_SOURCES_C))
STAGE2_OBJECTS_ASM= $(patsubst $(STAGE2_DIR)/%.asm, $(BIN_DIR)/stage2/asm/%.obj, $(STAGE2_SOURCES_ASM))

KERNEL_SOURCES_C= 	$(call RWILDCARD,,$(KERNEL_DIR)/*.c)
KERNEL_SOURCES_ASM= $(call RWILDCARD,,$(KERNEL_DIR)/*.asm)
KERNEL_OBJECTS_C= 	$(patsubst $(KERNEL_DIR)/%.c, $(BIN_DIR)/kernel/c/%.obj, $(KERNEL_SOURCES_C))
KERNEL_OBJECTS_ASM= $(patsubst $(KERNEL_DIR)/%.asm, $(BIN_DIR)/kernel/asm/%.obj, $(KERNEL_SOURCES_ASM))


BIN_DIR?=			
DRIVE_DIR?=
STAGE1_DIR=			boot/stage1
STAGE2_DIR=			boot/stage2
KERNEL_DIR= 		kernel
MAP_DIR?= 



.PHONY: stage1 stage2 kernel


stage1: $(BIN_DIR)/stage1.bin
stage2: $(BIN_DIR)/stage2.bin
kernel: $(BIN_DIR)/kernel.bin


$(BIN_DIR)/stage1.bin: $(STAGE1_DIR)/vbr.asm
	@$(ASM) $< -f bin -o $@ -I.
	@-sudo losetup -d /dev/loop0 || true
	@sudo losetup -fP $(DRIVE_DIR)/drive.img
	@sudo partprobe /dev/loop0
	@sudo dd if=$@ of=/dev/loop0p1 bs=1 seek=62 count=448 conv=notrunc >/dev/null
	@sudo losetup -d /dev/loop0
	@echo "--> created  stage1.bin"



$(BIN_DIR)/stage2/c/%.obj: $(STAGE2_DIR)/%.c
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(BIN_DIR)/stage2/asm/%.obj: $(STAGE2_DIR)/%.asm
	@mkdir -p $(@D)
	@$(ASM) -f elf32 -I. -o $@ $<


$(BIN_DIR)/stage2.bin: $(STAGE2_OBJECTS_ASM) $(STAGE2_OBJECTS_C)
	@$(TARGET_LD) $(STAGE2_LINKFLAGS) -Wl,-Map=$(MAP_DIR)/stage2.map -o $@ $^ $(TARGET_LIBS)
	@truncate -s 7680 $@

	@-sudo losetup -d /dev/loop0 || true
	@sudo losetup -fP $(DRIVE_DIR)/drive.img
	@sudo partprobe /dev/loop0
	@sudo dd if=$@ of=/dev/loop0p1 seek=1 bs=512 count=15 conv=notrunc 
	@-sudo losetup -d /dev/loop0 || true
	@echo "--> created  stage2.bin"



$(BIN_DIR)/kernel/c/%.obj: $(KERNEL_DIR)/%.c
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(BIN_DIR)/kernel/asm/%.obj: $(KERNEL_DIR)/%.asm
	@mkdir -p $(@D)
	@$(ASM) -f win32 -I. -o $@ $<


$(BIN_DIR)/kernel.bin: $(KERNEL_OBJECTS_ASM) $(KERNEL_OBJECTS_C)
	@$(KERNEL_LD) $(KERNEL_LINKFLAGS) -Wl,-Map=$(MAP_DIR)/kernel.map -o $@ $^ $(KERNEL_LIBS)

	@-sudo umount /mnt/kernel || true
	@-sudo losetup -d /dev/loop0 || true
	@sudo losetup -fP $(DRIVE_DIR)/drive.img
	@sudo partprobe /dev/loop0

	@sudo mount /dev/loop0p1 /mnt/kernel
	@sudo cp $(BIN_DIR)/kernel.bin /mnt/kernel

	@sudo losetup -d /dev/loop0
	@sudo umount /mnt/kernel
	@echo "--> created  kernel.bin"