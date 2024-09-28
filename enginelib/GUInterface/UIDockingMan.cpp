#include "pch.h"

#include "UIMan.h"
#include "UIControl.h"
#include "UIDockingMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIDockingMan::SDOCKINGINFO::SDOCKINGINFO (CUIControl* _pCONTROL, const BOOL _ISPARENT)
: pCONTROL(_pCONTROL)
, pPARENT(NULL)
, ISPARENT(_ISPARENT)
{
    if ( pCONTROL )
    {
        rcRECT = pCONTROL->GetGlobalPos();
    }
}

const UIRECT CUIDockingMan::SDOCKINGINFO::GetGlobalPos ()
{
    if ( pCONTROL )
    {
        return pCONTROL->GetGlobalPos();
    }
    //CHILDLIST

    return UIRECT();
}

void CUIDockingMan::SDOCKINGINFO::SetGlobalPos ( D3DXVECTOR2& vPos )
{
    if ( pCONTROL )
    {
        pCONTROL->SetGlobalPos(vPos);
    }
    //CHILDLIST
}

// ----------------------------------------------------------------------------

const FLOAT CUIDockingMan::DEFAULT_ATTACH_DISTANCE = 10.0f;

CUIDockingMan::CUIDockingMan ()
: m_pDragControl    (NULL)
, m_fAttachDistance (DEFAULT_ATTACH_DISTANCE)
{
    //Blank
}

CUIDockingMan::~CUIDockingMan ()
{
    //Blank
}

void CUIDockingMan::RemoveAll()
{
    if ( !m_DockingMap.empty() )
    {
        m_DockingMap.clear();
    }

    m_pDragControl = NULL;
}

void CUIDockingMan::Update ( float fElapsedTime )
{
    if ( !m_pDragControl )
    {
        return;
    }

    D3DXVECTOR2 vMovePos(-1.0f, -1.0f);
    UIRECT      rcGlobalPos = m_pDragControl->GetGlobalPos();

    // 변경이 없다면 취소
    if ( rcGlobalPos == m_pDragControl->rcRECT )
    {
        return;
    }

    m_rcDragReal.sizeX = rcGlobalPos.sizeX;
    m_rcDragReal.sizeY = rcGlobalPos.sizeY;

    m_rcDragReal.SetPosition ( m_rcDragReal.left + (rcGlobalPos.left - m_pDragControl->rcRECT.left),
                               m_rcDragReal.top  + (rcGlobalPos.top  - m_pDragControl->rcRECT.top) );


    // Window 창에 맞추기
    if ( m_rcDragReal.left < m_fAttachDistance )
    {
        vMovePos.x = 0.0f; 
    }
    else if ( GetWindowWidth() < m_rcDragReal.right + m_fAttachDistance )
    {
        vMovePos.x = m_rcDragReal.left + GetWindowWidth() - m_rcDragReal.right; 
    }

    if ( m_rcDragReal.top < m_fAttachDistance )
    {
        vMovePos.y = 0.0f; 
    }
    else if ( GetWindowHeight() < m_rcDragReal.bottom + m_fAttachDistance )
    {
        vMovePos.y = m_rcDragReal.top + GetWindowHeight() - m_rcDragReal.bottom; 
    }

    // 다른 창들에 맞추기
    UIDOCKINGINFO_MAP_ITER iter = m_DockingMap.begin();

    for ( ; iter!=m_DockingMap.end(); ++iter )
    {
        SDOCKINGINFO& sInfo = (iter->second);

        if ( m_pDragControl == (&sInfo) )
        {
            continue;
        }

        if ( !sInfo.pCONTROL )
        {
            continue;
        }

        const UIRECT& rcDest = sInfo.pCONTROL->GetGlobalPos();

        AttachCheck ( m_rcDragReal, rcDest, vMovePos ); 
    }

    // 결과값 적용
    if ( vMovePos.x != -1 )
    {
        m_rcDragVirtual.left  = vMovePos.x;
        m_rcDragVirtual.right = vMovePos.x + rcGlobalPos.sizeX;
    }
    else
    {
        m_rcDragVirtual.left  = m_rcDragReal.left;
        m_rcDragVirtual.right = m_rcDragReal.right;
    }

    if ( vMovePos.y != -1 )
    {
        m_rcDragVirtual.top    = vMovePos.y;
        m_rcDragVirtual.bottom = vMovePos.y + rcGlobalPos.sizeY;
    }
    else
    {
        m_rcDragVirtual.top    = m_rcDragReal.top;
        m_rcDragVirtual.bottom = m_rcDragReal.bottom;
    }

    m_pDragControl->SetGlobalPos ( D3DXVECTOR2(m_rcDragVirtual.left, m_rcDragVirtual.top) );
    m_pDragControl->rcRECT = m_pDragControl->GetGlobalPos();
}

