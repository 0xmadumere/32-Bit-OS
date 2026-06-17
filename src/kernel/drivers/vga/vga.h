// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <kernel/include/common.h>

#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25


// vga colors
#define VGA_COLOR_BLACK         0
#define VGA_COLOR_BLUE          1
#define VGA_COLOR_GREEN         2
#define VGA_COLOR_CYAN          3
#define VGA_COLOR_RED           4
#define VGA_COLOR_MAGENTA       5
#define VGA_COLOR_BROWN         6
#define VGA_COLOR_LIGHT_GREY    7
#define VGA_COLOR_DARK_GREY     8
#define VGA_COLOR_LIGHT_BLUE    9
#define VGA_COLOR_LIGHT_GREEN   10
#define VGA_COLOR_LIGHT_CYAN    11
#define VGA_COLOR_LIGHT_RED     12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_YELLOW        14
#define VGA_COLOR_WHITE         15


// bits 6-4 = background color
// bits 3-0 = foreground color
// returns full attr byte
#define VGA_ENTRY_COLOR(fg, bg) ((uint8_t)((bg) << 4) | (fg))


// a full text entry = attr << 8 | char
#define VGA_ENTRY(c, color) ((uint16_t)(c) | ((uint16_t)(color) << 8))


// some pre made stuff
#define VGA_DEFAULT VGA_ENTRY_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK)
#define VGA_PANIC   VGA_ENTRY_COLOR(VGA_COLOR_WHITE, VGA_COLOR_RED)
#define VGA_WARN    VGA_ENTRY_COLOR(VGA_COLOR_BLACK, VGA_COLOR_YELLOW)
#define VGA_OK      VGA_ENTRY_COLOR(VGA_COLOR_LIGHT_GREEN,VGA_COLOR_BLACK)
#define VGA_INFO    VGA_ENTRY_COLOR(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK)
#define VGA_DEBUG   VGA_ENTRY_COLOR(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK)

typedef enum
{
    VGA_CURSOR_BLOCK,      // thick boy cursor
    VGA_CURSOR_UNDERLINE,  // thin underline
    VGA_CURSOR_HALFBLOCK,  // bottom half block
    VGA_CURSOR_TOPBAR,     // thin bar at top
    VGA_CURSOR_DISABLED    // disabled
} VGA_CURSOR_SHAPE;


void vga_init(void);
void vga_setcursor(int x, int y);
char vga_getchar(int x, int y);
void vga_putc(uint8_t x, uint8_t y, int c, uint8_t attr);
void vga_putchar(int c);
void vga_puts(const char* str);
void vga_clear();
void vga_scroll(int lines);
void vga_cursor_left(void);
void vga_cursor_right(void);
void vga_change_cursor(VGA_CURSOR_SHAPE shape);
static inline uint16_t vga_invert_attr(uint16_t cell);
void vga_cursor_hide(void);
void vga_cursor_show(int x, int y);