#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

#include "./UIColorPickerColorTable_Define.h"

#include <vector>
//-----------------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_COLORTABLE_SELECT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CUIColorPickerColorTable : public CUIGroup
{

public:
    enum
    {
       COLORPICKERCOLORTABLE_NONE = NO_ID + 1,

       COLORPICKERCOLORTABLE_BEGIN,
    };

public:
    enum
    {
        TABLE_NUM_WIDTH  = 16,
        TABLE_NUM_HEIGHT = 2,
    };

public:
    CUIColorPickerColorTable(EngineDeviceMan* pEngineDevice);
    virtual ~CUIColorPickerColorTable();

public:
    void	     CreateSubControl ();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void         SetColorTable ( std::vector<DWORD>& dwColorTable );

public:
    void         SetRestrict ( const BOOL bRestrict );
    const DWORD  GetColor() { return m_dwColor; }

private:
    CUIControl*  m_pColorTable[TABLE_NUM_HEIGHT][TABLE_NUM_WIDTH];

private:
    DWORD        m_dwColor;
};
