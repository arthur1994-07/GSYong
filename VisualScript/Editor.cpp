#include "stdafx.h"

#include "../InternalCommonLib/dxstdafx.h"

#include <boost/lexical_cast.hpp>
#include <../../../RanThirdParty/WindowsSDK_v6.1/Include/WinDef.h>

#include "../VisualScriptLib/Logic/SequenceEvent.h"
#include "../VisualScriptLib/Logic/SequenceVariable.h"
#include "../VisualScriptLib/Logic/SequenceAction.h"
#include "../VisualScriptLib/Logic/SequenceFrame.h"
#include "../VisualScriptLib/Logic/SequenceMgr.h"

#include "../EngineLib/DxBase/RenderInterface.h"
#include "../EngineLib/DxBase/RenderDevice.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/TextTexture/TextUtil.h"
#include "../EngineLib/DxResponseMan.h"

#include "../RanLogicServer/DxServerInstance.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../enginelib/GUInterface/GameTextControl.h"

#include "./VisualScript.h"
#include "./Frame.h"
#include "./Editor.h"

#include "../SigmaCore/DebugInclude.h"

CEditor* g_pEditor = NULL;

CEditor::CEditor()
    : m_pSequenceRoot( NULL )
    , m_pSequence( NULL )
    , m_pFocusObject( NULL )
    , m_nFocusConnType( INT_MIN )
    , m_nFocusConnIndex( INT_MIN )
    , m_nSequencePastCount( 0 )
{
    CString strPath = theApp.m_strPath;
    strPath.Append( _T( "\\" ) );

    //GMTOOL::Create( strPath.GetString() );

    NewSequenceRoot();
    m_pSequence = m_pSequenceRoot;
    
    CVisualScriptApp* pApp = (CVisualScriptApp*) AfxGetApp();
    SUBPATH* pPath = pApp->GetSubPath();
    m_pEngineDevice = new DxResponseMan(SP_OFFICE_TEST, pPath);
}

CEditor::~CEditor()
{
    SAFE_DELETE( m_pSequenceRoot );

    //GMTOOL::CleanUp();

    delete m_pEngineDevice;
    m_pEngineDevice = NULL;

    ic::CRttiMgr::Destroy();
}

VOID CEditor::NewSequenceRoot()
{
    CRenderDevice::s_pHitProxyViewport = NULL;

    SetFocusInfo( NULL, INT_MIN, INT_MIN );

    EmptySelectedSequence();

    SAFE_DELETE( m_pSequenceRoot );
    m_pSequenceRoot = new vs::CSequence;
    m_pSequence = NULL;
}

VOID CEditor::SaveSequence( const TSTRING& strPath )
{
    vs::CSequenceMgr::Save( m_pSequenceRoot, strPath );
}

BOOL CEditor::LoadSequence( const TSTRING& strPath )
{
    if( !vs::CSequenceMgr::Load( m_pSequenceRoot, strPath ) )
        return FALSE;

    CRenderDevice::s_pHitProxyViewport = NULL;

    if( m_pSequenceRoot )
        m_pSequenceRoot->CheckForErrors();
    else
        NewSequenceRoot();

    m_pSequence = NULL;

    return TRUE;
}

BOOL CEditor::CopyObjects( ic::Int2& sOrigin )
{
    if( m_vecSequenceSelected.empty() )
        return FALSE;

    vs::CSequenceMgr::OStream( m_strSequenceCopy, m_vecSequenceSelected );

    m_sSequenceCopyOrigin = sOrigin;
    m_nSequencePastCount = 0;

    return TRUE;
}

BOOL CEditor::PasteObjects( ic::Int2& sOrigin, ic::Int2* pAtPos )
{
    if( m_strSequenceCopy.empty() )
        return FALSE;

    vs::SequenceObjectVec vecSequencePasted;
    vs::CSequenceMgr::IStream( m_strSequenceCopy, vecSequencePasted );

    if( vecSequencePasted.empty() )
        return FALSE;

    EmptySelectedSequence();
    m_vecSequenceSelected = vecSequencePasted;

    ic::Int2 sMoveDist;

    if( pAtPos )
    {
        ic::Int4 sBox( SHRT_MAX, SHRT_MAX, -SHRT_MAX, -SHRT_MAX );
        CalcBoundingBox( m_vecSequenceSelected, sBox );

        sMoveDist.x = pAtPos->x - sBox.x;
        sMoveDist.y = pAtPos->y - sBox.y;
    }
    else
    {
        ++m_nSequencePastCount;

        const INT PasteOffset = 30;
        INT nOffset = m_nSequencePastCount * PasteOffset;
        sMoveDist = m_sSequenceCopyOrigin - sOrigin + ic::Int2( nOffset, nOffset );
    }

    MoveSelectedSequence( sMoveDist );

    m_pSequence->PasteObject( m_vecSequenceSelected );
    m_pSequence->PostLoad();

    return TRUE;
}

