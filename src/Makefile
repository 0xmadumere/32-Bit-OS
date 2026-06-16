ASM?=
TARGET?=
CC?=
TARGET_CC= 			$(TARGET)-gcc
TARGET_CFLAGS= 		-ffreestanding -nostdlib -nostartfiles -fno-pic \
					-I~/opt/cross/lib/gcc/i686-elf/13.2.0/include -I.
TARGET_LD= 			$(TARGET)-gcc
TARGET_LIBS= 		-lgcc
TARGET_LINKFLAGS=	-T$(STAGE2_DIR)/stage2.ld -nostdlib
RWILDCARD=			$(foreach d,$(wildcard $1*),$(call RWILDCARD,$d/,$2) $(filter $(subst *,%,$2),$d))
SOURCES_C=          $(call RWILDCARD,,$(STAGE2_DIR)/*.c)
SOURCES_ASM=        $(call RWILDCARD,,$(STAGE2_DIR)/*.asm)

OBJECTS_C=      	$(patsubst $(STAGE2_DIR)/%.c, $(BIN_DIR)/stage2/c/%.obj, $(SOURCES_C))
OBJECTS_ASM=    	$(patsubst $(STAGE2_DIR)/%.asm, $(BIN_DIR)/stage2/asm/%.obj, $(SOURCES_ASM))
BIN_DIR?=			bin

DRIVE_DIR?=
STAGE1_DIR=			boot/stage1
STAGE2_DIR=			boot/stage2
MAP_DIR?= 



.PHONY: stage1 stage2


stage1: $(BIN_DIR)/stage1.bin
stage2: $(BIN_DIR)/stage2.bin


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


$(BIN_DIR)/stage2.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(MAP_DIR)/stage2.map -o $@ $^ $(TARGET_LIBS)
	@truncate -s 7680 $@

	@-sudo losetup -d /dev/loop0 || true
	@sudo losetup -fP $(DRIVE_DIR)/drive.img
	@sudo partprobe /dev/loop0
	@sudo dd if=$@ of=/dev/loop0p1 seek=1 bs=512 count=15 conv=notrunc 
	@-sudo losetup -d /dev/loop0 || true
	@echo "--> created  stage2.bin"