const BOOL CUIDockingMan::RegisterControl ( CUIControl* pControl, const BOOL bParent )
{
    if ( !pControl )
    {
        GASSERT( "CUIDockingMan::RegisterControl !Control");
        return FALSE;
    }

    UIGUID cID = pControl->GetWndID();

    if ( NO_ID == cID )
    {
        GASSERT( "CUIDockingMan::RegisterControl ID == NO_ID");
        return FALSE;
    }

    if ( m_DockingMap.find( cID ) != m_DockingMap.end() )
    {
        GASSERT( "CUIDockingMan::RegisterControl 같은 ID를 가진 Control이 존재합니다.");
        return FALSE;
    }

    SDOCKINGINFO sInfo( pControl, bParent );
    m_DockingMap.insert( std::make_pair(cID, sInfo) );

    return TRUE;
}

void CUIDockingMan::SetDragControl ( CUIControl* pControl )
{
    if ( !pControl )
    {
        m_pDragControl = NULL;
        return;
    }

    UIDOCKINGINFO_MAP_ITER iter = m_DockingMap.find( pControl->GetWndID() ); 

    if ( iter == m_DockingMap.end() )
    {
        m_pDragControl = NULL;
        return;
    }
    
    SDOCKINGINFO* pInfo = &(iter->second);

    // 현재 드래그중인 컨트롤이라면 
    if ( pInfo == m_pDragControl )
    {
        return;
    }

    m_pDragControl  = pInfo;
    m_rcDragReal    = pInfo->GetGlobalPos();
    m_rcDragVirtual = m_rcDragReal;
    pInfo->rcRECT   = m_rcDragReal;
}

const BOOL CUIDockingMan::AttachCheck ( const UIRECT& rcSrc, const UIRECT& rcDest, D3DXVECTOR2& vInOutMove )
{
    UIRECT rcExpendRect = rcDest;

    rcExpendRect.left   = rcDest.left   - m_fAttachDistance;
    rcExpendRect.right  = rcDest.right  + m_fAttachDistance;
    rcExpendRect.top    = rcDest.top    - m_fAttachDistance;
    rcExpendRect.bottom = rcDest.bottom + m_fAttachDistance;
    rcExpendRect.sizeX  = rcDest.sizeX  + m_fAttachDistance*2.0f;
    rcExpendRect.sizeY  = rcDest.sizeY  + m_fAttachDistance*2.0f;

    if ( !CollisionCheck ( rcSrc, rcExpendRect ) )
    {
        return FALSE;
    }

    D3DXVECTOR2 vMove(-1.0f, -1.0f);

    if ( fabs(rcDest.left - rcSrc.right) < m_fAttachDistance 
        && rcSrc.left <= rcDest.left )
    {
        vInOutMove.x = rcSrc.left + (rcDest.left - rcSrc.right);
    }
    else if ( fabs(rcSrc.left - rcDest.right) < m_fAttachDistance 
        && rcDest.right <= rcSrc.right )
    {
        vInOutMove.x = rcDest.right;
    }

    if ( fabs(rcDest.top - rcSrc.bottom) < m_fAttachDistance
        && rcSrc.top <= rcDest.top )
    {
        vInOutMove.y = rcSrc.top + (rcDest.top - rcSrc.bottom);
    }
    else if ( fabs(rcSrc.top - rcDest.bottom) < m_fAttachDistance 
        && rcDest.bottom <= rcSrc.bottom )
    {
        vInOutMove.y = rcDest.bottom;
    }

    return TRUE;
    //vInOutMove = vMove;
}

const BOOL CUIDockingMan::CollisionCheck  ( const UIRECT& rcSrc, const UIRECT& rcDest )
{
    if( rcSrc.right  < rcDest.left || rcSrc.left > rcDest.right ) return FALSE;
    if( rcSrc.bottom < rcDest.top  || rcSrc.top > rcDest.bottom ) return FALSE;

    return TRUE;
}

const FLOAT CUIDockingMan::GetWindowWidth ()
{
    return (FLOAT)HIWORD( CUIMan::GetResolution() );
}

const FLOAT CUIDockingMan::GetWindowHeight ()
{
    return (FLOAT)LOWORD( CUIMan::GetResolution() );
}
