#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"

#include "../../../RanLogic/GLColorTable.h"

#include "UIColorPickerColorTable.h"

//#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIColorPickerColorTable::CUIColorPickerColorTable(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_dwColor(0)
{
    for ( int i=0; i<TABLE_NUM_HEIGHT; ++i )
    {
        memset( m_pColorTable[i], NULL, TABLE_NUM_WIDTH*sizeof(CUIControl*) );
    }
}

CUIColorPickerColorTable::~CUIColorPickerColorTable ()
{
}

void CUIColorPickerColorTable::CreateSubControl ()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    CUIControl* pColorBase = new CUIControl(m_pEngineDevice);
    pColorBase->Create ( NO_ID, "COLORPICKER_COLORTABLE_BASE", UI_FLAG_DEFAULT );

    const UIRECT& rcBaseLocalRect     = pColorBase->GetLocalPos(); 
    const UIRECT& rcParentGlobalRect  = GetGlobalPos(); 

    for ( int i=0; i<TABLE_NUM_HEIGHT; ++i )
    {
        for ( int j=0; j<TABLE_NUM_WIDTH; ++j )
        {
            FLOAT fPosX = rcBaseLocalRect.sizeX*(float)j;
            FLOAT fPosY = rcBaseLocalRect.sizeY*(float)i;

            CUIControl* pControl = new CUIControl(m_pEngineDevice);
            pControl->CreateSub ( this, "COLORPICKER_COLORTABLE_BASE", UI_FLAG_DEFAULT, COLORPICKERCOLORTABLE_BEGIN+i*TABLE_NUM_WIDTH+j );
            pControl->SetLocalPos  ( D3DXVECTOR2(fPosX, fPosY) );
            pControl->SetGlobalPos ( D3DXVECTOR2(rcParentGlobalRect.left+fPosX, rcParentGlobalRect.top+fPosY) );
            pControl->SetUseRender ( TRUE );
            pControl->SetDiffuse (0);
            RegisterControl( pControl );
            m_pColorTable[i][j] = pControl;
        }
    }

    delete pColorBase;

    SetColorTable( GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_DEFAULT );
}

void CUIColorPickerColorTable::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIColorPickerColorTable::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    if ( COLORPICKERCOLORTABLE_BEGIN <= ControlID && ControlID <= COLORPICKERCOLORTABLE_BEGIN+TABLE_NUM_WIDTH*TABLE_NUM_HEIGHT )
    {
        if ( CHECK_MOUSE_IN ( dwMsg ) && CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
        {
            DWORD dwIndex = ControlID - COLORPICKERCOLORTABLE_BEGIN;
            DWORD nY      = dwIndex/TABLE_NUM_WIDTH;
            DWORD nX      = dwIndex%TABLE_NUM_WIDTH;

            m_dwColor = m_pColorTable[nY][nX]->GetDiffuse();
            AddMessageEx(UIMSG_COLORTABLE_SELECT);           
        }
    }

    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CUIColorPickerColorTable::SetRestrict ( const BOOL bRestrict )
{
    if ( bRestrict )
    {
        SetColorTable( GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT );
    }
    else
    {
        SetColorTable( GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_DEFAULT );
    }
}

void CUIColorPickerColorTable::SetColorTable ( std::vector<DWORD>& dwColorTable )
{
    if ( (DWORD)dwColorTable.size() < (DWORD)(TABLE_NUM_HEIGHT*TABLE_NUM_WIDTH) )
    {
        return;
    }

    for ( int i=0; i<TABLE_NUM_HEIGHT; ++i )
    {
        for ( int j=0; j<TABLE_NUM_WIDTH; ++j )
        {
            m_pColorTable[i][j]->SetDiffuse ( dwColorTable[i*TABLE_NUM_WIDTH+j]);
        }
    }
}
