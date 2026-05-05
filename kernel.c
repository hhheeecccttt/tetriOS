/*
 * kernel.c – Mode 13h minimal graphics kernel
 * 320x200x256 framebuffer at 0xA0000
 */

volatile unsigned char *VGA = (volatile unsigned char*)0xA0000;

#define SCREEN_W 320
#define SCREEN_H 200
#define COLS 10
#define ROWS 15

unsigned int board[ROWS][COLS];

extern char font8x8_basic[128][8];

/* Draw one pixel */
static inline void put_pixel(int x, int y, unsigned char color) {
    if ((unsigned)x >= 320 || (unsigned)y >= 200)
        return;

    VGA[y * SCREEN_W + x] = color;
}

static void draw_thick_point(int x, int y, int t, unsigned char color) {
    for (int dy = -t; dy <= t; dy++)
        for (int dx = -t; dx <= t; dx++)
            put_pixel(x + dx, y + dy, color);
}

/* Draw one character (8x8 font) */
static void draw_char(int x, int y, char c, unsigned char color) {
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
static void print_string(int x, int y, const char *s, unsigned char color) {
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

void fill_screen(unsigned char color) {
    unsigned char *vga = (unsigned char*)0xA0000;

    for (int i = 0; i < 320 * 200; i++)
        vga[i] = color;
}

static void draw_line(int x0, int y0, int x1, int y1, int t, unsigned char color) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;

    int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1)
    {
        draw_thick_point(x0, y0, t, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err * 2;

        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

static void fill_box(int x, int y, int w, int h, unsigned char color) {
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            put_pixel(x + i, y + j, color);
}

void kernel_main(void)
{
    int holdLeft = 55;
    int holdRight = 103;

    int nextRight = 265;

    int gridLeft = 103;
    int gridRight = 217;

    int gridBottom = 187;
    int holdBottom = 45;

    /* Clear screen (fill with black) */
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++)
        VGA[i] = 0;

    /* Print text */
    fill_screen(18);
    fill_box(holdLeft, 20, 50, 20, 22);
    print_string(holdLeft + 6, 27, "HOLD", 15);
    draw_line(holdLeft, 20, holdRight, 20, 1, 15);
    draw_line(holdLeft, 20, holdLeft, 65, 1, 15);
    draw_line(holdLeft, 40, holdRight, 40, 1, 15);
    draw_line(holdLeft, 65, holdLeft + 5, 70, 1, 15);
    draw_line(holdLeft + 5, 70, holdRight, 70, 1, 15);

    fill_box(gridRight, 20, 50, 20, 22);
    print_string(gridRight + 6, 27, "NEXT", 15);
    draw_line(gridRight, 20, nextRight, 20, 1, 15);
    draw_line(gridRight, 40, nextRight, 40, 1, 15);
    draw_line(nextRight, 20, nextRight, 140, 1, 15);
    draw_line(nextRight, 140, nextRight - 5, 145, 1, 15);
    draw_line(gridRight, 145, nextRight - 5, 145, 1, 15);

    for (int r = 20; r <= gridBottom; r += 11) {
        for (int c = gridLeft; c < gridRight; c++) {
            put_pixel(c, r, 22);
        }
    }

    for (int c = 105; c <= 220; c += 11) {
        for (int r = 20; r < gridBottom; r++) {
            put_pixel(c, r, 22);
        }
    }

    draw_line(gridLeft, 20, gridLeft, gridBottom, 1, 15);
    draw_line(gridRight, 20, gridRight, gridBottom, 1, 15);
    draw_line(gridLeft, gridBottom, gridRight, gridBottom, 1, 15);

    /* Halt */
    for (;;)
        __asm__ volatile ("hlt");
}