vs::CSequenceObject* CEditor::NewSequenceObject(
    size_t nIndex,
    const ic::Int2& sPos,
    const TSTRING& strName )
{
    vs::CSequenceObject* pObject = m_pSequence->NewObject( nIndex, sPos );
    if( !pObject )
        return NULL;

    vs::CSequenceFrame* pSeqFrame = dynamic_cast< vs::CSequenceFrame* >( pObject );
    if( pSeqFrame )
    {
        if( !strName.empty() )
            pObject->strComment = strName;

        if( !m_vecSequenceSelected.empty() )
        {
            const INT FitCommentToSelectedBorder = 15;

            ic::Int4 sBox( SHRT_MAX, SHRT_MAX, -SHRT_MAX, -SHRT_MAX );
            CalcBoundingBox( m_vecSequenceSelected, sBox );

            pSeqFrame->sPos.x = sBox.x - FitCommentToSelectedBorder;
		    pSeqFrame->sPos.y = sBox.y - FitCommentToSelectedBorder;
		    pSeqFrame->sSize.x = ( sBox.z - sBox.x ) + ( FitCommentToSelectedBorder * 2 );
		    pSeqFrame->sSize.y = ( sBox.w - sBox.y ) + ( FitCommentToSelectedBorder * 2 );
            pSeqFrame->bDrawBox = TRUE;
        }
    }
    else
    {
        if( !strName.empty() )
            pObject->strName = strName;
    }

    vs::CSequence* pSequence = dynamic_cast< vs::CSequence* >( pObject );
    if( pSequence )
        m_pSequence->MekeUniqueName( pSequence );

    m_pSequence->InsertObject( pObject );

    pObject->OnCreated();

    EmptySelectedSequence();

    InsertSelectedSequence( pObject );

    return pObject;
}

BOOL CEditor::SetFocusInfo( vs::CSequenceObject* pObject, INT nConnType, INT nConnIndex )
{
    BOOL bIsDiff = FALSE;
    if( m_pFocusObject != pObject ||
        m_nFocusConnType != nConnType ||
        m_nFocusConnIndex != nConnIndex )
    {
        bIsDiff = TRUE;
    }

    m_pFocusObject = pObject;
    m_nFocusConnType = nConnType;
    m_nFocusConnIndex = nConnIndex;

    return bIsDiff;
}

VOID CEditor::BreakAllLinksOfFocusObject()
{
    m_pSequence->BreakLinkAll( m_pFocusObject );
}

VOID CEditor::BreakLinksOfFocusConnector( INT nLinkIndex )
{
    m_pSequence->BreakLink(
        m_pFocusObject,
        m_nFocusConnType,
        m_nFocusConnIndex,
        nLinkIndex );
}

VOID CEditor::ActionObject( vs::CSequenceObject* pObject, INT nConnType, INT nConnIndex )
{
    vs::CSequenceOp* pOp = dynamic_cast< vs::CSequenceOp* >( pObject );
    if( !pOp )
        return;

    switch( nConnType )
    {
    case ic::EInput:
        {
            pOp->InsertInputImpulse( nConnIndex );
        }
        break;

    case ic::EOutput:
        {
            pOp->InsertOutputImpulse( nConnIndex );
        }
        break;
    }
}

BOOL CEditor::ActionNamedVar( ic::CBaseObject* pObject )
{
    vs::CSeqVar_Named* pNamedVar = dynamic_cast< vs::CSeqVar_Named* >( pObject );
    if( !pNamedVar )
        return FALSE;

    std::vector< vs::CSequenceVariable* > vecVars;
    m_pSequence->FindNamedVariable( pNamedVar->strFindVarName, FALSE, vecVars );

    if( vecVars.empty() )
        return FALSE;

    SetFocusInfo( NULL, INT_MIN, INT_MIN );
    EmptySelectedSequence();

    size_t nSize = vecVars.size();
    for( size_t i=0; i<nSize; ++i )
        InsertSelectedSequence( vecVars[ i ] );

    return TRUE;
}

VOID CEditor::EmptySelectedSequence()
{
    m_vecSequenceSelected.clear();
}

VOID CEditor::DeleteSelectedSequence()
{
    CRenderDevice::s_pHitProxyViewport = NULL;

    BOOST_FOREACH( vs::CSequenceObject* pObject, m_vecSequenceSelected )
        m_pSequence->DeleteObject( pObject );

    SetFocusInfo( NULL, INT_MIN, INT_MIN );

    EmptySelectedSequence();
}

