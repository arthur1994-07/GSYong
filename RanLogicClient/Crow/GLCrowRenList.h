#pragma once

//#include <map>
//#include <string>

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../Pet/GLAnyPet.h"
#include "../Pet/GLPetClient.h"
#include "../Summon/GLSummonClient.h"
#include "../Material/GLMaterialClient.h"

namespace CROWCOLOR
{
	const DWORD MOB_COLOR_GRAY	= D3DCOLOR_ARGB(255,184,184,184);
	const DWORD MOB_COLOR_GREEN	= D3DCOLOR_ARGB(210,39,207,102);
	const DWORD MOB_COLOR_RED	= D3DCOLOR_ARGB(255,255,0,0);
	const DWORD MOB_COLOR_WHITE	= D3DCOLOR_ARGB(255,255,255,255);
};

enum DISP_PARTY
{
	//	번호 변경 불가 ( 텍스쳐 타입 )
	PARTY_MEMBER			= 0,
	PARTY_MASTER			= 1,
	PARTY_MEMBER_TARGET		= 2,
	PARTY_MASTER_TARGET		= 3,
	PARTY_MASTER_GENERAL	= 4,

    SOLO_TARGET				= 1001,
	OTHER_PLAYER			= 1002,
	NODATA					= USHRT_MAX
};

enum DISP_TYPE
{
	TYPE_NORMAL			= 0x00,	 //	일반
	TYPE_TARGET			= 0x01,	 //	타겟
	TYPE_OURS			= 0x02,  //	내가 속한 곳인가
	TYPE_PARTYMASTER	= 0x04,  //	파티 마스터
	TYPE_PARTYMEMBER	= 0x08,	 //	파티 멤버
	TYPE_CLUBMASTER		= 0x10,  //	클럽 마스터
	TYPE_CLUBMEMBER		= 0x20,	 //	클럽 멤버
	TYPE_CLUBCD			= 0x40,	 //	CD 인증
	TYPE_CLUBBATTLE		= 0x80,  //	클럽배틀
	TYPE_CTF			= 0x100, //	CTF
};

enum SNS_FLAG
{
    SNSFLAG_FACEBOOK = 0x0001,
    SNSFLAG_TWITTER  = 0x0002,
};

class GLCharacter;
class GLCharClient;
class GLCrowClient;
class CItemClientDrop;
class CMoneyClientDrop;
class GLGaeaClient;
struct GLLANDMARK;

struct CROWREN
{
	EMCROW		m_emCROW;
	DWORD		m_dwID;
	WORD		m_wSCHOOL;

	DWORD		m_dwCOLOR;
	DISP_PARTY	m_emPARTY;
	DWORD		m_dwTYPE;
    bool		m_bPMARKET;
    DWORD		m_dwSNS;

	char		m_szNAME[CHAR_SZNAME];
	char		m_szPMARKET[CHAT_MSG_SIZE+1];
	char		m_szNICK[CHAR_SZNAME];

    int         m_nTitleClass;
    char        m_szTitle[EM_TITLE_MAXLENGTH];

	DWORD		m_dwCLUB;
	DWORD		m_dwCLUB_MARK_VER;

	D3DXVECTOR3	m_vPOS;
    BOOL        m_bHP;
    float       m_fPERHP;
	DWORD		m_dwCOUNT;

	BOOL		m_bSIMPLE;
	bool		m_bActionType[SKILL::EMACTION_TYPE_NSIZE];
	BOOL		m_bGM;

	DWORD		m_dwCountryID;

	CROWREN () 
		: m_emCROW(CROW_PC)
		, m_dwID(0)
		, m_wSCHOOL(NODATA)
		
		, m_dwCOLOR(NS_UITEXTCOLOR::DEFAULT)
		, m_emPARTY(NODATA)
		, m_dwTYPE(TYPE_NORMAL)
        , m_bPMARKET(false)
        , m_dwSNS(0)
		
		, m_vPOS(0,0,0)
		, m_dwCLUB(CLUB_NULL)
		, m_dwCLUB_MARK_VER(0)
		, m_dwCOUNT(0)

        , m_bHP (FALSE)
        , m_fPERHP (0.0f)

        , m_bSIMPLE ( FALSE )

        , m_nTitleClass( ACTIVITY_CLASS_SIZE )
		, m_bGM( FALSE )

		, m_dwCountryID( 0 )
	{
		memset(m_szNAME, 0, sizeof(char) * (CHAR_SZNAME));
		memset(m_szPMARKET, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
		memset(m_szNICK, 0, sizeof(char) * (CHAR_SZNAME));
		memset(m_bActionType,0,sizeof(char) * SKILL::EMACTION_TYPE_NSIZE );
        memset(m_szTitle,0, sizeof(char) * (EM_TITLE_MAXLENGTH));
	}

	bool DIFFERENT ( const CROWREN &sINFO );

	bool IsCLUB () const { return m_dwCLUB!=CLUB_NULL; }

    // PC,NPC,MOB,SUMMON;
    void SetData( GLGaeaClient* const pGaeaClient, ClientActor* const pActor );

    // PET,ITEM,MONEY;
    void SetData( GLGaeaClient* const pGaeaClient, PGLPETCLIENT pMyPet );
    void SetData( GLGaeaClient* const pGaeaClient, std::tr1::shared_ptr<GLAnyPet> pAnyPet );
    void SetData( GLGaeaClient* const pGaeaClient, std::tr1::shared_ptr<CItemClientDrop> spITEM );
    void SetData( GLGaeaClient* const pGaeaClient, std::tr1::shared_ptr<CMoneyClientDrop> spMONEY );

    // OldVersion(faction System 이전);
    void SetDataNonFaction( GLGaeaClient* const pGaeaClient, GLCharacter* const pMYCHAR );
    void SetDataNonFaction( GLGaeaClient* const pGaeaClient, GLCharClient* const pCHAR );

    // LandMark라는것도 있었음(안써서 지움);

	void UPDATE ( DWORD _COUNT, D3DXVECTOR3 _vPOS );
};

