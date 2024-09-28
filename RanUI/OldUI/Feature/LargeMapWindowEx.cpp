#include "StdAfx.h"
#include "LargeMapWindow.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//CUIControl* CLargeMapWindow::MAKE_MARK ( const int nPartyID )
//{
//	m_CONTROL_NEWID++;
//	if ( BASE_CONTROLID_END <= m_CONTROL_NEWID ) m_CONTROL_NEWID = BASE_CONTROLID;	
//
//	static CString strKeyword = "TEST_MARK";
//	CString strKeywordNum;
//	strKeywordNum.Format ( "%s%d", strKeyword, nPartyID );
//
//	CUIControl* pMark = new CUIControl(m_pRenderDevice);
//	pMark->CreateSub ( this, strKeywordNum.GetString(), UI_FLAG_DEFAULT, m_CONTROL_NEWID );
//	pMark->InitDeviceObjects ( m_pd3dDevice );
//	pMark->RestoreDeviceObjects ( m_pd3dDevice );
//	pMark->SetUseRender ( TRUE );
//	RegisterControl ( pMark );
//
//	return pMark;
//}

void CLargeMapWindow::UPDATE_PARTY ()
{
	UPDATE_PARTY_DEL();

	// CTF���ϋ��� �������� �ʴ´�.
	if ( IS_CTF_UPDATE() )
	{
		return;
	}

	GLPartySlotClient* pSelf = FindSelfClient ();

	if( pSelf )
	{
		UPDATE_PARTY_INS( pSelf ); // �߰�
	}
	//else // Note : ��Ƽ�� �ƴ϶�� �� �� �����ϴ� ��� ��Ʈ���� �����.
	//{
	//	PARTY_ONMAP_MAP_ITER iter;

	//	for( iter = m_mapPARTY.begin(); iter != m_mapPARTY.end(); )
	//	{
	//		PARTY_ONMAP_MAP_ITER cur = iter++;

	//		CUIControl * pMark = (*cur).second;
	//		if( pMark ) DeleteControl( pMark->GetWndID(), 0 );

	//		m_mapPARTY.erase( cur );
	//	}
	//}
}

void CLargeMapWindow::UPDATE_PARTY_DEL()
{
	//PARTY_ONMAP_MAP_ITER iter;

	//for( iter = m_mapPARTY.begin(); iter != m_mapPARTY.end(); )
	//{
	//	PARTY_ONMAP_MAP_ITER cur = iter++;

	//	const DWORD& dwGaeaID = (*cur).first;
	//	GLPARTY_CLIENT *pMember = m_pGaeaClient->GetPartyClient()->FindMember ( dwGaeaID );
	//	if ( pMember && IsSameMap ( pSelf, pMember ) ) continue;		

	//	CUIControl* pMark = (*cur).second;
	//	if( !pMark ) return ;

	//	DeleteControl( pMark->GetWndID(), 0 );
	//	m_mapPARTY.erase( cur );
	//}

	for( INT i=0; i<MAXPARTY; ++i )
	{
		m_pMark[i]->SetVisibleSingle( FALSE );
	}
}

void CLargeMapWindow::UPDATE_PARTY_INS(GLPartySlotClient* const pSelf )
{
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( pMyParty->isValid() == false )
		return;

	GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY); // ������;
	if( ( pSelf != pMaster ) && IsSameMap( pSelf, pMaster ) )
		UPDATE_CHAR_POS ( m_pMark[0], pMaster->m_vPosition );

	DWORD nMEMBER_NUM = pMyParty->GetNMember();
	if( nMEMBER_NUM > 1 )
	{
		nMEMBER_NUM -= 1; // Note : �����ʹ� ����
		for ( DWORD nIndex = 0; nIndex < nMEMBER_NUM; ++nIndex )
		{
			GLPartySlotClient* const pMember = pMyParty->GetMember( nIndex );

			if( !pMember )
				continue;

			if( !IsSameMap( pSelf, pMember ) )
				continue;

			if( pSelf == pMember )
				continue;

			//	Note : AddPartyMemberOnMAP�� ������ �Ķ���Ϳ� +1�� �����Ѱ���
			//         �ǵ��� ���Դϴ�. Master�� 0, ����� 1���ͽ����ؼ� 7���� �����մϴ�.
			UPDATE_CHAR_POS( m_pMark[nIndex+1], pMember->m_vPosition );
		}
	}
}

