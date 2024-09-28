#include "../pch.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Land/GLLandMark.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/RANPARAM.h"

#include "../Char/GLCharacter.h"
#include "../Char/GLCharClient.h"
#include "../Club/GLClubClient.h"
#include "../Party/GLPartyClient.h"
#include "../SNS/GLSNSClient.h"
#include "../GLGaeaClient.h"
#include "../Land/GLLandManClient.h"
#include "./GLCrowClient.h"
#include "./GLCrowRenList.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool CROWREN::DIFFERENT ( const CROWREN &sINFO )
{
	//m_emCROW;	//	key �̹Ƿ� �ٸ� ��찡 ����.
	//m_dwID;

	if ( m_wSCHOOL != sINFO.m_wSCHOOL )
		return true;
	if ( m_dwCOLOR != sINFO.m_dwCOLOR )
		return true;
	if ( m_emPARTY != sINFO.m_emPARTY )
		return true;
	if ( m_dwTYPE != sINFO.m_dwTYPE )
		return true;
	if ( m_bPMARKET != sINFO.m_bPMARKET )
		return true;
    if ( m_bSIMPLE != sINFO.m_bSIMPLE )
		return true;
    if ( m_dwSNS != sINFO.m_dwSNS )
		return true;

    if ( m_bHP != sINFO.m_bHP )
		return true;
   
	if ( strcmp(m_szNAME,sINFO.m_szNAME) )
		return true;
	if ( strcmp(m_szPMARKET,sINFO.m_szPMARKET) )
		return true;
	if ( strcmp(m_szNICK,sINFO.m_szNICK) )
		return true;
    if ( strcmp(m_szTitle,sINFO.m_szTitle) )
		return true;
    if ( m_nTitleClass != sINFO.m_nTitleClass )
		return true;

	if ( m_dwCLUB!=sINFO.m_dwCLUB )
		return true;
	if ( m_dwCLUB_MARK_VER!=sINFO.m_dwCLUB_MARK_VER )
		return true;

	if ( m_dwCountryID != sINFO.m_dwCountryID )
		return true;


	//m_vPOS;		//	�׻� ���ϴ� �ڷ�.
    //m_fHPPercent 
	//m_dwCOUNT;

	return false;
}

//void CROWREN::INIT ( GLLANDMARK *pLandMark )
//{
//	//m_emCROW = CROW_LANDMARK;
//	m_emCROW = CROW_NULL;
//	m_dwID = pLandMark->m_dwID;
//
//	StringCchCopy ( m_szNAME, CHAR_SZNAME, pLandMark->m_strMARK.c_str() );
//}
//
//void CROWREN::SETTYPEFLAG ( GLLANDMARK *pLandMark )
//{
//	m_dwCOLOR = pLandMark->m_dwColor;	//NS_UITEXTCOLOR::ORANGE;
//
//	// Note : �̸�ǥ�� ��������
//	m_vPOS		= pLandMark->m_pAffineParts->vTrans;
//	m_vPOS.y	= pLandMark->m_vMax.y;
//}

void CROWREN::UPDATE ( DWORD _COUNT, D3DXVECTOR3 _vPOS )
{
	m_vPOS = _vPOS;
	m_dwCOUNT = _COUNT;
}

