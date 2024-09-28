#include "stdafx.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "../VisualScriptLib/Logic/SequenceEvent.h"
#include "../VisualScriptLib/Logic/SequenceVariable.h"
#include "../VisualScriptLib/Logic/SequenceAction.h"

#include "./Editor.h"
#include "./SequenceMenuBackground.h"

#include "../SigmaCore/DebugInclude.h"

CSequenceMenuBackground::CSequenceMenuBackground()
{
    CreatePopupMenu();
    m_cMenuEvent.CreatePopupMenu();
    m_cMenuVariable.CreatePopupMenu();
    m_cMenuAction.CreatePopupMenu();

    AppendMenu(
        MF_STRING | MF_POPUP,
        (UINT_PTR)m_cMenuEvent.GetSafeHmenu(),
        vs::CSequenceEvent::Static()->GetClassName().c_str() );

    AppendMenu(
        MF_STRING | MF_POPUP,
        (UINT_PTR)m_cMenuVariable.GetSafeHmenu(),
        vs::CSequenceVariable::Static()->GetClassName().c_str() );

    AppendMenu(
        MF_STRING | MF_POPUP,
        (UINT_PTR)m_cMenuAction.GetSafeHmenu(),
        vs::CSequenceAction::Static()->GetClassName().c_str() );

    std::vector< ic::CBaseObject* > vecObject;
    for( size_t i=0; i<ic::CRttiMgr::Count(); ++i )
    {
        ic::CBaseObject* pStatic = ic::CRttiMgr::FindStatic( i );
        vs::CSequenceObject* pObject = dynamic_cast< vs::CSequenceObject* >( pStatic );
        if( !pObject )
            continue;

        if( pObject->VFlags( ic::EInternal ) )
            continue;

        __gassert_( pObject->VIndex() < ESequenceObjectMax );

        if( pObject->IsChildOf( vs::CSequenceEvent::Rtti() ) ||
            pObject->IsChildOf( vs::CSequenceVariable::Rtti() ) ||
            pObject->IsChildOf( vs::CSequenceAction::Rtti() ) )
        {
            vecObject.push_back( pStatic );
        }
    }
    std::sort( vecObject.begin(), vecObject.end(), ic::CompBaseObject() );

    BOOST_FOREACH( ic::CBaseObject* pStatic, vecObject )
    {
        vs::CSequenceObject* pObject = dynamic_cast< vs::CSequenceObject* >( pStatic );

        if( pObject->IsChildOf( vs::CSequenceEvent::Rtti() ) )
        {
            m_cMenuEvent.AppendMenu(
                MF_STRING,
                pObject->VIndex(),
                pObject->GetClassName().c_str() );
        }
        else if( pObject->IsChildOf( vs::CSequenceVariable::Rtti() ) )
        {
            m_cMenuVariable.AppendMenu(
                MF_STRING,
                pObject->VIndex(),
                pObject->GetClassName().c_str() );
        }
        else if( pObject->IsChildOf( vs::CSequenceAction::Rtti() ) )
        {
            m_cMenuAction.AppendMenu(
                MF_STRING,
                pObject->VIndex(),
                pObject->GetClassName().c_str() );
        }
    }

    AppendMenu( MF_SEPARATOR, NULL, _T( "" ) );
    AppendMenu( MF_STRING, ENewComment, _T( "New Comment" ) );
    AppendMenu( MF_SEPARATOR, NULL, _T( "" ) );
    AppendMenu( MF_STRING, ECreateNewSequence, _T( "Create New Sequence" ) );
    AppendMenu( MF_STRING, EPasteHere, _T( "Paste Here" ) );
}

CSequenceMenuBackground::~CSequenceMenuBackground()
{
    m_cMenuAction.DestroyMenu();
    m_cMenuVariable.DestroyMenu();
    m_cMenuEvent.DestroyMenu();
    DestroyMenu();
}

CSequenceMenuBackground::SSelect CSequenceMenuBackground::Open( const ic::Int2& sPos, CWnd* pParent )
{
    SSelect sSelect;

    sSelect.nIndex = (size_t)TrackPopupMenu(
        TPM_NONOTIFY | TPM_RETURNCMD,
		sPos.x,
        sPos.y,
        pParent );

    return sSelect;
}