//void CLargeMapWindow::AddPartyMemberOnMAP ( const DWORD& dwGaeaID, const D3DXVECTOR3& vPos, const int nPartyID )
//{
//	//CUIControl* pMark(NULL);
//
//	//PARTY_ONMAP_MAP_ITER found = m_mapPARTY.find ( dwGaeaID );
//
//	//if ( found == m_mapPARTY.end() )
//	//{
//	//	pMark = MAKE_MARK( nPartyID );
//	//	if( !pMark ) return ;
//
//	//	m_mapPARTY.insert ( std::make_pair(dwGaeaID, pMark) );
//	//}
//	//else
//	//{
//	//	pMark = (*found).second;
//	//	if( !pMark ) return ;
//	//}
//
//	UPDATE_CHAR_POS ( m_pMark[nPartyID], vPos );
//}

BOOL CLargeMapWindow::IsSameMap(GLPartySlotClient* const pSelf, GLPartySlotClient* const pMember )
{
	if ( !pSelf || !pMember )
		return FALSE;
	
	return pSelf->m_sMapID == pMember->m_sMapID;
}

GLPartySlotClient* const CLargeMapWindow::FindSelfClient(void)
{
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();

	//	�����Ͱ� �ƴϸ�, �ڽĵ��� ã�ƺ���.
	GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY);	//	������

	if ( pMaster->isOneSelf(m_pGaeaClient) )
	{
		return pMaster;
	}
	else
	{ // Note : �ڱ�ã��
		DWORD nMEMBER_NUM = pMyParty->GetNMember();
		if( nMEMBER_NUM > 1 )
		{
			nMEMBER_NUM -= 1; // Note : �����ʹ� ����
			for ( DWORD _i = 0; _i < nMEMBER_NUM; ++_i )
			{
				GLPartySlotClient* const pMember = pMyParty->GetMember(_i);

				if (pMember && pMember->isOneSelf(m_pGaeaClient))
				{
					return pMember;				
				}
			}
		}
	}

	return NULL;
}

