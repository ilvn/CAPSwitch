/*
*   CAPSwitch
*   Switch keyboard layouts by using Caps Lock
*   Written by Ilya O. Levin, http://www.literatecode.com
*/
#include "cpsw.h"

volatile cpsw_context g = {0};

/* -------------------------------------------------------------------------- */
int PASCAL WinMain (HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpCmd,int nShow)
{
    MSG m;
    uint8_t i;

    USE_VAR(hInstance); USE_VAR(hPrev); USE_VAR(nShow); /* hush the warnings */

    i = (uint8_t) lstrlen(lpCmd) & 255; /* check for the '/q' parameter */
    while (i) if ( ((lpCmd[i--] & 0xDF) == 'Q') && (lpCmd[i] == '/') )
    { i = CPSW_QUIT; break;}

    if ( (g.event = CreateEvent(NULL,TRUE,FALSE, CPSW_EVENT)) != NULL )
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            if (i == CPSW_QUIT) SetEvent(g.event), Sleep(1000);
            else POP("Already active",MB_ICONINFORMATION);
        }
        else
        {
            if ( SetTimer(NULL, 0, 1000, timerFn) != 0)
            { 
                capsOff();
                g.hook = SetWindowsHookEx(WH_KEYBOARD_LL, khcbfn, hInstance,0);
                if (g.hook)
                {
                    while (GetMessage(&m,0,0,0)) 
                        TranslateMessage(&m), DispatchMessage(&m);
                    UnhookWindowsHookEx(g.hook);
                }
                else POP("Failed to hook a keyboard",MB_ICONERROR);
            }
            else POP("Failed to set a timer", MB_ICONERROR);
        }
        CloseHandle(g.event);
    }
    else POP("Failed to create an event", MB_ICONERROR);

    return 0;
} /* WinMain */


/* -------------------------------------------------------------------------- */
void CALLBACK timerFn(HWND hWnd,UINT msg,UINT_PTR evnt,DWORD dwTime) 
{
    USE_VAR(hWnd); USE_VAR(msg); USE_VAR(evnt); USE_VAR(dwTime);
    if (WaitForSingleObject(g.event,0) == WAIT_OBJECT_0) PostQuitMessage(0);
} /* timerFn */


/* -------------------------------------------------------------------------- */
LRESULT CALLBACK khcbfn(int ncode, WPARAM wparam, LPARAM lparam)
{
    PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lparam;
    HWND hW = NULL;
    HANDLE h;
    kbd_toggles_state k;

    if ( (ncode == HC_ACTION) && (wparam == WM_KEYDOWN) && 
        (p->vkCode == VK_CAPITAL) && ((hW = GetForegroundWindow()) != NULL) )
    {
        PostMessage(hW, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)HKL_NEXT);
        if ( (h = kbd_open()) != INVALID_HANDLE_VALUE )
        {
            if (kbd_getLED(h, &k)) k.led ^= TK_CAPSLOCK, kbd_setLED(h, &k);
            kbd_close(h);
        }
    }

    return( (hW) ? 1 : CallNextHookEx(NULL, ncode, wparam, lparam) );
} /* khcbfn */


/* -------------------------------------------------------------------------- */
void capsOff(void)
{
    if ( GetKeyState(VK_CAPITAL) & 1 )
    {
        keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
        keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
    }
} /* capsOff*/


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
