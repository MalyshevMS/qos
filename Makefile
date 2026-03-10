# User config
BUILD_DIR = build
SOURCES = \
kernel/kernel.cpp \
kernel/vga/vga.cpp \
kernel/dev/ports.cpp \
kernel/serial/serial.cpp \
kernel/mem/heap.cpp \
kernel/mem/new.cpp

BOOT_ASM = arch/x86/boot.asm

# Script
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

CXX = g++
LD = ld
NASM = nasm
OBJCOPY = objcopy
QEMU = qemu-system-x86_64

CXXFLAGS = -Iinclude -m32 -ffreestanding -nostdlib -fno-pie -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -c
LDFLAGS = -m elf_i386 -T linker.ld
ASMFLAGS = -f bin

TARGET = $(BUILD_DIR)/os.img
BOOT = $(BUILD_DIR)/boot.bin
KERNEL = $(BUILD_DIR)/kernel.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(BOOT): $(BOOT_ASM) | $(BUILD_DIR)
	$(NASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@

$(KERNEL_ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(KERNEL): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

$(TARGET): $(BOOT) $(KERNEL)
	cat $(BOOT) $(KERNEL) > $@

run: $(TARGET)
	$(QEMU) -drive format=raw,file=$(TARGET) -serial stdio

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean