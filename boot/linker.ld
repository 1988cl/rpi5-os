/*
 * ARM64 QEMU virt 机器链接脚本
 * 内核加载地址: 0x40080000
 * RAM起始地址: 0x40000000
 */

/* 指定输出架构为 AArch64 */
OUTPUT_ARCH(aarch64)

/* 指定程序入口点为 _start */
ENTRY(_start)

/* 定义内存区域 */
MEMORY {
    /*
     * QEMU virt 机器的内存布局:
     * 0x00000000-0x3FFFFFFF: 设备内存映射区域
     * 0x40000000-0x4007FFFF: 固件/引导加载程序区域
     * 0x40080000-0x4FFFFFFF: 内核加载区域（256MB）
     * 0x80000000-0xFFFFFFFF: 扩展内存区域
     */
    RAM (rwx) : ORIGIN = 0x40080000, LENGTH = 256M
}

/* 定义程序段 */
SECTIONS {
    /*
     * 代码段 (.text)
     * 从内核加载地址开始
     */
    . = ORIGIN(RAM);

    .text : {
        /* 确保启动代码在最前面 */
        KEEP(*(.text.boot))

        /* 所有其他代码 */
        *(.text*)

        /* 确保代码段以4字节对齐 */
        . = ALIGN(4);
    } > RAM

    /*
     * 只读数据段 (.rodata)
     * 包含常量字符串、常量数组等
     */
    .rodata : {
        *(.rodata*)
        . = ALIGN(8);
    } > RAM

    /*
     * 已初始化数据段 (.data)
     * 包含已初始化的全局变量和静态变量
     */
    .data : {
        *(.data*)
        . = ALIGN(8);
    } > RAM

    /*
     * 未初始化数据段 (.bss)
     * 包含未初始化的全局变量和静态变量
     * 启动时需要清零
     */
    . = ALIGN(16);
    __bss_start = .;

    .bss : {
        *(.bss*)
        *(COMMON)      /* 公共变量 */
        . = ALIGN(8);
    } > RAM

    __bss_end = .;

    /*
     * 栈空间
     * 放在内存末尾，向下增长
     */
    . = ALIGN(16);
    stack_bottom = .;

    /* 分配16KB栈空间 */
    . = . + 0x4000;

    stack_top = .;

    /*
     * 堆空间（可选）
     * 如果需要动态内存分配，可以在这里定义
     */
    . = ALIGN(16);
    heap_start = .;

    /* 分配1MB堆空间 */
    . = . + 0x100000;

    heap_end = .;

    /*
     * 丢弃不需要的段
     * 这些段对裸机程序无用
     */
    /DISCARD/ : {
        *(.comment)      /* 编译器注释 */
        *(.note*)        /* 备注信息 */
        *(.eh_frame*)    /* 异常处理帧 */
        *(.ARM.attributes) /* ARM属性 */
    }
}