VOID CEditor::InsertSelectedSequence( vs::CSequenceObject* pObject )
{
    if( !pObject )
        return;

    if( IsSelectedSequence( pObject ) )
        return;

    m_vecSequenceSelected.push_back( pObject );
}

VOID CEditor::InsertSelectedSequence( ic::SHitProxy* pHitProxy )
{
    ic::SLinkedObjProxy* pProxy = dynamic_cast< ic::SLinkedObjProxy* >( pHitProxy );
    if( pProxy )
        InsertSelectedSequence( dynamic_cast< vs::CSequenceObject* >( pProxy->pObject ) );
}

VOID CEditor::EraseSelectedSequence( vs::CSequenceObject* pObject )
{
    vs::SequenceObjectVecIter it = std::find(
        m_vecSequenceSelected.begin(),
        m_vecSequenceSelected.end(),
        pObject );

    if( it != m_vecSequenceSelected.end() )
        m_vecSequenceSelected.erase( it );
}

VOID CEditor::MoveSelectedSequence( ic::Int2& sDist )
{
    BOOST_FOREACH( vs::CSequenceObject* pObject, m_vecSequenceSelected )
    {
        pObject->sPos.x += sDist.x;
        pObject->sPos.y += sDist.y;
    }
}

BOOL CEditor::IsSelectedSequence( vs::CSequenceObject* pObject )
{
    vs::SequenceObjectVecIter it = std::find(
        m_vecSequenceSelected.begin(),
        m_vecSequenceSelected.end(),
        pObject );

    return ( it != m_vecSequenceSelected.end() ) ? TRUE : FALSE;
}

VOID CEditor::MakeConnectionToConnector(
    vs::CSequenceOp* pSrcObject,
    INT nSrcConnType,
    INT nSrcConnIndex,
    ic::CBaseObject* pDestObject,
    INT nDestConnType,
    INT nDestConnIndex )
{
    if( !pDestObject->IsChildOf( vs::CSequenceOp::Rtti() ) )
        return;

	if( nSrcConnType == ic::EInput && nDestConnType == ic::EOutput )
    {
		MakeLogicConnection(
            dynamic_cast< vs::CSequenceOp* >( pDestObject ),
            nDestConnIndex,
            pSrcObject,
            nSrcConnIndex );
    }
	else if( nSrcConnType == ic::EOutput && nDestConnType == ic::EInput )
    {
		MakeLogicConnection(
            pSrcObject,
            nSrcConnIndex,
            dynamic_cast< vs::CSequenceOp* >( pDestObject ),
            nDestConnIndex );
    }
}

VOID CEditor::MakeLogicConnection(
    vs::CSequenceOp* pSrcObject,
    INT nSrcConnIndex,
    vs::CSequenceOp* pDestObject,
    INT nDestConnIndex )
{
    if( pSrcObject == pDestObject )
        return;

	vs::SSeqOpOutputLink &rOutputLink = pSrcObject->vecOutputLinks[ nSrcConnIndex ];

	size_t nNewIndex = INT_MAX;
	BOOL bFoundEntry = FALSE;

    for( size_t i=0; i<rOutputLink.vecLinks.size() && !bFoundEntry; ++i )
	{
		if( !rOutputLink.vecLinks[ i ].pLinkedOp )
		{
			nNewIndex = i;
		}
		else if( rOutputLink.vecLinks[ i ].pLinkedOp == pDestObject &&
			rOutputLink.vecLinks[ i ].nInputLinkIndex == nDestConnIndex )
		{
			bFoundEntry = TRUE;
		}
	}

    if( bFoundEntry )
        return;

	if( nNewIndex == INT_MAX )
    {
		rOutputLink.vecLinks.push_back( vs::SSeqOpOutputInputLink() );
        nNewIndex = rOutputLink.vecLinks.size() - 1;
    }

    rOutputLink.vecLinks[ nNewIndex ].pLinkedOp = pDestObject;
	rOutputLink.vecLinks[ nNewIndex ].nInputLinkIndex = nDestConnIndex;
}

BOOL CanConnectVarToLink( vs::SSeqVarLink* pVarLink, vs::CSequenceVariable* pVariable )
{
    if( !pVarLink || !pVariable )
        return FALSE;

	if( !pVarLink->pExpected )
		return FALSE;

	if( pVarLink->nMaxVars != -1 && pVarLink->nMaxVars <= (INT)pVarLink->vecLinks.size() )
		return FALSE;

	if( pVariable->IsChildOf( pVarLink->pExpected ) )
		return TRUE;

    if( pVariable->IsA( vs::CSeqVar_Named::Rtti() ) )
    {
        vs::CSeqVar_Named* pNamedVar = dynamic_cast< vs::CSeqVar_Named* >( pVariable );

        if( !pNamedVar->pExpected )
            return TRUE;

        if( CanConnectVarToLink( pVarLink, dynamic_cast< vs::CSequenceVariable* >( pNamedVar->pExpected ) ) )
            return TRUE;
    }

	return FALSE;
}

