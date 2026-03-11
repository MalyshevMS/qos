# User config
BUILD_DIR = build
SOURCES = \
kernel/kernel.cpp \
kernel/vga/vga.cpp \
kernel/io/ports.cpp \
kernel/serial/serial.cpp \
kernel/mem/heap.cpp \
kernel/mem/new.cpp \
arch/x86/idt.cpp \
arch/x86/pic.cpp \
driver/keyboard.cpp

ASM_SOURCES = \
arch/x86/idt_load.asm \
arch/x86/irq_handler.asm

BOOT_ASM = arch/x86/boot.asm

# Script
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES)) $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

CXX = g++
LD = ld
NASM = nasm
OBJCOPY = objcopy
QEMU = qemu-system-x86_64

CXXFLAGS = -Iinclude -m32 -ffreestanding -nostdlib -fno-pie -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-stack-protector -c
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

$(BUILD_DIR)/%.o: %.asm | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(NASM) -f elf32 $< -o $@

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