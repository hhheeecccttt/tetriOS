/*
 * kernel.c – Mode 13h minimal graphics kernel
 * 320x200x256 framebuffer at 0xA0000
 */

#define VGA ((unsigned char*)0xA0000)

#define SCREEN_W 320
#define SCREEN_H 200

extern char font8x8_basic[128][8];

/* Draw one pixel */
static inline void put_pixel(int x, int y, unsigned char color)
{
    VGA[y * SCREEN_W + x] = color;
}

/* Draw one character (8x8 font) */
static void draw_char(int x, int y, char c, unsigned char color)
{
    for (int row = 0; row < 8; row++)
    {
        unsigned char bits = font8x8_basic[(unsigned char)c][row];

        for (int col = 0; col < 8; col++)
        {
            if (bits & (1 << col))
            {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

/* Print string in graphics mode */
static void print_string(int x, int y, const char *s, unsigned char color)
{
    while (*s)
    {
        if (*s == '\n')
        {
            y += 8;
            x = 0;
        }
        else
        {
            draw_char(x, y, *s, color);
            x += 8;
        }

        s++;
    }
}

void fill_screen(unsigned char color)
{
    unsigned char *vga = (unsigned char*)0xA0000;

    for (int i = 0; i < 320 * 200; i++)
        vga[i] = color;
}

void kernel_main(void)
{
    /* Clear screen (fill with black) */
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++)
        VGA[i] = 0;

    /* Print text */
    fill_screen(18);
    print_string(10, 10, "KERNEL & BOOT OK", 2);        // green

    /* Halt */
    for (;;)
        __asm__ volatile ("hlt");
}