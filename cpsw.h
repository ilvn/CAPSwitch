/*
*   CAPSwitch
*   Switch keyboard layouts by using Caps Lock
*   Written by Ilya O. Levin, http://www.literatecode.com
*/
#define LIBCTINY
#pragma once
#if defined (_UNICODE)
#error Unicode is not supported
#endif
#if defined (_MBCS)
#error MBCS is not supported
#endif
#define VC_EXTRALEAN
#define _WIN32_WINNT 0x0555
#define  WIN32_LEAN_AND_MEAN
#pragma warning(push, 1)
#include <windows.h>
#include <winioctl.h>
#pragma warning(pop)
#pragma warning(disable:4090) /* different volatile qualifiers */
#pragma comment (lib, "user32")
#ifdef LIBCTINY
#pragma comment(linker, "/NODEFAULTLIB:libc")
#pragma comment(linker, "/NODEFAULTLIB:libcmt")
#pragma comment(lib, "libctiny")
#endif

#ifndef uint8_t
#define uint8_t unsigned char
#endif
#ifndef uint16_t
#define uint16_t unsigned short
#endif
#ifndef uint32_t
#define uint32_t unsigned long
#endif
#ifndef uint64_t
#define uint64_t unsigned __int64
#endif

#define USE_VAR(x)  ((x) == (x))

#define IOCTL_KBD_SET_STATE CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0002, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KBD_GET_STATE CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0010, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define TK_CAPSLOCK   4
#define TK_NUMLOCK    2
#define TK_SCROLLLOCK 1

#define CPSW_EVENT  "CAPSwitch_evnt"
#define CPSW_QUIT   42

#define POP(x,y)  MessageBox(NULL, x, "CAPSwitch", MB_OK|y)

#ifdef __cplusplus
extern "C" { 
#endif

    typedef struct { 
        HANDLE event;
        HHOOK  hook;
     } cpsw_context;

    typedef struct {
        uint16_t id;  
        uint16_t led;
    } kbd_toggles_state;

    void CALLBACK timerFn(HWND,UINT,UINT_PTR,DWORD);
    LRESULT CALLBACK khcbfn(int, WPARAM, LPARAM); 
    void capsOff(void);
    HANDLE kbd_open(void);
    BOOL kbd_close(HANDLE);
    uint32_t kbd_getLED(HANDLE, kbd_toggles_state *);
    uint32_t kbd_setLED(HANDLE, kbd_toggles_state *);     

#ifdef __cplusplus
}
#endif