void CROWREN::SetData( GLGaeaClient* const pGaeaClient, ClientActor* const pActor )
{
    // ���� ���� CrowRender DataSetting �Լ�;
    // Faction System�� ���� �����͸� �����ϵ��� ����;
    // ������ CDM,CTF,������ ���� ���� �����Ѵٴ� �������ǾƷ� �ۼ��Ǿ��ִ�. ( faction���� ��ü );
    // ���� faction ������ ���� �ʴ´ٸ� ���� ��Ĵ�� ��µǵ��� �س���;
    
    // ��ð� ���� ������ ���ؼ��� ������ �˻�����. ( ����� faction���� ��ü�� �Ұ��ؼ� );

    if (pActor == NULL)
        return;

    m_emCROW = pActor->GetCrow();
    m_dwID = pActor->GetGaeaID();
    m_dwTYPE = TYPE_NORMAL;

    StringCchCopy ( m_szNAME, CHAR_SZNAME, pActor->GetName() );

    GLLandManClient* const pLanMan = pGaeaClient->GetActiveMap();
    if ( pLanMan == NULL )
        return;

    const EMCROW _emCrow = pActor->GetCrow();
    GLCharacter* const pMyChar = pGaeaClient->GetCharacter();
    switch ( _emCrow )
    {
    case CROW_PC:
        {
            m_wSCHOOL = pActor->GetCharSchool();

            if ( pMyChar == pActor )
            {
				if ( pMyChar )
					m_dwCountryID = pMyChar->GetCountryInfo().wCountryID;

				// Note : �̸�ǥ�� ��������
				m_vPOS = pMyChar->GetPosBodyHeight();

                Faction::ManagerClient* pFactionMan = pLanMan->GetFactionManager();
				if ( (pFactionMan == NULL) || (pFactionMan->getMyFaction() == Faction::FactionID() ) )
                {
                    SetDataNonFaction(pGaeaClient, pMyChar);
                    return;
                }

                m_emPARTY = OTHER_PLAYER;
                if (pMyChar->IsPartyMaster())
                    m_emPARTY = PARTY_MASTER;
                else if (pMyChar->IsPartyMem())
                    m_emPARTY = PARTY_MEMBER;

                m_bPMARKET = pMyChar->m_sPMarket.IsOpen();
                StringCchCopy(m_szPMARKET, CHAT_MSG_SIZE+1, pMyChar->m_sPMarket.GetTitle().c_str());
                StringCchCopy(m_szNICK, CHAR_SZNAME, pMyChar->GetNickName());

                TITLEINFO sTitle;
                if (pMyChar->m_TitleManager.GetCurrentTitle(sTitle))
                {
                    m_nTitleClass = sTitle.nClass;

                    if (!sTitle.strTitle.empty())
                        StringCchCopy(m_szTitle, EM_TITLE_MAXLENGTH, ID2GAMEINTEXT(sTitle.strTitle.c_str()));
                    else
                        ZeroMemory(m_szTitle, EM_TITLE_MAXLENGTH);
                }

                m_dwCLUB = pGaeaClient->GetMyClub()->DbNum();
                m_dwCLUB_MARK_VER = pGaeaClient->GetMyClub()->m_dwMarkVER;

                //	Note : �̸� ��.
                if ( m_emPARTY == PARTY_MASTER || m_emPARTY == PARTY_MEMBER )  // ��Ƽ�� ���� ������ �����;
                    m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
                else
				{
					if( pMyChar->m_dwUserLvl < USER_USER_GM )
						m_dwCOLOR = pMyChar->GET_PK_COLOR();
					else
						m_dwCOLOR = NS_UITEXTCOLOR::GM_CHAR_NAME;
				}

				//if( pMyChar->GetCharData().m_Base.m_bGM )
				//{
				//	m_dwCOLOR = NS_UITEXTCOLOR::AQUABLUE;
				//}

                if (RANPARAM::emSkillTypeUI != EM_SKILLTYPE_NONE)
                {
                    for (int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; ++i)
                        m_bActionType[i] = pMyChar->m_bActionType[i];
                }

                // Note : SNS ��� ���� ����
                m_dwSNS = 0;

                if (pGaeaClient->GetSNSClient()->FBIsAuthed())
                    m_dwSNS |= SNSFLAG_FACEBOOK;
                if (pGaeaClient->GetSNSClient()->TTIsAuthed())
                    m_dwSNS |= SNSFLAG_TWITTER;
            }
            else
            {
                GLCharClient* const pChar = (GLCharClient* const)pActor;
				if ( pChar )
					m_dwCountryID = pChar->GetCountryInfo().wCountryID;

				// Note : �̸�ǥ�� ��������
				m_vPOS = pChar->GetPosBodyHeight();

                Faction::EMRELATION_TYPE emRelation(Faction::EMRELATION_ERROR);
                Faction::ManagerClient* pFactionMan = pLanMan->GetFactionManager();
                if ( pFactionMan )
                {
                    emRelation = pFactionMan->getRelationClient(pMyChar, pChar);
                }
                
                if ( emRelation == Faction::EMRELATION_ERROR )
                {
                    SetDataNonFaction(pGaeaClient, pChar);
                    return;
                }

                //	Note : ��� �˻�.;
                if ( pMyChar->ISCONFRONT_TAR ( STARGETID(CROW_PC, pChar->GetGaeaID()) ) )
                    emRelation = Faction::EMRELATION_ENEMY;
                
                //	����������.;
                if ( pMyChar->IS_PLAYHOSTILE ( pChar->GetCharID() ) )
                    emRelation = Faction::EMRELATION_ENEMY;

                const GLPartyID& PartyID = pGaeaClient->GetMyPartyClient()->GetPartyID();
                const bool bMyParty = ( PartyID.isValidParty() && (PartyID == pChar->GETPARTYID()) );

                // Note : PARTY �Ӽ�.;
                const bool bPartyMaster = pChar->IsPartyMaster();
                const bool bPartyMember = pChar->IsPartyMem();
                if ( emRelation == Faction::EMRELATION_ENEMY )
                {
                    if ( bPartyMaster )
                        m_emPARTY = PARTY_MASTER_TARGET;
                    else if ( bPartyMember )
                        m_emPARTY = PARTY_MEMBER_TARGET;
                    else
                        m_emPARTY = SOLO_TARGET;

                    m_dwTYPE |= TYPE_TARGET;
                }
                else if ( bMyParty )
                {
                    m_emPARTY = bPartyMaster ? PARTY_MASTER : PARTY_MEMBER;
                }
                else
                {
                    m_emPARTY = bPartyMaster ? PARTY_MASTER_GENERAL : OTHER_PLAYER;
                }

                //	Note : �̸� ��.
                switch ( emRelation )
                {
                case Faction::EMRELATION_ALLY:
                    m_dwCOLOR = bMyParty ? NS_UITEXTCOLOR::GREENYELLOW : NS_UITEXTCOLOR::DEFAULT;
                    break;
                case Faction::EMRELATION_NEUTRAL_ENEMY:
				case Faction::EMRELATION_NEUTRAL_ALLY:
                    //m_dwCOLOR = pChar->GET_PK_OR_GM_COLOR();
					if( pChar->m_dwUserLvl < USER_USER_GM )
						m_dwCOLOR = pChar->GET_PK_COLOR();
					else
						m_dwCOLOR = NS_UITEXTCOLOR::GM_CHAR_NAME;
                    break;
                case Faction::EMRELATION_ENEMY:
                    m_dwCOLOR = NS_UITEXTCOLOR::RED;
                    break;
                }

				if( pChar->GetCharData().m_Base.m_bGM )
				{
					m_dwCOLOR = NS_UITEXTCOLOR::GM_CHAR_NAME;
				}

                //	Note : �̸� ���..
                m_bPMARKET = pChar->m_sPMarket.IsOpen();
                StringCchCopy ( m_szPMARKET, CHAT_MSG_SIZE+1, pChar->m_sPMarket.GetTitle().c_str() );
                StringCchCopy ( m_szNICK, CHAR_SZNAME, pChar->GetNickName());

                if ( strlen(pChar->GetCharData().m_Base.szTitle) != 0 )
                {
                    StringCchCopy ( m_szTitle, EM_TITLE_MAXLENGTH, ID2GAMEINTEXT(pChar->GetCharData().m_Base.szTitle) );
                }
                else
                {
                    ZeroMemory(m_szTitle, EM_TITLE_MAXLENGTH);
                }

                m_nTitleClass = pChar->GetCharData().m_Base.byActivityClass;

                m_dwCLUB = pChar->GetClubID();
                m_dwCLUB_MARK_VER = pChar->GetCharData().m_Base.dwGuildMarkVer;


                if ( RANPARAM::emSkillTypeUI == EM_SKILLTYPE_ALL ) 
                {
                    for ( int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; ++i ) 
                    {
                        m_bActionType[i] = pChar->m_bActionType[i];
                    }
                }
                else if ( RANPARAM::emSkillTypeUI == EM_SKILLTYPE_PARTY ) 
                {
                    if ( m_emPARTY == PARTY_MASTER || m_emPARTY == PARTY_MEMBER ) 
                    {
                        for ( int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; ++i ) 
                        {
                            m_bActionType[i] = pChar->m_bActionType[i];
                        }
                    }
                }

                // Note : SNS ��� ���� ����
                m_dwSNS = 0;

                if ( pChar->m_byFB ) m_dwSNS |= SNSFLAG_FACEBOOK;
                if ( pChar->m_byTW ) m_dwSNS |= SNSFLAG_TWITTER;
            }
        }
        break;
    case CROW_NPC:
    case CROW_MOB:
    case CROW_MATERIAL:
        {
            GLCrowClient* const pCrow = (GLCrowClient* const) pActor;
            Faction::EMRELATION_TYPE emRelation(Faction::EMRELATION_ERROR);
            Faction::ManagerClient* pFactionMan = pLanMan->GetFactionManager();
            if ( pFactionMan )
            {
                emRelation = pFactionMan->getRelationClient(pMyChar, pActor);
            }

            if ( _emCrow == CROW_NPC )  // NPC�϶�;
            {
                switch( emRelation )
                {
                case Faction::EMRELATION_ENEMY:
                    m_dwCOLOR = NS_UITEXTCOLOR::RED;
                    break;
                case Faction::EMRELATION_NEUTRAL_ENEMY:
				case Faction::EMRELATION_NEUTRAL_ALLY:
                case Faction::EMRELATION_ALLY:
                case Faction::EMRELATION_ERROR:  // faction �̼�����;
                    m_dwCOLOR = NS_UITEXTCOLOR::ORANGE;
                    break;
                }
            }
            else if ( _emCrow == CROW_MATERIAL )  // ��� NPC�϶�;
            {
                WORD wMyLevel = pMyChar->GetLevel();
                WORD wMobLevel = pCrow->GetLevel();
                float fEXP_RATE = GLCONST_CHAR::GETEXP_RATE ( wMyLevel, wMobLevel );

                switch( emRelation )
                {
                case Faction::EMRELATION_ENEMY:
                case Faction::EMRELATION_ERROR:  // faction �̼�����;
                    {
                        if (fEXP_RATE == 0)
                            m_dwCOLOR = CROWCOLOR::MOB_COLOR_GRAY;
                        else
                            m_dwCOLOR = CROWCOLOR::MOB_COLOR_RED;
                    }
                    break;
                case Faction::EMRELATION_NEUTRAL_ENEMY:
				case Faction::EMRELATION_NEUTRAL_ALLY:
                    m_dwCOLOR = CROWCOLOR::MOB_COLOR_WHITE;
                    break;
                case Faction::EMRELATION_ALLY:
                    m_dwCOLOR = NS_UITEXTCOLOR::ORANGE;
                    break;
                }
            }
            else  // Mob�϶�;
            {
                switch ( emRelation )
                {
                case Faction::EMRELATION_ENEMY:  // ���� ��� �������ó�� �������� ���� ���� ǥ��;
                case Faction::EMRELATION_ERROR:  // faction �̼�����;
                    {
                        WORD wMyLevel = pMyChar->GetLevel();
                        WORD wMobLevel = pCrow->GetLevel();
                        float fEXP_RATE = GLCONST_CHAR::GETEXP_RATE ( wMyLevel, wMobLevel );

                        if (pCrow->IsNonDamaged() )
                            m_dwCOLOR = CROWCOLOR::MOB_COLOR_WHITE; // ���ݹ��� �ʴ� Crow���
                        else if ( fEXP_RATE==0 )
                            m_dwCOLOR = CROWCOLOR::MOB_COLOR_GRAY;
                        else
                            m_dwCOLOR = CROWCOLOR::MOB_COLOR_RED;

                        m_dwTYPE |= TYPE_TARGET;
                    }
                    break;
                case Faction::EMRELATION_NEUTRAL_ENEMY:  // �߸��� ��� ���;
				case Faction::EMRELATION_NEUTRAL_ALLY:
                    m_dwCOLOR = CROWCOLOR::MOB_COLOR_WHITE;
                    break;
                case Faction::EMRELATION_ALLY:  // �Ʊ��� ��� ��������;
                    m_dwCOLOR = NS_UITEXTCOLOR::ORANGE;
                    break;
                }
                // Note : ���� Crow�� Ÿ���� �ڽ��� ��쿡�� HP�� ���;
                if ( RANPARAM::bAUTO_CROW_HP 
                    && pCrow->GetTargetID().emCrow == CROW_PC 
                    && pCrow->GetTargetID().GaeaId == pMyChar->GetGaeaID() )
                {
                    m_bHP = true;
                }
                else
                {
                    m_bHP = false;
                }
                // Note : ü��
                m_fPERHP = static_cast<float>(pCrow->GETHP()) / static_cast<float>(pCrow->GETMAXHP());
            }

            // Note : �̸�ǥ�� ��������
            m_vPOS   = pCrow->GetPosBodyHeight();            
        }
        break;
    case CROW_SUMMON:
        {
            m_bSIMPLE = RANPARAM::bSIMPLE_SUM_NAME;

            GLSummonClient* const pSummon = (GLSummonClient* const)pActor;

            std::tr1::shared_ptr<GLCharClient> pChar = pGaeaClient->GetChar(pSummon->m_dwOwnerGaeaID);
            if ( pChar == NULL )
            {
                m_dwCOLOR = NS_UITEXTCOLOR::WHITE;
            }
            else
            {
                CROWREN TempCrowRen;
                TempCrowRen.SetData( pGaeaClient, pChar.get() );
                m_dwCOLOR = TempCrowRen.m_dwCOLOR;
            }

            // Note : ���� ��ȯ���� Ÿ��
            if ( RANPARAM::bAUTO_CROW_HP /*
                && pSummon->GetTargetID().emCrow == CROW_PC 
                && pSummon->GetTargetID().GaeaId == pMyChar->GetGaeaID()*/ )
            {
                m_bHP = true;
            }
            else
            {
                m_bHP = false;
            }

            // Note : �̸�ǥ�� ��������
            m_vPOS = pSummon->GetPosBodyHeight();

            // Note : ü��
            m_fPERHP = static_cast<float>(pSummon->GETHP()) / static_cast<float>(pSummon->GETMAXHP());
        }
        break;
    default:
        break;
    }
}

