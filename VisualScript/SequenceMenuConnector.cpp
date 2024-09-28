#include "stdafx.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "../VisualScriptLib/Logic/SequenceEvent.h"
#include "../VisualScriptLib/Logic/SequenceVariable.h"

#include "./Editor.h"
#include "./SequenceMenuConnector.h"

#include "../SigmaCore/DebugInclude.h"

CSequenceMenuConnector::CSequenceMenuConnector()
{
    CreatePopupMenu();
    m_cMenuBreakLinkTo.CreatePopupMenu();

    AppendMenu(
        MF_STRING,
        EBreakAllLinks,
        _T( "Break All Links" ) );

    AppendMenu(
        MF_STRING | MF_POPUP,
        (UINT_PTR)m_cMenuBreakLinkTo.GetSafeHmenu(),
        _T( "Break Link To" ) );

    vs::CSequenceOp* pConnOp = dynamic_cast< vs::CSequenceOp* >( g_pEditor->m_pFocusObject );
    INT nConnType = g_pEditor->m_nFocusConnType;
    INT nConnIndex = g_pEditor->m_nFocusConnIndex;

    switch( nConnType )
    {
    case ic::EInput:
        {
            size_t i = 0;

            BOOST_FOREACH( vs::SequenceObjectMapValue& it, g_pEditor->m_pSequence->mapObjects )
            {
                vs::CSequenceOp* pSequenceOp = dynamic_cast< vs::CSequenceOp* >( it.second );
                if( !pSequenceOp )
                    continue;

                BOOST_FOREACH( vs::SSeqOpOutputLink& rOutputLink, pSequenceOp->vecOutputLinks )
                {
                    BOOST_FOREACH( vs::SSeqOpOutputInputLink& rOutInLink, rOutputLink.vecLinks )
                    {
                        if( rOutInLink.pLinkedOp != pConnOp ||
                            rOutInLink.nInputLinkIndex != nConnIndex )
                            continue;

                        CString strText;
                        strText.Format(
                            _T( "%s (%s)" ),
                            pSequenceOp->GetClassName().c_str(),
                            rOutputLink.strLinkDesc.c_str() );

                        m_cMenuBreakLinkTo.AppendMenu(
                            MF_STRING,
                            EBreakLinkTo + i,
                            strText.GetString() );

                        ++i;
                    }
                }
            }

            AppendMenu( MF_SEPARATOR, NULL, _T( "" ) );
            AppendMenu( MF_STRING, ESetActivateDelay, _T( "Set Activate Delay" ) );
            AppendMenu( MF_STRING, ESetActiveEvent, _T( "Active Event" ) );
        }
        break;

    case ic::EOutput:
        {
            size_t nSize = pConnOp->vecOutputLinks[ nConnIndex ].vecLinks.size();
            nSize = min( nSize, EBreakLinkToMax );

            for( size_t i=0; i<nSize; ++i )
            {
                vs::SSeqOpOutputInputLink& rLink = pConnOp->vecOutputLinks[ nConnIndex ].vecLinks[ i ];

                if( !rLink.pLinkedOp )
                    continue;

                CString strText;
                strText.Format(
                    _T( "%s (%s)" ),
                    rLink.pLinkedOp->GetClassName().c_str(),
                    rLink.pLinkedOp->vecInputLinks[ rLink.nInputLinkIndex ].strLinkDesc.c_str() );

                m_cMenuBreakLinkTo.AppendMenu(
                    MF_STRING,
                    EBreakLinkTo + i,
                    strText.GetString() );
            }

            AppendMenu( MF_SEPARATOR, NULL, _T( "" ) );
            AppendMenu( MF_STRING, ESetActivateDelay, _T( "Set Activate Delay" ) );
            AppendMenu( MF_STRING, ESetActiveEvent, _T( "Active Event" ) );
        }
        break;

    case ic::EVariable:
        {
            size_t nSize = pConnOp->vecVariableLinks[ nConnIndex ].vecLinks.size();
            nSize = min( nSize, EBreakLinkToMax );

            for( size_t i=0; i<nSize; ++i )
            {
                vs::CSequenceVariable* pLink = pConnOp->vecVariableLinks[ nConnIndex ].vecLinks[ i ];

                if( !pLink )
                    continue;

                CString strText;
                strText.Format(
                    _T( "%s (%s)" ),
                    pLink->GetClassName().c_str(),
                    pLink->GetValueText().c_str() );

                m_cMenuBreakLinkTo.AppendMenu(
                    MF_STRING,
                    EBreakLinkTo + i,
                    strText.GetString() );
            }
        }
        break;
    }
}

CSequenceMenuConnector::~CSequenceMenuConnector()
{
    m_cMenuBreakLinkTo.DestroyMenu();
    DestroyMenu();
}

CSequenceMenuConnector::SSelect CSequenceMenuConnector::Open( const ic::Int2& sPos, CWnd* pParent )
{
    SSelect sSelect;

    sSelect.nIndex = (size_t)TrackPopupMenu(
        TPM_NONOTIFY | TPM_RETURNCMD,
		sPos.x,
        sPos.y,
        pParent );

    return sSelect;
}