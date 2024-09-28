#include "stdafx.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "./Editor.h"
#include "./SequenceMenuObject.h"

#include "../SigmaCore/DebugInclude.h"

CSequenceMenuObject::CSequenceMenuObject()
{
    if( !CreatePopupMenu() )
        return;

    AppendMenu(
        MF_STRING,
        EBreakAllLinks,
        _T( "Break All Links To Object(s)" ) );

    vs::CSequenceObject* pObject = g_pEditor->m_pFocusObject;
    if( pObject->VHasProperty() )
    {
        AppendMenu(
            MF_STRING,
            EEnumProperty,
            _T( "Enum Property" ) );
    }
}

CSequenceMenuObject::~CSequenceMenuObject()
{
    DestroyMenu();
}

CSequenceMenuObject::SSelect CSequenceMenuObject::Open( const ic::Int2& sPos, CWnd* pParent )
{
    SSelect sSelect;

    sSelect.nIndex = (size_t)TrackPopupMenu(
        TPM_NONOTIFY | TPM_RETURNCMD,
		sPos.x,
        sPos.y,
        pParent );

    return sSelect;
}