void CROWREN::SetDataNonFaction( GLGaeaClient* const pGaeaClient, GLCharacter* const pMYCHAR )
{
    m_emPARTY = OTHER_PLAYER;
    if (pMYCHAR->IsPartyMaster())
        m_emPARTY = PARTY_MASTER;
    else if (pMYCHAR->IsPartyMem())
        m_emPARTY = PARTY_MEMBER;

    m_dwTYPE = TYPE_NORMAL;
    if ( m_emPARTY==PARTY_MASTER || m_emPARTY==PARTY_MEMBER )		m_dwTYPE |= TYPE_OURS;

    PLANDMANCLIENT pLAND = pGaeaClient->GetActiveMap();
    if (pLAND)
    {
        if (pLAND->m_bClubBattle || pLAND->m_bClubBattleHall)
        {
            if (pMYCHAR->IsCDCertify())
                m_dwTYPE |= TYPE_CLUBCD;
        }
    }

    m_bPMARKET = pMYCHAR->m_sPMarket.IsOpen();
    StringCchCopy(m_szPMARKET, CHAT_MSG_SIZE+1, pMYCHAR->m_sPMarket.GetTitle().c_str());
    StringCchCopy(m_szNICK, CHAR_SZNAME, pMYCHAR->m_szNick);

    TITLEINFO sTitle;
    if (pMYCHAR->m_TitleManager.GetCurrentTitle(sTitle))
    {
        m_nTitleClass = sTitle.nClass;

        if (!sTitle.strTitle.empty())
            StringCchCopy(m_szTitle, EM_TITLE_MAXLENGTH, ID2GAMEINTEXT(sTitle.strTitle.c_str()));
        else
            ZeroMemory(m_szTitle, EM_TITLE_MAXLENGTH);
    }

    // Note : �̸�ǥ�� ��������
    m_vPOS = pMYCHAR->GetPosBodyHeight();

    m_dwCLUB = pGaeaClient->GetMyClub()->DbNum();
    m_dwCLUB_MARK_VER = pGaeaClient->GetMyClub()->m_dwMarkVER;

    //	Note : �̸� ��.
    if (m_dwTYPE & TYPE_OURS)
        m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
    else
	{
		if( pMYCHAR->m_dwUserLvl < USER_USER_GM )
			m_dwCOLOR = pMYCHAR->GET_PK_COLOR(); // �ڱ� �ڽ��� ���̵�
		else
			m_dwCOLOR = NS_UITEXTCOLOR::GM_CHAR_NAME;
        //m_dwCOLOR = pMYCHAR->GET_PK_OR_GM_COLOR();
	}

	//if( pMYCHAR->GetCharData().m_Base.m_bGM )
	//{
	//	m_dwCOLOR = NS_UITEXTCOLOR::AQUABLUE;
	//}

    if (RANPARAM::emSkillTypeUI != EM_SKILLTYPE_NONE)
    {
        for (int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; ++i)
            m_bActionType[i] = pMYCHAR->m_bActionType[i];
    }

    // Note : SNS ��� ���� ����
    m_dwSNS = 0;

    if (pGaeaClient->GetSNSClient()->FBIsAuthed())
        m_dwSNS |= SNSFLAG_FACEBOOK;
    if (pGaeaClient->GetSNSClient()->TTIsAuthed())
        m_dwSNS |= SNSFLAG_TWITTER;
}