VOID CEditor::MakeConnectionToObject(
    vs::CSequenceOp* pSrcObject,
    INT nSrcConnType,
    INT nSrcConnIndex,
    ic::CBaseObject* pDestObject )
{
    if( pSrcObject == pDestObject )
        return;

    if( nSrcConnType != ic::EVariable )
        return;

    vs::CSequenceVariable* pVariable = dynamic_cast< vs::CSequenceVariable* >( pDestObject );
    if( !pVariable )
        return;

    vs::SSeqVarLink& rVarLink = pSrcObject->vecVariableLinks[ nSrcConnIndex ];

    if( !CanConnectVarToLink( &rVarLink, pVariable ) )
        return;

    vs::SeqVariableVecIter fi = std::find(
        rVarLink.vecLinks.begin(),
        rVarLink.vecLinks.end(),
        pVariable );

    if( fi == rVarLink.vecLinks.end() )
    {
        rVarLink.vecLinks.push_back( pVariable );
        pSrcObject->OnLinkVariable( pVariable, nSrcConnIndex );
        pVariable->OnLinkFrom( pSrcObject, nSrcConnIndex );
    }
}

VOID CEditor::CalcBoundingBox( const vs::CSequenceObject* pObject, ic::Int4& sBox )
{
    if( !pObject )
        return;

    const ic::Int4 sObjectBox = pObject->GetBoundingBox();

  /*  sBox.x = min( sBox.x, sObjectBox.x );
    sBox.y = min( sBox.y, sObjectBox.y );

    sBox.z = max( sBox.z, sObjectBox.z );
    sBox.w = max( sBox.w, sObjectBox.w );*/
}

VOID CEditor::CalcBoundingBox( const vs::SequenceObjectMap& mapObjects, ic::Int4& sBox )
{
    BOOST_FOREACH( const vs::SequenceObjectMapValue& it, mapObjects )
        CalcBoundingBox( it.second, sBox );
}

VOID CEditor::CalcBoundingBox( const vs::SequenceObjectVec& vecObjects, ic::Int4& sBox )
{
    BOOST_FOREACH( const vs::CSequenceObject* pObject, vecObjects )
        CalcBoundingBox( pObject, sBox );
}

VOID CEditor::UpdateSequence( FLOAT fElapsedTime )
{
    m_pSequence->UpdateOp( fElapsedTime );
}

VOID CEditor::DrawSequence( FLOAT fElapsedTime )
{
    m_pSequence->DrawSequence(
        g_pRI,
        m_vecSequenceSelected,
        m_pFocusObject,
        m_nFocusConnType,
        m_nFocusConnIndex );
}

VOID CEditor::RenderTarget( FLOAT fElapsedTime )
{
}

HRESULT CEditor::OnDeviceCreated(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    CTextUtil::m_bUsage = TRUE;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		m_pEngineDevice->OneTimeSceneInit(
			theApp.m_strPath.GetString(),
			theApp.GetFrame(),
			CGameTextMan::GetInstance().GetCommentText("FONTSYS",1).GetString(),
			false,
			language::KOREAN,
			g_pRI->DefaultFontName().c_str() );
	}
	else
	{
		m_pEngineDevice->OneTimeSceneInit(
			theApp.m_strPath.GetString(),
			theApp.GetFrame(),
			COMMENT::FONTSYS[ 1 ].c_str(),
			false,
			language::KOREAN,
			g_pRI->DefaultFontName().c_str() );
	}

    DxFontMan::GetInstance().InitDeviceObjects( pd3dDevice );

    m_pEngineDevice->InitDeviceObjects( pd3dDevice, FALSE );

    return S_OK;
}

VOID CEditor::OnDeviceDestroyed()
{
    m_pEngineDevice->DeleteDeviceObjects();

    m_pEngineDevice->FinalCleanup();
}

HRESULT CEditor::OnDeviceReset(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    m_pEngineDevice->RestoreDeviceObjects();

    return S_OK;
}

VOID CEditor::OnDeviceLost()
{
    m_pEngineDevice->InvalidateDeviceObjects();
}

VOID CEditor::OnFrameMove(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    m_pEngineDevice->FrameMove( FLOAT( fTime ), fElapsedTime, FALSE );
}

VOID CEditor::OnFrameRender(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    CTextUtil::Get()->FrameMove();
}