void CLargeMapWindow::UPDATE_GUILD()
{
	RESET_GUILD();
	
	if (!IS_GUILD_UPDATE())
        return;

	//	Ŭ�� Ȯ��
	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();

	// Ŭ�� ��Ʋ�������� ����
	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if (!pLand)
        return;

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    int nMyChannel = pGlobalStage->GetChannel();
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if (!pChar)
        return;
	
	DWORD dwCharID = pChar->GetCharID();

	CLUBMEMBERS& mapMembers = pMyClub->GetMembers();
	CLUBMEMBERS_ITER iter_mem = mapMembers.begin();
	CLUBMEMBERS_ITER iter_mem_end = mapMembers.end();

	GUILD_POS_VECTOR_ITER iter_guild = m_vecGuild.begin();
	GUILD_POS_VECTOR_ITER iter_guild_end = m_vecGuild.end();

	// ��� ��ȸ
	for ( ; iter_mem != iter_mem_end; ++iter_mem)
	{
		GLCLUBMEMBER& sClubMember = (*iter_mem).second;
		const bool bOnline = sClubMember.m_Online;
		const int nChannel = sClubMember.m_Channel;
		const DWORD dwID = sClubMember.m_ChaDbNum;

		// ������
		if (!bOnline || nMyChannel != nChannel)
            continue;		
		
		// �ڽ��ΰ�
		if (dwID == dwCharID)
            continue;		
		
		// ���� ��Ƽ�ΰ�
		if ( m_pGaeaClient->GetMyPartyClient()->isMember(sClubMember.m_ChaName.c_str()) )
            continue;
				
		// ���� ���̸�
		if (pLand->GetGaeaMapID() != sClubMember.m_CurrentMap.getGaeaMapID())
            continue;; 

		const D3DXVECTOR3& vPos = sClubMember.m_CurrentPos;

		if ( iter_guild != iter_guild_end )
		{
			CUIControl* pControl = (CUIControl*) (*iter_guild);
			UPDATE_CHAR_POS( pControl, vPos );

			iter_guild++;
		}
		else
		{
			// ���� ����
			CUIControl* pControl = new CUIControl(m_pEngineDevice);
			pControl->CreateSub( this, "MINIMAP_GUILD_MARK" );
			pControl->InitDeviceObjects ( m_pd3dDevice );
			pControl->RestoreDeviceObjects ( m_pd3dDevice );
			pControl->SetVisibleSingle( FALSE );
			RegisterControl( pControl );


			m_vecGuild.push_back ( pControl );
			UPDATE_CHAR_POS( pControl, vPos );
			
			iter_guild = m_vecGuild.end();
			iter_guild_end = m_vecGuild.end();
		}
	}
}

void CLargeMapWindow::RESET_GUILD()
{
	GUILD_POS_VECTOR_ITER iter_guild = m_vecGuild.begin();
	GUILD_POS_VECTOR_ITER iter_guild_end = m_vecGuild.end();
	
	for ( ; iter_guild != iter_guild_end; ++iter_guild )
	{
		CUIControl* pControl = (CUIControl*)(*iter_guild);
		pControl->SetVisibleSingle( FALSE );
	}	
}
bool CLargeMapWindow::IS_GUILD_UPDATE()
{
	//	Ŭ�� Ȯ��
	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	if (!pMyClub->IsValid())
        return false;

	if (pMyClub->GetBattleNum() <= 0)
        return false;

	// Ŭ�� ��Ʋ�������� ����
	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if ( pLand && !pLand->IsClubBattleZone() )	return false;
	
	return true;
}

void CLargeMapWindow::UPDATE_GUILD_INFO( bool bUpdate )
{
	if ( IS_GUILD_UPDATE() ) 
	{
		//m_pGaeaClient->ReqClubInfoUpdate( bUpdate );
	}
}