void CROWREN::SetDataNonFaction( GLGaeaClient* const pGaeaClient, GLCharClient* const pCHAR )
{
    m_dwTYPE = TYPE_NORMAL;

    GLCharacter* const pMYCHAR = pGaeaClient->GetCharacter();
    PLANDMANCLIENT pLAND = pGaeaClient->GetActiveMap();
    if ( pLAND == NULL )
        return;

    //	Note : ��� �˻�.
    EMCONFT_TYPE emTYPE = pMYCHAR->GetConftingType();
    BOOL bCONFT_TAR = pMYCHAR->ISCONFRONT_TAR ( STARGETID(CROW_PC, pCHAR->GetGaeaID()) );
    if ( bCONFT_TAR )
    {
        m_dwTYPE |= TYPE_TARGET;

        if ( pCHAR->IsPartyMaster () )
            m_dwTYPE |= TYPE_PARTYMASTER;
        else if ( pCHAR->IsPartyMem () )
            m_dwTYPE |= TYPE_PARTYMEMBER;




        if (emTYPE == EMCONFT_GUILD)
        {
            if ( pCHAR->IsClubMaster () )
                m_dwTYPE |= TYPE_CLUBMASTER;
            else
                m_dwTYPE |= TYPE_CLUBMEMBER;
        }
    }
    else
    {
        //if ( emTYPE == EMCONFT_GUILD )
        //{
        //	const DWORD& dwCharID = pCHAR->GetCharID();
        //	if ( pMYCHAR->m_sCLUB.ISCONFT_MEMBER ( dwCharID ) )
        //	{
        //		m_dwTYPE |= TYPE_CLUBMEMBER;
        //	}
        //}

        //	Note : ���� ��Ƽ������ �˻�.
        //
        const GLPartyID& PartyID = pGaeaClient->GetMyPartyClient()->GetPartyID();
        if ( PartyID.isValidParty() && (PartyID == pCHAR->GETPARTYID()) )
        {
            if (pCHAR->IsPartyMaster())
                m_dwTYPE |= TYPE_OURS | TYPE_PARTYMASTER;
            else
                m_dwTYPE |= TYPE_OURS | TYPE_PARTYMEMBER;
        }

        //	��Ƽ �������� ���
        if ( pCHAR->IsPartyMaster() )
            m_dwTYPE |= TYPE_PARTYMASTER;
        else if (pCHAR->IsPartyMem())
            m_dwTYPE |= TYPE_PARTYMEMBER;

        //	Ŭ�� �������� ���.
        if (pCHAR->IsClubMaster())
            m_dwTYPE |= TYPE_CLUBMASTER;

        //	
        if (pLAND->m_bClubBattle || pLAND->m_bClubBattleHall)
        {
            if (pCHAR->IsCDCertify())
                m_dwTYPE |= TYPE_CLUBCD;
        }
    }

    bool bTarget(false);
    GLClubClient* pMyClub = pGaeaClient->GetMyClub();
    if ( pMyClub == NULL )
        return;

    if ( pLAND->IsPKZone() )
    {
        SDROP_CHAR &sCHARDATA_TAR = pCHAR->GetCharData();        

        //	����Ŭ����;
        if ( pLAND->m_bClubBattle )
        {
            bTarget = (pMyClub->IsAllianceGuild_CHIEF(sCHARDATA_TAR.m_Base.dwGuild) == false);
        }
        // CDM;
        else if (pLAND->m_bClubDeathMatch && pGaeaClient->IsClubDeathMatch())
        {
            bTarget = (pMyClub->DbNum() != sCHARDATA_TAR.m_Base.dwGuild);
        }
        // CTF;
        else if ( pGaeaClient->IsCTFPlayer() )
        {
            m_dwTYPE |= TYPE_CTF;

            bTarget = ( pMYCHAR->m_wSchool != sCHARDATA_TAR.m_Base.wSchool );
        }
        // PKMAP;
        else if ( pLAND->IsPKMap() )
        {
            bTarget = ( pMyClub->DbNum() == 0 || pMyClub->DbNum() != sCHARDATA_TAR.m_Base.dwGuild );
        }
        //	����������.;
        else if ( pMYCHAR->IS_PLAYHOSTILE ( sCHARDATA_TAR.m_Base.dwCharID ) )
        {
            bTarget = true;
        }
        //	���⼭ ���� üũ�ϴ°��� �������� �����Ҽ� ����.;
        //	������ üũ ;
        else if ( sCHARDATA_TAR.m_Base.wLevel <= GLCONST_CHAR::nPK_LIMIT_LEVEL )
        {
            bTarget = false;
        }
        //	PK ����;
        else if ( pGaeaClient->IsPKServer() )
        {
            bTarget = (pMYCHAR->m_wSchool != sCHARDATA_TAR.m_Base.wSchool && !pMyClub->IsAllianceGuild_CHIEF(sCHARDATA_TAR.m_Base.dwGuild));
        }
        // ���� �̺�Ʈ�ϰ��.;
        else if ( pGaeaClient->IsBRIGHTEVENT() )
        {
            bTarget = true;
        }
        //	�б��� ���� PK.;
        else if ( !GLCONST_CHAR::bSCHOOL_FREE_PK_Z_FORCED ) 
        {
            bTarget = ( pGaeaClient->IsSchoolFreePk () && pMYCHAR->m_wSchool!=sCHARDATA_TAR.m_Base.wSchool );
        }
    }

    if ( bTarget )
    {
        m_dwTYPE |= TYPE_TARGET;
    }

    // Ŭ�� ��Ʋ���ϰ��;
    if ((pMyClub->IsBattle(pCHAR->GETCLUBID()) || pMyClub->IsBattleAlliance(pCHAR->GETALLIANCEID())) && pLAND->IsClubBattleZone())		
    {
        m_dwTYPE |= TYPE_CLUBBATTLE;
    }

    // Note : PARTY �Ӽ�.
    m_emPARTY = OTHER_PLAYER;
    if ( m_dwTYPE & TYPE_PARTYMASTER )
        m_emPARTY = PARTY_MASTER_GENERAL;

    if ( m_dwTYPE & TYPE_TARGET )
    {
        if ( m_dwTYPE & TYPE_PARTYMASTER )
            m_emPARTY = PARTY_MASTER_TARGET;
        else if ( m_dwTYPE & TYPE_PARTYMEMBER )
            m_emPARTY = PARTY_MEMBER_TARGET;
        else
            m_emPARTY = SOLO_TARGET;
    }
    else  if ( m_dwTYPE & TYPE_OURS )
    {
        if ( m_dwTYPE & TYPE_PARTYMASTER )
            m_emPARTY = PARTY_MASTER;
        if ( m_dwTYPE & TYPE_PARTYMEMBER )
            m_emPARTY = PARTY_MEMBER;
    }

    //	Note : �̸� ��.
    if ( m_dwTYPE&TYPE_OURS )
    {
        m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
    }
    else if ( m_dwTYPE&TYPE_CLUBBATTLE )
    {
        DWORD nIndex = 0;        
        CLUB_BATTLE_ITER pos = pMyClub->m_mapBattle.begin();
        CLUB_BATTLE_ITER pos_end = pMyClub->m_mapBattle.end();
        for ( ; pos != pos_end; ++pos)
        {
            if (pCHAR->GETCLUBID() == (*pos).first)
                break;
            if (pCHAR->GETALLIANCEID() == (*pos).first)
                break;
            nIndex++;
        }
        nIndex = nIndex % NS_UITEXTCOLOR::MAX_CLUB_BATTLE_COLOR;

        m_dwCOLOR = NS_UITEXTCOLOR::CLUB_BATTLE_COLOR[nIndex];
    }
    else if ( m_dwTYPE&TYPE_CTF )
    {
        if ( pMYCHAR->m_wSchool != pCHAR->GetCharSchool() )
        {
            m_dwCOLOR = NS_UITEXTCOLOR::RED;
        }
        else
        {
            m_dwCOLOR = NS_UITEXTCOLOR::DEFAULT;
        }
    }
    else if ( m_dwTYPE&TYPE_TARGET )
    {
        m_dwCOLOR = NS_UITEXTCOLOR::RED;
    }
    else
    {
        m_dwCOLOR = NS_UITEXTCOLOR::DEFAULT;
        //m_dwCOLOR = pCHAR->GET_PK_OR_GM_COLOR();
		if( pCHAR->m_dwUserLvl < USER_USER_GM )
			m_dwCOLOR = pCHAR->GET_PK_COLOR();
		else
			m_dwCOLOR = NS_UITEXTCOLOR::GM_CHAR_NAME;
    }

	if( pCHAR->GetCharData().m_Base.m_bGM )
	{
		m_dwCOLOR = NS_UITEXTCOLOR::GM_CHAR_NAME;
	}

    //	Note : �̸� ���..
    m_bPMARKET = pCHAR->m_sPMarket.IsOpen();
    StringCchCopy ( m_szPMARKET, CHAT_MSG_SIZE+1, pCHAR->m_sPMarket.GetTitle().c_str() );

    StringCchCopy ( m_szNICK, CHAR_SZNAME, pCHAR->GetCharData().m_Base.szNick );

    if ( strlen(pCHAR->GetCharData().m_Base.szTitle) != 0 )
    {
        StringCchCopy ( m_szTitle, EM_TITLE_MAXLENGTH, ID2GAMEINTEXT(pCHAR->GetCharData().m_Base.szTitle) );
    }
    else
    {
        ZeroMemory(m_szTitle, EM_TITLE_MAXLENGTH);
    }

    m_nTitleClass = pCHAR->GetCharData().m_Base.byActivityClass;


    // Note : �̸�ǥ�� ��������
    m_vPOS = pCHAR->GetPosBodyHeight();

    m_dwCLUB = pCHAR->GetCharData().m_Base.dwGuild;
    m_dwCLUB_MARK_VER = pCHAR->GetCharData().m_Base.dwGuildMarkVer;


    if ( RANPARAM::emSkillTypeUI == EM_SKILLTYPE_ALL ) 
    {
        for ( int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; ++i ) 
        {
            m_bActionType[i] = pCHAR->m_bActionType[i];
        }
    }
    else if ( RANPARAM::emSkillTypeUI == EM_SKILLTYPE_PARTY ) 
    {
        if ( m_emPARTY == PARTY_MASTER || m_emPARTY == PARTY_MEMBER ) 
        {
            for ( int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; ++i ) 
            {
                m_bActionType[i] = pCHAR->m_bActionType[i];
            }
        }
    }

    // Note : SNS ��� ���� ����
    m_dwSNS = 0;

    if ( pCHAR->m_byFB ) m_dwSNS |= SNSFLAG_FACEBOOK;
    if ( pCHAR->m_byTW ) m_dwSNS |= SNSFLAG_TWITTER;
}

