#include <videoDriver.h>
#include <keyboard.h>
#include <lib.h>
#include <time.h>
#include <sound.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define SYS_CALLS_QTY 19

extern uint8_t hasregisterInfo;
extern const uint64_t registerInfo[17];
extern int _hlt();

extern Color WHITE;
extern Color BLACK;

static uint64_t sys_read(uint64_t fd, char *buff)
{
    if (fd != 0)
    {
        return -1;
    }

    *buff = getCharFromKeyboard();
    return 0;
}

static int sys_drawCursor()
{
    vDriver_drawCursor();
    return 1;
}

static uint64_t sys_write(uint64_t fd, char buffer)
{
    if (fd != 1)
    {
        return -1;
    }

    vDriver_print(buffer, WHITE, BLACK);
    return 1;
}

static uint64_t sys_writeColor(uint64_t fd, char buffer, Color color)
{
    if (fd != 1)
    {
        return -1;
    }

    vDriver_print(buffer, color, BLACK);
    return 1;
}

static uint64_t sys_clear()
{
    vDriver_clear();
    return 1;
}

static uint64_t sys_getScrHeight()
{
    return vDriver_getHeight();
}

static uint64_t sys_getScrWidth()
{
    return vDriver_getWidth();
}

static void sys_drawRectangle(int x, int y, int x2, int y2, Color color)
{
    vDriver_drawRectangle(x, y, x2, y2, color);
}

static void sys_wait(int ms)
{
    if (ms > 0)
    {
        int start_ms = ms_elapsed();
        do
        {
            _hlt();
        } while (ms_elapsed() - start_ms < ms);
    }
}

static uint64_t sys_getHours()
{
    return getHours();
}

static uint64_t sys_getMinutes()
{
    return getMinutes();
}

static uint64_t sys_getSeconds()
{
    return getSeconds();
}

static uint64_t sys_registerInfo(uint64_t registers[17])
{
    if (hasregisterInfo)
    {
        for (uint8_t i = 0; i < 17; i++)
        {
            registers[i] = registerInfo[i];
        }
    }
    return hasregisterInfo;
}

static uint64_t sys_printmem(uint64_t *address)
{
    if ((uint64_t)address > (0x20000000 - 32))
    {
        return -1;
    }

    uint8_t *aux = (uint8_t *)address;
    vDriver_prints("\n", WHITE, BLACK);
    for (int i = 0; i < 32; i++)
    {
        vDriver_printHex((uint64_t)aux, WHITE, BLACK);
        vDriver_prints(" = ", WHITE, BLACK);
        vDriver_printHex(*aux, WHITE, BLACK);
        vDriver_newline();
        aux++;
    }

    return 0;
}

static uint64_t sys_pixelPlus()
{
    plusScale();
    return 1;
}

static uint64_t sys_pixelMinus()
{
    minusScale();
    sys_clear();
    return 1;
}

static uint64_t sys_playSpeaker(uint32_t frequence, uint64_t duration)
{
    beep(frequence, duration);
    return 1;
}

static uint64_t sys_stopSpeaker()
{
    stopSpeaker();
    return 1;
}

uint64_t syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax)
{
    uint8_t r, g, b;
    Color color;
    switch (rax)
    {
    case 0:
        return sys_read(rdi, (char *)rsi);
    case 1:
        return sys_write(rdi, (char)rsi);
    case 2:
        return sys_clear();
    case 3:
        return sys_getHours();
    case 4:
        return sys_getMinutes();
    case 5:
        return sys_getSeconds();
    case 6:
        return sys_getScrHeight();
    case 7:
        return sys_getScrWidth();
    case 8:
        r = (r8 >> 16) & 0xFF;
        g = (r8 >> 8) & 0xFF;
        b = r8 & 0xFF;
        color.r = r;
        color.g = g;
        color.b = b;
        sys_drawRectangle(rdi, rsi, rdx, r10, color);
        return 1;
    case 9:
        sys_wait(rdi);
        return 1;
    case 10:
        return sys_registerInfo((uint64_t *)rdi);
    case 11:
        return sys_printmem((uint64_t *)rdi);
    case 12:
        return sys_pixelPlus();
    case 13:
        return sys_pixelMinus();
    case 14:
        return sys_playSpeaker((uint32_t)rdi, rsi);
    case 15:
        return sys_stopSpeaker();
    case 16:
        return sys_drawCursor();
    case 17:
        r = (rdx >> 16) & 0xFF;
        g = (rdx >> 8) & 0xFF;
        b = rdx & 0xFF;
        color.r = r;
        color.g = g;
        color.b = b;
        return sys_writeColor(rdi, (char)rsi, color);
    default:
        return 0;
    }
}