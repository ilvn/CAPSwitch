/*
*   Keyboard indicator lights control functions
*   Written by Ilya O. Levin, http://www.literatecode.com
*/
#define SELF_TEST
#pragma warning(push,0)
#include <windows.h>
#include <winioctl.h>
#pragma warning(pop)

#define IOCTL_KBD_SET_STATE CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0002, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KBD_GET_STATE CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0010, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define TK_CAPSLOCK   4
#define TK_NUMLOCK    2
#define TK_SCROLLLOCK 1

#ifndef uint16_t
#define uint16_t unsigned short
#endif
#ifndef uint32_t
#define uint32_t unsigned long
#endif

typedef struct {
    uint16_t id;  
    uint16_t led;
} kbd_toggles_state;

/* -------------------------------------------------------------------------- */
HANDLE kbd_open(void)
{
    HANDLE hKb = INVALID_HANDLE_VALUE;

    if (DefineDosDevice (DDD_RAW_TARGET_PATH, "Kbd","\\Device\\KeyboardClass0"))
       hKb = CreateFile("\\\\.\\Kbd",GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);

    return hKb; 
} /* kbd_open */


/* -------------------------------------------------------------------------- */
BOOL kbd_close(HANDLE hKb)
{
    BOOL rv = FALSE;
    if ( (hKb != INVALID_HANDLE_VALUE) && 
        (DefineDosDevice (DDD_REMOVE_DEFINITION, "Kbd", NULL)) )
        rv = CloseHandle(hKb);

    return rv;
} /* kbd_close */


/* -------------------------------------------------------------------------- */
uint32_t kbd_getLED(HANDLE hKb, kbd_toggles_state *k)
{
    kbd_toggles_state buf;
    uint32_t sz = 0; 

    if ( (hKb != INVALID_HANDLE_VALUE) && (k != NULL))
        k->id = 0, k->led = 0,
        DeviceIoControl(hKb, IOCTL_KBD_GET_STATE,
        &buf, sizeof(buf), k, sizeof(buf), &sz, NULL);

    return sz;     
} /* kbd_getLED */

/* -------------------------------------------------------------------------- */
uint32_t kbd_setLED(HANDLE hKb, kbd_toggles_state *k)
{
    uint32_t sz = 0; 

    if ( (hKb != INVALID_HANDLE_VALUE) && (k != NULL))
        if (!DeviceIoControl(hKb, IOCTL_KBD_SET_STATE,
            k, sizeof(kbd_toggles_state), NULL, 0, &sz, NULL)) sz = 0;

    return sz;     
} /* kbd_setLED */


#ifdef SELF_TEST
#pragma warning(push, 0)
#include <stdio.h>
#pragma warning(pop)

int main(void)
{
    HANDLE h;
    kbd_toggles_state k = {0};
    uint32_t rc;

    if ( (h = kbd_open()) != INVALID_HANDLE_VALUE )
    {
        rc = kbd_getLED(h, &k);
        printf("kbd_getLED() - %d: id = %06x led = %06x\n", rc, k.id, k.led);

        k.led ^= TK_CAPSLOCK; /* toggle the Caps Lock LED */

        rc = kbd_setLED(h, &k);
        printf("kbd_setLED() - %d\n", rc);

        kbd_close(h);
    } else printf("keyboard access error\n");

    return 0;
} 

#endif
