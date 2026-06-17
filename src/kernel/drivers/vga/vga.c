// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#include <kernel/drivers/vga/vga.h>
#include <kernel/x86/io.h>

int16_t g_ScreenX = 0;
int16_t g_ScreenY = 0;
static int16_t g_CursorX = -1;
static int16_t g_CursorY = -1;
volatile uint16_t* g_Screen_buff = (uint16_t*)0xB8000;


void vga_init(void)
{
    uint16_t pos;
    uint8_t low;
    uint8_t high;

    outb(0x3D4, 0x0F);
    low = inb(0x3D5);

    outb(0x3D4, 0x0E);
    high = inb(0x3D5);

    pos = (uint16_t)(low | (high << 8));

    // set cursor position vars, to current cursor pos
    g_ScreenX = pos % SCREEN_WIDTH;
    g_ScreenY = pos / SCREEN_WIDTH;

    // disable cursor, we use our own software cursor
    vga_change_cursor(VGA_CURSOR_DISABLED);
}


void vga_putc(uint8_t x, uint8_t y, int c, uint8_t attr)
{
    uint16_t cell = VGA_ENTRY(c, attr); 
    g_Screen_buff[(y * SCREEN_WIDTH) + x] = cell;
}


void vga_putchar(int c)
{
    // hide cursor, since we call setcursor at the bottom
    vga_cursor_hide();

    switch (c)
    {
        case '\t':
            vga_puts("    ");
            break;

        case '\n':
            g_ScreenX = 0;
            g_ScreenY++;
            break;
        
        case '\b':
            if (g_ScreenX > 0)
                g_ScreenX--;
            break;

        case '\r':
            g_ScreenX = 0;
            break;

        default:
            vga_putc(g_ScreenX, g_ScreenY, c, VGA_DEFAULT);
            g_ScreenX++;
            break;
    }


    if (g_ScreenX >= SCREEN_WIDTH)
    {
        g_ScreenX = 0;
        g_ScreenY++;
    }

    if (g_ScreenY >= SCREEN_HEIGHT)
        vga_scroll(1);


    vga_setcursor(g_ScreenX, g_ScreenY);
}

void vga_puts(const char* str)
{
    while (*str)
        vga_putchar(*str++);
}


char vga_getchar(int x, int y)
{
    return g_Screen_buff[(y * SCREEN_WIDTH) + x];
}




void vga_scroll(int lines)
{
    for (int y = lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            vga_putc(x, y - lines, vga_getchar(x, y), VGA_DEFAULT);

    for (int y = SCREEN_HEIGHT - lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            vga_putc(x, y, '\0', VGA_DEFAULT);

    g_ScreenY -= lines;
    vga_setcursor(g_ScreenX, g_ScreenY);
}



void vga_clear()
{
    for (uint8_t y=0; y < SCREEN_HEIGHT; y++)
        for (uint8_t x=0; x < SCREEN_WIDTH; x++)
            vga_putc(x, y, ' ', VGA_DEFAULT);

    // we set both to -1, to indicate cursor is no longer on screen
    // so that set cursos doesnt try to invert the cell where the cursor last was, before the clear
    g_CursorX = -1;
    g_CursorY = -1;

    g_ScreenX = 0;
    g_ScreenY = 0;
    vga_setcursor(g_ScreenX, g_ScreenY);
}



void vga_cursor_left(void)
{
    if (g_ScreenX > 0)
        g_ScreenX--;


    else if (g_ScreenY > 0)
    {
        // move cursor the far right of the upper line
        g_ScreenY--;
        g_ScreenX = SCREEN_WIDTH - 1;
    }

 
    vga_setcursor(g_ScreenX, g_ScreenY);
}
 

void vga_cursor_right(void)
{
    if (g_ScreenX < SCREEN_WIDTH - 1)
        g_ScreenX++;
    else
    {
        // move cursor to the far left of lower line
        g_ScreenX = 0;
        g_ScreenY++;
 
        if (g_ScreenY >= SCREEN_HEIGHT)
            vga_scroll(1);
    }
 
    vga_setcursor(g_ScreenX, g_ScreenY);
}


void vga_setcursor(int x, int y)
{
    vga_cursor_show(x, y);
}


void vga_change_cursor(VGA_CURSOR_SHAPE shape)
{
    uint8_t start; 
    uint8_t end;
 
    switch (shape)
    {
        case VGA_CURSOR_BLOCK:
            start = 0;
            end   = 15;
            break;
 
        case VGA_CURSOR_UNDERLINE:
            start = 14;
            end   = 15;
            break;
 
        case VGA_CURSOR_HALFBLOCK:
            start = 8;
            end   = 15;
            break;
 
        case VGA_CURSOR_TOPBAR:
            start = 0;
            end   = 1;
            break;
 
        case VGA_CURSOR_DISABLED:
        default:
            outb(0x3D4, 0x0A);
            outb(0x3D5, 0x20);  // bit 5 set disables cursor
            return;
    }
 
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | start);
 
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}



// this function inverts the bg and fg of a vga cell
static inline uint16_t vga_invert_attr(uint16_t cell)
{
    uint8_t ch   = cell & 0xFF;
    uint8_t attr = (cell >> 8) & 0xFF;
 
    uint8_t fg = attr & 0x0F;
    uint8_t bg = (attr >> 4) & 0x0F;
 
    uint8_t new_attr = (fg << 4) | bg;  // swap fg and bg nibbles
 
    return (uint16_t)ch | ((uint16_t)new_attr << 8);
}


// un inverts the cell showing the cursor
void vga_cursor_hide(void)
{
    // is the cursor currently drawn?
    if (g_CursorX < 0 || g_CursorY < 0)
        return;
 
    int idx = g_CursorY * SCREEN_WIDTH + g_CursorX;
    g_Screen_buff[idx] = vga_invert_attr(g_Screen_buff[idx]);
 
    // cursosx and cursory set to negative, meaning cursor is not shown
    g_CursorX = -1;
    g_CursorY = -1;
}



// inverts the cell at currnet x.y  to move it there
void vga_cursor_show(int x, int y)
{
    // is cursor actively already at this x.y?
    if (g_CursorX == x && g_CursorY == y)
        return; 
 
    vga_cursor_hide(); 
    // hide old cursor
 
    int idx = y * SCREEN_WIDTH + x;
    g_Screen_buff[idx] = vga_invert_attr(g_Screen_buff[idx]);
    //invert x.y where we want cursor to be at

    // udpdate cursorx and cursory
    g_CursorX = x;
    g_CursorY = y;
}