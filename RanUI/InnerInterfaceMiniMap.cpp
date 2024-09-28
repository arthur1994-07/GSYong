#include "stdafx.h"
#include "./OldUI/Interface/MiniMap.h"
#include "./OldUI/Tutorial/MinimapTarget.h"
#include "./InnerInterface.h"
#include "./OldUI/Feature/RnMapWindowInterface.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::SetVisibleMiniMapClubGuidBattleTime( const BOOL bVisible )
{
    if ( m_pMiniMap == NULL)
    {
        return;
    }
    if ( bVisible )
    {
        m_pMiniMap->StartClubGuidBattleTime();
    }
    else
    {
        m_pMiniMap->EndClubBattleTime();
    }  
}

void CInnerInterface::SetVisibleMiniMapClubDeathMatchTime( const BOOL bVisible )
{
    if ( m_pMiniMap == NULL)
    {
        return;
    }
    if ( bVisible )
    {
        m_pMiniMap->StartClubDeathMatchTime();
    }
    else
    {
        m_pMiniMap->EndClubBattleTime();
    }  
}

bool CInnerInterface::MinimapTargetIsEnable() const
{
    return m_pMinimapTarget->IsEnable();
}

void CInnerInterface::SetClubGuidBattleName( const std::vector<std::string> &vecName )
{
    return m_pMiniMap->SetClubGuidBattleName( vecName );
}

void CInnerInterface::UpdateClubGuidBattleTime( const std::vector<float> &vecTime )
{
    if ( m_pMiniMap )
    {
        m_pMiniMap->UpdateClubGuidBattleTime( vecTime );
    }
}

void CInnerInterface::UpdateClubDeathMatchTime( const float fTime )
{
    if ( m_pMiniMap )
    {
        m_pMiniMap->UpdateClubDeathMatchTime( fTime );
    }
}

//////////////////////////////////////////////////////////////////////////
void CInnerInterface::MinimapTargetSetEnable( bool State )
{
    m_pMinimapTarget->SetEnable( State );
}

const UIRECT& CInnerInterface::MinimapTargetGetGlobalPos() const
{ 
    return m_pMinimapTarget->GetGlobalPos();
}

void CInnerInterface::MinimapTargetSetGlobalPos( const UIRECT& Pos )
{     
    m_pMinimapTarget->SetGlobalPos( Pos );
}

void CInnerInterface::MinimapTargetSetGlobalPos( const D3DXVECTOR2& vPos )
{
    m_pMinimapTarget->SetGlobalPos( vPos );
}

void CInnerInterface::MinimapTargetSetLocalPos( const UIRECT& rcLocalPosNew )
{
    m_pMinimapTarget->SetLocalPos( rcLocalPosNew );
}

void CInnerInterface::MinimapTargetSetLocalPos( const D3DXVECTOR2& vPos )
{
    m_pMinimapTarget->SetLocalPos( vPos );
}

void CInnerInterface::MsgMapSearchNameList(const std::vector<std::pair<int, std::string> >& nameList)
{
	m_pMapWindowInterface->MsgMapSearchNameList(nameList);
}

void CInnerInterface::MsgMapSearchNameDetailInfo(int level,const std::string& charName,const std::string& title,int school,int emClass,
												 bool isParty,const std::string& clubName,DWORD dwGaeaID)
{
	m_pMapWindowInterface->MsgMapSearchNameDetailInfo(level,charName,title,school,emClass,isParty,clubName,dwGaeaID);
}

void CInnerInterface::MsgMapSearchNamePosition(int level,const std::string& charName,DWORD mapID,const D3DXVECTOR3& pos)
{
	m_pMapWindowInterface->MsgMapSearchNamePosition(level,charName,mapID,pos);
}


void CInnerInterface::MsgMapPartyInfoListBoard(bool isExistMyBoard,DWORD mapID,
									  const std::vector<std::string>& NameVec,const std::vector<DWORD>& MaxPlayerVec,
									  const std::vector<DWORD>& CurrentPlayerVec,const std::vector<std::string>& TitleVec)
{
	m_pMapWindowInterface->MsgMapPartyInfoListBoard(isExistMyBoard,mapID,NameVec,MaxPlayerVec,CurrentPlayerVec,TitleVec);
}

void CInnerInterface::MsgMapPartyInfoDetail(DWORD mapID,const std::vector<GLPARTY_LINK_MEM>& memberVec)
{
	m_pMapWindowInterface->MsgMapPartyInfoDetail(mapID,memberVec);
}


