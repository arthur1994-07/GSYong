#include "stdafx.h"
#include "vstscanf.h"

#define LEFT_BRACKET ('[' | ('a' - 'A')) /* 'lowercase' version */

int _vstscanf_s(const TCHAR* buf, const TCHAR* format, va_list argPtr)
{
    // Get an upper bound for the # of args
    int count = 0, temp = 0;
    const TCHAR* p = format;

    while(1)
    {
        TCHAR c = *(p++);

        if (c == 0) 
            break;

        if (count != temp)
        {
            if((c == _T('c') || c == _T('s') || c == LEFT_BRACKET))
                ++count;

            temp = count;
        }

        if (c == _T('%') && (p[0] != _T('*') && p[0] != _T('%'))) 
            ++count;
    }

    if (count <= 0)
        return 0;

    int result;

    // copy stack pointer
    _asm
    {
        mov esi, esp;
    }

    // push variable parameters pointers on stack
    for (int i = count - 1; i >= 0; --i)
    {
        _asm
        {
            mov eax, dword ptr[i];
            mov ecx, dword ptr [argPtr];
            mov edx, dword ptr [ecx+eax*4];
            push edx;
        }
    }

    int stackAdvance = (2 + count) * 4;

    _asm
    {
        // now push on the fixed params
        mov eax, dword ptr [format];
        push eax;
        mov eax, dword ptr [buf];
        push eax;

        // call _ftscanf, and more the result in to result
        call dword ptr [_stscanf_s];
        mov result, eax;

        // restore stack pointer
        mov eax, dword ptr[stackAdvance];
        add esp, eax;
        //mov esp, esi;
    }

    return result;
}