void CROWREN::SetData( GLGaeaClient* const pGaeaClient, PGLPETCLIENT pMyPet )
{
    m_emCROW = CROW_PET;
    m_dwID = pMyPet->m_dwGUID;

    StringCchCopy(m_szNAME, CHAR_SZNAME, pMyPet->GetName());

    // Note : �̸�ǥ�� ��������
    m_vPOS = pMyPet->GetPosition();
    m_vPOS.y += pMyPet->m_fHeight;

    // ����
    m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
}

void CROWREN::SetData( GLGaeaClient* const pGaeaClient, std::tr1::shared_ptr<GLAnyPet> pAnyPet )
{
    m_emCROW = CROW_PET;
    m_dwID = pAnyPet->m_dwGUID;

    StringCchCopy ( m_szNAME, PETNAMESIZE+1, pAnyPet->m_szName );

    // Note : �̸�ǥ�� ��������
    m_vPOS = pAnyPet->GetPosition();
    m_vPOS.y += pAnyPet->m_fHeight;

    // ����
    m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
}

void CROWREN::SetData( GLGaeaClient* const pGaeaClient, std::tr1::shared_ptr<CItemClientDrop> spITEM )
{
    m_emCROW = CROW_ITEM;
	m_dwID = spITEM->dwGlobID;

	SITEM *pITEM_DATA = GLogicData::GetInstance().GetItem(spITEM->sItemClient.sNativeID);
	if ( !pITEM_DATA )
	{
		// Client �����Ͱ� �������̶� Data �� ���� ��ġ�� �������� �Ǿ� ���� �ʴ°�~?
		m_vPOS = spITEM->vPos;

		return;
	}

	/*// ����� �������� ��ħ �������� ��� ���� ǥ��
	// ��ȹ������ �� �ʿ� ���ٰ� �ؼ� �ּ� ó���� �Ѥ�;
	#ifdef DAUMTESTPARAM
	{
		if( pITEM->sItemClient.wTurnNum > 1 )
		{
			CString strDescText = ID2GAMEWORD( "ITEM_TURN_INFO", 0 );
			CString strName;
			strName.Format( "%s [%s:%d]", pITEM_DATA->GetName(), strDescText.GetString(), pITEM->sItemClient.wTurnNum );
			StringCchCopy( m_szNAME, CHAR_SZNAME, strName.GetString() );
		}
		else
		{
			StringCchCopy( m_szNAME, CHAR_SZNAME, pITEM_DATA->GetName() );
		}
	}
	#else
	*/
	{
		StringCchCopy( m_szNAME, CHAR_SZNAME, pITEM_DATA->GetName() );
	}
	//#endif

    //m_dwCOLOR = NS_UITEXTCOLOR::ITEMMONEY;
	m_dwCOLOR = ITEM_INFOR_COLOR::dwItemRank[ pITEM_DATA->sBasicOp.emLevel ];

    m_vPOS = spITEM->vPos;
}

