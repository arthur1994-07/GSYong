#pragma once

namespace ic
{
    enum EMDXKEYFLAGS
    {
	    DXKEY_IDLE		= (0x0001),
	    DXKEY_DOWN		= (0x0002),
	    DXKEY_PRESSED	= (0x0004),
	    DXKEY_DRAG		= (0x0008),
	    DXKEY_UP		= (0x0010),
	    DXKEY_DUP		= (0x0020),

	    DXKEY_DOWNED	= (DXKEY_DOWN|DXKEY_PRESSED|DXKEY_DRAG),
    };

    enum DX_MOUSEKEYS
    {
	    DXMOUSE_LEFT	= 0,
	    DXMOUSE_RIGHT	= 1,
	    DXMOUSE_MIDDLE	= 2,
	    DXMOUSE_3		= 3,
	    DXMOUSE_4		= 4,
	    DXMOUSE_5		= 5,
	    DXMOUSE_6		= 6,
	    DXMOUSE_7		= 7,
    };


    class DxInputDeviceInterface
    {
    public:
        static DxInputDeviceInterface* g_pInstance;

    public:
        static void SetInstance( DxInputDeviceInterface* pInstance );
        static DxInputDeviceInterface* GetInstance();

    public:
        virtual DWORD GetKeyState ( BYTE ScanCode ) = 0;
        virtual DWORD GetMouseState ( ic::DX_MOUSEKEYS MKey ) = 0;
        virtual void GetMouseMove ( int &_ndx, int &_ndy, int &_ndz ) = 0;

    public:
        DxInputDeviceInterface();
        virtual ~DxInputDeviceInterface();
    };

};