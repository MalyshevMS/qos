BUILD_DIR = build
ISO_DIR = $(BUILD_DIR)/iso

CXX = g++
LD = ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T linker.ld

all: $(BUILD_DIR)/qos.iso

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: kernel/boot.asm | $(BUILD_DIR)
	nasm -f elf64 $< -o $@

$(BUILD_DIR)/multiboot.o: kernel/multiboot.asm | $(BUILD_DIR)
	nasm -f elf64 $< -o $@

$(BUILD_DIR)/kernel.o: kernel/kernel.cpp | $(BUILD_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.bin: \
$(BUILD_DIR)/boot.o \
$(BUILD_DIR)/multiboot.o \
$(BUILD_DIR)/kernel.o
	$(LD) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/qos.iso: $(BUILD_DIR)/kernel.bin
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BUILD_DIR)/kernel.bin $(ISO_DIR)/boot/
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $@ $(ISO_DIR)

run: all
	qemu-system-x86_64 \
	-cdrom $(BUILD_DIR)/qos.iso \
	-m 1G \
	-serial stdio

clean:
	rm -rf $(BUILD_DIR)