void CROWREN::SetData( GLGaeaClient* const pGaeaClient, std::tr1::shared_ptr<CMoneyClientDrop> spMONEY )
{
    m_emCROW = CROW_MONEY;
    m_dwID = spMONEY->dwGlobID;

    // ����� ���� �ڸ��� ���� ǥ��
    //#if defined(KR_PARAM) || defined(KRT_PARAM)
    //{
        CString strMoney = NS_UITEXTUTIL::MAKE_MONEY_FORMAT( spMONEY->lnAmount, 3, "," );
        StringCchCopy( m_szNAME, CHAR_SZNAME, strMoney.GetString() );
    //}
    //#else
    //{
    //	//std::strstream strSTEAM;
    //	//strSTEAM << pMONEY->lnAmount << std::ends;

    //	TCHAR szMoney[128] = {0};
    //	_snprintf( szMoney, 128, "%I64d", pMONEY->lnAmount );

    //	StringCchCopy ( m_szNAME, CHAR_SZNAME, szMoney );
    //	//strSTEAM.freeze( false );
    //}
    //#endif
    
    //m_dwCOLOR = NS_UITEXTCOLOR::ITEMMONEY;
	m_dwCOLOR = ITEM_INFOR_COLOR::dwMoney;

    m_vPOS = spMONEY->vPos;
}