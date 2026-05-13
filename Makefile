# ========== 1. Toolchain Setup ==========
# Define the cross-compiler for ARM64
CROSS_COMPILE = aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc   # C compiler
AS = $(CROSS_COMPILE)as    # Assembler
LD = $(CROSS_COMPILE)ld    # Linker
OBJCOPY = $(CROSS_COMPILE)objcopy  # Convert ELF to binary

# ========== 2. Compilation Flags ==========
# Basic flags for bare-metal ARM64 kernel
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -mgeneral-regs-only -g
LDFLAGS = -nostdlib -T boot/linker.ld -g

# ========== 3. Files and Directories ==========
# List all source files
BOOT_SRC = boot/boot.S
KERNEL_SRCS = kernel/main.c kernel/kprintf.c kernel/kdtb.c kernel/kstring.c kernel/kmm.c
DRIVER_SRCS = drivers/kuart.c


# Convert .c files to .o files
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o) $(DRIVER_SRCS:.c=.o)
BOOT_OBJ = boot/boot.o

# Final output files
TARGET = kernel8.elf
IMAGE = kernel8.img

# ========== 4. Build Rules ==========
# Default target: build everything
all: $(IMAGE)

# Create binary image from ELF file
$(IMAGE): $(TARGET)
	$(OBJCOPY) -O binary $< $@
	@echo "✓ Created binary: $@"

# Link all object files into ELF
$(TARGET): $(BOOT_OBJ) $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "✓ Linked: $@"

# Compile boot assembly code
boot/boot.o: boot/boot.S
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "✓ Compiled: $<"

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@
	@echo "✓ Compiled: $<"

# ========== 5. Useful Commands ==========
# Run in QEMU
run: $(IMAGE)
	@echo "Starting QEMU..."
	qemu-system-aarch64 \
		-M virt \
		-cpu cortex-a72 \
		-display none \
		-serial stdio \
		-kernel $(IMAGE)

# Clean up build files
clean:
	rm -f $(TARGET) $(IMAGE) *.o boot/*.o kernel/*.o drivers/*.o test/*.o
	@echo "✓ Cleaned all build files"

# Quick test: build and run
test: all run

# Show file sizes
size: $(TARGET) $(IMAGE)
	@echo "File sizes:"
	@ls -lh $(TARGET) $(IMAGE) 2>/dev/null || echo "Files not found"

# Show this help message
help:
	@echo "Available commands:"
	@echo "  make all     - Build everything (default)"
	@echo "  make run     - Build and run in QEMU"
	@echo "  make clean   - Remove all build files"
	@echo "  make test    - Build and test in QEMU"
	@echo "  make size    - Show file sizes"
	@echo "  make help    - Show this message"

# Debug target: add debug symbols
debug: CFLAGS += -g
debug: all
	@echo "✓ Built with debug symbols"

# Disassemble the binary
disasm: $(TARGET)
	$(CROSS_COMPILE)objdump -d $< | less

# ========== 6. Special Targets ==========
# Tell make these are "phony" targets (not real files)
.PHONY: all run clean test size help debug disasm host-test