void CLargeMapWindow::UPDATE_CTF()
{
    RESET_CTF();

    if ( !IS_CTF_UPDATE() )
    {
        return;
    }

    using namespace PVP;
    GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();

    // ���� ���� ��ȸ
    {
        GLCaptureTheFieldClient::VEC_ALLIANCEINFO& vecAllianceInfo = pCTFClient->GetAllianceInfo();
        GLCaptureTheFieldClient::VEC_ALLIANCEINFO_ITER iterInfo = vecAllianceInfo.begin();
        CTF_POS_VECTOR_ITER iterVector = m_vecCTF.begin();
        
        for ( ; iterInfo != vecAllianceInfo.end(); ++iterInfo )
        {
            SALLIANCEINFO& sAllianceInfo = (*iterInfo);

            if ( sAllianceInfo.IsNull() )
            {
                break;
            }

            if ( iterVector != m_vecCTF.end() )
            {
                CUIControl* pControl = (CUIControl*) (*iterVector);
                UPDATE_CHAR_POS( pControl, sAllianceInfo.vPos );

                iterVector++;
            }
            else
            {
                // ���� ����
                CUIControl* pControl = new CUIControl(m_pEngineDevice);
                pControl->CreateSub( this, "MINIMAP_CTFALLIANCE_MARK" );
                pControl->InitDeviceObjects ( m_pd3dDevice );
                pControl->RestoreDeviceObjects ( m_pd3dDevice );
                pControl->SetVisibleSingle( FALSE );
                RegisterControl( pControl );

                m_vecCTF.push_back ( pControl );
                UPDATE_CHAR_POS( pControl, sAllianceInfo.vPos );

                iterVector = m_vecCTF.end();
            }
        }
    }

    // ������ ���� ��ȸ
    {
        GLCaptureTheFieldClient::VEC_AUTHENTICATORINFO& vecAuthInfo = pCTFClient->GetCurrentGlobalState().vecAUTHENTICATOR;
        GLCaptureTheFieldClient::VEC_AUTHENTICATORINFO_ITER iterAuth = vecAuthInfo.begin();
        CTF_AUTHENTICATOR_VECTOR_ITER iterVector = m_vecCTFAuthenticator.begin();

        for ( ; iterAuth != vecAuthInfo.end(); ++iterAuth )
        {
            SAUTHENTICATORINFO& sAuthInfo = (*iterAuth);

            GASSERT( sAuthInfo.emAuthSchool != SCHOOL_NUM );

            if ( sAuthInfo.IsNull() )
            {
                break;
            }

            if ( iterVector != m_vecCTFAuthenticator.end() )
            {
                std::string strControl; 

                if( sAuthInfo.emAuthSchool == SCHOOL_NONE )
                {
                    strControl = "LARGEMAP_CTFMARK_SCHOOL_NONE";
                }
                else
                {
                    strControl = sc::string::format("LARGEMAP_CTFMARK_SCHOOL%1%", sAuthInfo.emAuthSchool);
                }

                CUIControl* pControl = (CUIControl*) (*iterVector);

                if ( pControl->GetControlNameEx() != strControl )
                {
                    pControl->ResetControl( GetGlobalPos(), strControl.c_str() );
                }

                UPDATE_CHAR_POS( pControl, sAuthInfo.vPos );
                iterVector++;
            }
            else
            {
                std::string strControl; 

                if( sAuthInfo.emAuthSchool == SCHOOL_NONE )
                {
                    strControl = "LARGEMAP_CTFMARK_SCHOOL_NONE";
                }
                else
                {
                    strControl = sc::string::format("LARGEMAP_CTFMARK_SCHOOL%1%", sAuthInfo.emAuthSchool);
                }

                // ���� ����
                CUIControl* pControl = new CUIControl(m_pEngineDevice);
                pControl->CreateSub( this, strControl.c_str() );
                pControl->InitDeviceObjects ( m_pd3dDevice );
                pControl->RestoreDeviceObjects ( m_pd3dDevice );
                pControl->SetVisibleSingle( FALSE );
                RegisterControl( pControl );

                m_vecCTFAuthenticator.push_back ( pControl );
                UPDATE_CHAR_POS( pControl, sAuthInfo.vPos );

                iterVector = m_vecCTFAuthenticator.end();
            }
        }
    }
}

void CLargeMapWindow::RESET_CTF()
{
    CTF_POS_VECTOR_ITER iterAlliance = m_vecCTF.begin();

    for ( ; iterAlliance != m_vecCTF.end(); ++iterAlliance )
    {
        CUIControl* pControl = (CUIControl*)(*iterAlliance);
        pControl->SetVisibleSingle( FALSE );
    }	

    CTF_AUTHENTICATOR_VECTOR_ITER iterAuth = m_vecCTFAuthenticator.begin();

    for ( ; iterAuth != m_vecCTFAuthenticator.end(); ++iterAuth )
    {
        CUIControl* pControl = (CUIControl*)(*iterAuth);
        pControl->SetVisibleSingle( FALSE );
    }	
}

bool CLargeMapWindow::IS_CTF_UPDATE()
{
    // CTF ���� ���϶��� ����
    if ( !m_pGaeaClient->IsCTFPlayer() )
    {
        return false;
    }

    return true;
}