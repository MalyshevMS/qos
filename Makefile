# User config
BUILD_DIR = build
PYTHON_DIR = py
DISK_SIZE = 512 # Bytes TODO: Add more types (like K, M, G, etc.)
DISK_MSG_1 = "Hello, from disk 1" # Only appears in disk_msg section
DISK_MSG_2 = "Hello, from disk 2"

SOURCES = \
kernel/kernel.cpp \
kernel/console.cpp \
kernel/vga/vga.cpp \
kernel/vga/vconsole.cpp \
kernel/serial/serial.cpp \
kernel/mem/heap.cpp \
kernel/mem/new.cpp \
kernel/hardware/power.cpp \
kernel/multitask/task.cpp \
kernel/runtime.cpp \
arch/x86/idt.cpp \
arch/x86/pic.cpp \
arch/x86/gdt.cpp \
arch/x86/exceptions.cpp \
driver/acpi.cpp \
driver/pci.cpp \
driver/timer.cpp \
driver/keyboard.cpp \
driver/disk.cpp \
driver/sata.cpp \
klib/mem.cpp \
klib/cstring.cpp \
klib/string.cpp

ASM_SOURCES = \
arch/x86/idt_load.asm \
arch/x86/gdt_load.asm \
arch/x86/tss.asm \
arch/x86/irq_handler.asm \
arch/x86/exception_handler.asm \
arch/x86/switch_to_user.asm \
arch/x86/multiboot_header.asm \
kernel/user_test.asm

# Script
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES)) $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

CXX = g++
LD = ld
NASM = nasm
PY = python3
DD = dd
OBJCOPY = objcopy
GRUB_MKRESCUE = grub-mkrescue
QEMU = qemu-system-x86_64

CXXFLAGS = -Iinclude -std=c++20 -m32 -ffreestanding -nostdlib -fno-pie -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-stack-protector -fno-use-cxa-atexit -c
LDFLAGS = -m elf_i386 -T linker.ld
ASMFLAGS = -f bin

TARGET_ELF = $(BUILD_DIR)/kernel.elf
TARGET_ISO = $(BUILD_DIR)/os.iso
TARGET_DISK_1 = $(BUILD_DIR)/disk_1.img
TARGET_DISK_2 = $(BUILD_DIR)/disk_2.img

all: $(TARGET_ISO)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@printf " CXX\t$<\n"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.asm | $(BUILD_DIR)
	@printf " NASM\t$<\n"
	@mkdir -p $(dir $@)
	@$(NASM) -f elf32 $< -o $@

$(TARGET_ELF): $(OBJECTS)
	@printf " LD\t$@\n"
	@$(LD) $(LDFLAGS) -o $@ $^

$(TARGET_ISO): $(TARGET_ELF)
	@printf " GRUB\t$@\n"
	@mkdir -p $(BUILD_DIR)/isodir/boot/grub
	@cp $(TARGET_ELF) $(BUILD_DIR)/isodir/boot/kernel.elf
	@cp grub.cfg $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	@$(GRUB_MKRESCUE) -o $@ $(BUILD_DIR)/isodir > /dev/null 2>&1

$(TARGET_DISK_1): | $(BUILD_DIR)
	@printf " DISK\t$@\n"
	@qemu-img create -f raw $@ $(DISK_SIZE) > /dev/null

$(TARGET_DISK_2): | $(BUILD_DIR)
	@printf " DISK\t$@\n"
	@qemu-img create -f raw $@ $(DISK_SIZE) > /dev/null

disk_random: $(TARGET_DISK_1) $(TARGET_DISK_2)
	@printf " DD\t$@\n"
	@$(DD) if=/dev/random of=$(TARGET_DISK_1) bs=$(DISK_SIZE) count=1 > /dev/null
	@$(DD) if=/dev/random of=$(TARGET_DISK_2) bs=$(DISK_SIZE) count=1 > /dev/null

disk_msg_1: $(TARGET_DISK_1)
	@printf " PY\t$@\n"
	@$(PY) $(PYTHON_DIR)/disk_msg.py $(TARGET_DISK_1) $(DISK_MSG_1) $(DISK_SIZE) > /dev/null

disk_msg_2: $(TARGET_DISK_2)
	@printf " PY\t$@\n"
	@$(PY) $(PYTHON_DIR)/disk_msg.py $(TARGET_DISK_2) $(DISK_MSG_2) $(DISK_SIZE) > /dev/null

run: $(TARGET_ISO) $(TARGET_DISK_1) $(TARGET_DISK_2)
	@printf " QEMU\t$(TARGET_ISO)\n"
	@$(QEMU) -cdrom $(TARGET_ISO) \
        -enable-kvm \
        -serial stdio \
        -device ahci,id=ahci \
		-device usb-ehci,id=ehci \
        \
        -drive file=$(TARGET_DISK_1),format=raw,id=disk1,if=none \
        -device ide-hd,drive=disk1,bus=ahci.0 \
		\
		-drive file=$(TARGET_DISK_2),format=raw,id=disk2,if=none \
        -device ide-hd,drive=disk2,bus=ahci.1
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean disk_random disk_msg_1 disk_msg_2