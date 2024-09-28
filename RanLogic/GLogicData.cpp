#include "pch.h"

#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Compress/Unzipper.h"
#include "../SigmaCore/File/StringFile.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/G-Logic/GLPeriod.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"
#include "../EngineLib/Common/gltexfile.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "./Activity/ActivityBase.h"
#include "./Character/GLCharData.h"
#include "./Crow/GLCrowData.h"
#include "./Crow/GLCrowDataMan.h"
#include "./GLCommentFile.h"
#include "./GLStringTable.h"
#include "./GLColorTable.h"
#include "./GLUseFeatures.h"
#include "./Product/GLProductRecipeMan.h"
#include "./Item/GLItemMan.h"
#include "./NpcTalk/NpcTalkMan.h"
#include "./Post/GLPostData.h"
#include "./Quest/GLQuestMan.h"
#include "./SNS/GLSNSData.h"
#include "./Transport/GLBusStation.h"
#include "./Transport/GLTaxiStation.h"
#include "./GLogicData.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
GLCONST_CHARCLASS::~GLCONST_CHARCLASS()
{
	SAFE_DELETE(m_pSkinDataDummy);
}
//----------------------------------------------------------------------------------------
void GLCONST_CHARCLASS::LoadAniSet(char* szSkinObj, bool bToolmode)
{
	if (szSkinObj[0]=='\0')
        return;

	m_pSkinDataDummy = new DxSkinDataDummy();
	BOOL bOK = m_pSkinDataDummy->LoadFile(szSkinObj);
	if (!bOK)
	{
        std::string Msg(sc::string::format("[%1%] Skin File Load Fail.", szSkinObj));
		MessageBox(NULL, Msg.c_str(), "ERROR", MB_OK);
		SAFE_DELETE(m_pSkinDataDummy);
		return;
	}

	size_t nSIZE = m_pSkinDataDummy->m_vecANIMINFO.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		const SANIMCONINFO *pAnimConInfo = m_pSkinDataDummy->m_vecANIMINFO[n];

		SANIATTACK sAniAttack;
		sAniAttack.Assign ( pAnimConInfo );
		m_ANIMATION[pAnimConInfo->m_MainType][pAnimConInfo->m_SubType].push_back ( sAniAttack );
	}

	if ( bToolmode == false )
		SAFE_DELETE(m_pSkinDataDummy);
}

void GLCONST_CHARCLASS::ClearAniSet ()
{
	for ( int i=0; i<AN_SUB_00_SIZE; ++i )
	for ( int j=0; j<AN_TYPE_SIZE; ++j )
	{
		m_ANIMATION[j][i].clear ();
	}

	SAFE_DELETE(m_pSkinDataDummy);
}


//----------------------------------------------------------------------------------------
namespace GLCONST_CHAR
{
	BOOL		bTESTSERVER			= FALSE;			//	�׽�Ʈ �������� ����.
	INT			nUI_KEYBOARD		= 0;				// �������̽� Ű���� ����. �⺻ ���� Ű����, UI �Ѵ� ���
	INT			nUI_DEFAULTKEYTYPE	= 1;				// �⺻ ŰŸ��
	BOOL		bBATTLEROYAL		= FALSE;			// ��Ʋ�ξ�
	INT			nMAX_FRIEND_NUMBER	= 100;				// �ִ� ģ��, ���� ��� ���� - ����
    INT			nMAX_FRIEND_LIST	= 60;				// �ִ� ��� ���� - UI���� 
    INT			nMAX_FRIEND_GROUP	= 5;				// �ִ� ��� ���� - UI���� 
    INT			nMAX_FRIEND_BLOCK	= 50;				// ���� ��� ���� - UI����
	WORD		wLEVEL_FOR_EXTREME	= 190;				// �ذ��θ� �����ϱ� ���� �����Ǿ�� �ϴ� ����(lv.190)
	WORD		wSTART_LEVEL_FOR_EXTREME = 100;
	WORD		wLEVEL_FOR_ACTOR	= 150;				// ������ ���� ���� ����;
    BYTE        ExcessExpProcessType = 0;   // �ʰ� ����ġ ó�� Ÿ��. 0 : ������ 1 : �ذ��θ� 2 : ���

	float		fFIELD_NEXTPICK		= 0.4f;
	WORD		wSERVER_NORMAL		= 40;				//	���� ����.
	WORD		wSERVER_CONGEST		= 70;				//	���� ȥ��.
	WORD		wSERVER_NOVACANCY	= 95;				//  ���� Ǯ, ���� �Ұ�

//	float		fDISPRICE			= 0.3f;				//	������ ���Ǹ� ����.
	WORD		wMAXITEM_AGE		= 120;
	WORD		wMAXITEM_HOLD		= 10;

	float		fCHAR_HEIGHT[GLCI_NUM_ACTOR]	= 
	{ 
		20.0f, 20.0f,
		17.0f, 17.0f,
		20.0f, 17.0f,
		17.0f, 17.0f,
		20.0f, 20.0f,
		12.0f, 12.0f,
		13.0f, 13.0f,
		14.0f, 14.0f,
		17.0f, 17.0f,
		20.0f, 17.0f,
	};

	//	Note : ĳ���� �⺻ ���.
	//
	WORD		wSCHOOLNUM = 1;	
	std::string	strSCHOOLNAME[MAX_SCHOOL]	= { "����" };
	SNATIVEID	nidSTARTMAP[MAX_SCHOOL]		= { SNATIVEID(0,0) };
	DWORD		dwSTARTGATE[MAX_SCHOOL]		= { 0 };	

    WORD        wENABLE_NEW_CLASS = RESTRICT_SCIENTIST_NEW_CLASS;

	BOOL		bPARTY_2OTHERSCHOOL = TRUE;
	BOOL		bCLUB_2OTHERSCHOOL = TRUE;

    float fCHAT_OVERINPUT_TIMER = 0.5f; // ���� ���� ��� �� ���� �Է� �ð��� ���� ��(��).
    WORD wCHAT_OVERINPUT_CANCEL_TIMER = 180; // ���� ������� ���ӽð�(��).
    int nCHAT_OVERINPUT_COUNT = 3; // ���� ���� ��� �� ���� �Է� ���ڿ� ���� �Է� ��� Ƚ��.

    SNATIVEID   nidREBIRTH_RECOMMEND_ITEM = SNATIVEID(0,0);    // ��Ȱ â���� ��õ�Ǵ� ������ �̹����� ID.
    std::string strREBIRTH_RECOMMEND_ITEM = "��ȥ��";    // ��Ȱ â���� ��õ�Ǿ� ����Ʈ �������� �˻��� �������� �̸�.
    WORD wAUTO_REBIRTH_TIMER = 60; // ��Ȱ â���� �ڵ� ��Ȱ �ð�.

    SNATIVEID nidDIALOGUE_RECOMMEND_ITEM = SNATIVEID(0,0);    // NPC ��ȭ â���� ��õ�Ǵ� ������ �̹����� ID.
    std::string strDIALOGUE_RECOMMEND_ITEM = "�ҹ���";    // NPC ��ȭ â���� ��õ�Ǿ� ����Ʈ �������� �˻��� �������� �̸�.

    float fBECOME_AWAY_TIME = 1800.0f; // �Է��� ���� �� �ڸ���� ���� �ɸ��� �ð�(��).
    int nAWAY_GESTURE_NUMBER = 29;    // �ڸ���� �ÿ� ĳ���Ͱ� ���� ������ ��ȣ. 29 = �ɱ�.

    WORD wNON_APPLY_QBOX_ON_VEHICLE_NUM = 0;
    WORD wNON_APPLY_QBOX_ON_VEHICLE[QUESTION_SIZE] = 
    {
    }; // Ż �� ž�½ÿ� ���� �� �� QBOX.
    WORD wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM = 0;
    WORD wNON_APPLY_SKILL_TYPE_ON_VEHICLE[SKILL::FOR_TYPE_SIZE] = 
    {
    }; // Ż �� ž�½ÿ� ���� �� �� ��ų Ÿ�Ե�.
    WORD wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM = 0;
    WORD wNON_APPLY_SKILL_IMPACT_ON_VEHICLE[EIMPACTA_SIZE] = 
    {
    }; // Ż �� ž�½ÿ� ���� �� �� ��ų ȿ����.
    WORD wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM = 0;
    WORD wNON_APPLY_SKILL_SPEC_ON_VEHICLE[EMSPECA_NSIZE] = 
    {
    }; // Ż �� ž�½ÿ� ���� �� �� ��ų Ư��ȿ����.

    WORD wMaxRebuyList = 100;
    float fRebuyMultiplePrice = 1.0f;
    WORD wMinRebuyPrice = 1;

	WORD		wMAX_LEVEL			= 255;			//	���� �ִ� ����.
	WORD		wMAX_EXTREME_LEVEL	= 255;			//	�ذ��� ���� �ִ� ����.
	WORD		wLVL_STATS_P		= 3;			//	������ ���� ����Ʈ �ο���.
	WORD		wLVL_2ndSTATS_P		= 4;			//	������ ���� ����Ʈ �ο���. �ذ���
	float		fLVL_EXP_G			= 25.0f;		//	���� ����ġ ���⿡ �ʿ��� ȯ�� ����.
	float		fLVL_EXP_S			= 1.3f;			//	���� ����ġ ���⿡ �ʿ��� ȯ�� ����.
	float		fREC_EXP_RATE		= 2.0f;			//	������ ȸ������ �ٶ� ȹ���ϴ� ����ġ�� ����.
	float		fKILL_EXP_RATE		= 0.5f;			//	"��ȹ�� ���� ����ġ" * ���϶� ����ġ.

	float	fDIE_DECEXP[DIE_DECEXP_NUM] =		// ������ ���̴� ����ġ ����
	{
		0, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	float	fDIE_RECOVERYEXP[DIE_DECEXP_NUM] =	// �׾����� ȸ���Ҽ� �ִ� ����ġ ����
	{
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	float	fEXP_RATE_MONEY[DIE_DECEXP_NUM]	=	// ����ġ�� �ݾ� ����
	{
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	//WORD		wGRADE_MAX			= 9;			//	���� �Ҽ� �ִ� ��� ����ġ 
	//WORD		wGRADE_MAX_REGI		= 9;

	//WORD		wDAMAGE_GRADE		= 3;			//	�� ��޴� ���� ���ݷ� ��ġ.
	//WORD		wDEFENSE_GRADE		= 2;			//	�� ��޴� ���� ���� ��ġ.

	//float		fDAMAGE_GRADE		= 10;			//	�� ��޴� ������ ���ݷ� ��ġ.
	//float		fDEFENSE_GRADE		= 5;			//	�� ��޴� ������ ���� ��ġ.

	//float		fDAMAGE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH]	=	// �ֻ��� ��� ���ݷ� ������
	//{
	//	1.0f, 1.0f, 1.0f, 1.0f,	1.0f, 1.0f
	//};

	//float		fDEFENSE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH]	=	// �ֻ��� ��� ���ݷ� ������
	//{
	//	1.0f, 1.0f, 1.0f, 1.0f,	1.0f, 1.0f
	//};

	//WORD		wUSE_GRADE_NUM[GRADE_LIMIT_MAX-GRADE_HIGH]	=	// �ֻ��� ��޽� �ʿ��� ������ ����
	//{
	//	1, 2, 3, 4, 5, 6	
	//};

	//WORD		wRESIST_FIRE_GRADE	= 2;			//	�� ��޴� ���� ����(ȭ) ��ġ.
	//WORD		wRESIST_ICE_GRADE	= 2;			//	�� ��޴� ���� ����(��) ��ġ.
	//WORD		wRESIST_ELEC_GRADE	= 2;			//	�� ��޴� ���� ����(��) ��ġ.
	//WORD		wRESIST_POISON_GRADE= 2;			//	�� ��޴� ���� ����(��) ��ġ.
	//WORD		wRESIST_SPIRIT_GRADE= 2;			//	�� ��޴� ���� ����(��) ��ġ.


	WORD		wLVL_SKILL_P		= 1;			//	������ ��ų ����Ʈ �ο���.
	WORD		wLVL_2ndSKILL_P		= 2;			//	������ ��ų ����Ʈ �ο���. �ذ���

	WORD		wMAXATRANGE_SHORT	= 2;			//	���� ���ݰŸ�.
	float		fPUSHPULL_VELO		= 40.0f;		//	�аų� ��涧 �̵� �ӵ�.
    WORD		wMAXMOB_TRACKING	= 3;			//	�ִ� �� ���� ��.
    float       fNPC_INTERACTION_RANGE   = 30.0f;    //  NPC�� ��ȣ�ۿ� �� �� �ִ� �Ÿ�;
    float       fNPC_INTERACTION_ADJUST  = 20.0f;    //  NPC�� ��ȣ�ۿ��� �Ҷ� ���� ���� ���� (����, ����Ʈ, ���� ���� �� �Ŵ� ��ġ���� ���� �˳��ϰ� ��ȿ�� ����)

	int			nHitRateMinimum		= 30;			// ���� ���߷�;
	int			nHitRateMaximum		= 99;			// �ִ� ���߷�;
	float		fHitRatePercentUnit = 30.0f;		// ���߷��� ��з��� ��ȯ�� �����ų ����;
	float		fAvoidRatePercentUnit = 20.0f;		// ȸ�Ƿ��� ��з��� ��ȯ�� �����ų ����;
	float		fAttackSuccesRate_LvScale = 1.7f;			// ���ݼ����� ���Ŀ��� ���Ǵ� ���� ���� �����;
    float		fMOB_TRACING		= 90.0f;		//	�� ���� �Ÿ�.
	float		fLOW_SEED_DAMAGE	= 0.05f;		//	damage ���� ���� ���� damage ����.
	float		fCONFT_SPTY_EXP		= 0.005f;		//	�б� ��� �¸��ÿ� ��ü ����ġ �й�.

	WORD		wBODYRADIUS			= 4;			//	ĳ���� ��ü �ݰ�.
	float		fWALKVELO			= 10.0f;		//	ĳ���� �ȱ� �ӵ�.
	float		fRUNVELO			= 36.0f;		//	ĳ���� �ٱ� �ӵ�.

	float		fREACT_VALID_SCALE	= 4.0f;				//	�׼��� ��ȿ �Ÿ� ����.
	float		fATTACK_RANGE_ADJUST_RATE = 1.0f;	// �Ϲ� ���� ���� ���� ����;

    float       fENDURE_VARY_MAX    = 10.0f;        //  ������ ��ų �ִ� ���� ����.
	float		fUNIT_TIME  		= 1.0f;		    //	ȸ�� ���� �ð�. (Actor ��ü);
	float		fHP_INC_PER		    = 0.2f * 0.01f;	//	�ɸ��� �����ð��� HP ȸ����(%);
	float		fMP_INC_PER			= 0.4f * 0.01f;	//	�ɸ��� �����ð��� MP ȸ����(%);
	float		fSP_INC_PER			= 0.4f * 0.01f;	//	�ɸ��� �����ð��� SP ȸ����(%);

	float		fHP_INC				= 0;			//	�ɸ��� �����ð��� HP ȸ����;
	float		fMP_INC				= 0;			//	�ɸ��� �����ð��� MP ȸ����;
	float		fSP_INC				= 0;			//	�ɸ��� �����ð��� SP ȸ����;

	float		fRESIST_PHYSIC_G	= 0.5f;			//	���� ���ݽ� �Ӽ� ����ġ ���� ����.
	float		fRESIST_G			= 0.5f;			//	���� ���ݽ� �Ӽ� ����ġ ���� ����.
	float		fREGIST_MAX_G	= 0.99f;
	float		fRESIST_MAX_CROW		= 0.99f;
	WORD		wBASIC_DIS_SP		= 1;			//	�⺻ ���ݽ� �Ҹ� SP ��.

	float		fLOWSP_MOTION		= 0.20f;		//	���¹̳� (%) ���� ������ ��� ó��.
	float		fLOWSP_DAMAGE		= 0.50f;		//	���¹̳� ������ Damage ����.
	float		fLOWSP_HIT_DROP		= 0.25f;		//	���¹̳� ������ ������ ����.
	float		fLOWSP_AVOID_DROP	= 0.50f;		//	���¹̳� ������ ȸ���� ����.
	DWORD		dwACCEPT_LOWERSTATS	= 5;			//	stats �������� ���¹̳��� Ŀ���� �� �ִ� ���� �ѵ�. ( ���� ���⿡ ����. )

	float		fDAMAGE_DEC_RATE	= 40000.0f;		//	����� �����(�ݺ��)
	float		fDAMAGE_GRADE_K		= 10.0f;		//	(�⺻����+�������) ����� �ݿ���.(�ݺ��)
	float		fDAMAGE_DEC_MAX_G	= 0.95f;		// �ִ� ������ �����;

	DWORD		dwCRITICAL_DAMAGE	= 120;			//	ũ��Ƽ�� ������ ����
	DWORD		dwCRITICAL_MAX		= 40;			//	ũ��Ƽ�� Ȯ�� MAX

	DWORD		dwCRUSHING_BLOW_DAMAGE	= 150;		//  ����Ÿ�� ������ ����
	DWORD		dwCRUSHING_BLOW_MAX		= 20;		//  ����Ÿ�� Ȯ�� max
	float		fCRUSH_BLOW_RANGE		= 10.0f;	//  ����Ÿ�ݽ� �з����� �Ÿ� ( ������ 	

	DWORD		dwReleaseStigmaDistance	= 500;			// ���� ȿ�� ���� �Ÿ�(����Ʈ);

	SNATIVEID	sProductMotionID( AN_GATHERING, AN_SUB_NONE );
	FLOAT		fReferCharDelay( 10.0f );

	std::string	sInvenExItemSearch = "�κ��丮";
	WORD		wInvenDefaultLine = 7;
	WORD		wInvenExtendLine = EInvenTotalLine - wInvenDefaultLine - EInvenPremiumLine;
	WORD		wInvenExtendEventLine = 0;

	//hsshin ���۽ý��� ���õ���� �߰�
 	bool		bUnableProductType[Product::ETypeTotal] = { false, };
 	bool		bPublicStudyProductType[Product::ETypeTotal] = { false, };
 	DWORD		dwStudyPointDefault = 10;

	float		fTrash_SellCostPer = 0;		// ������ �̿�� 1���� (���ӸӴ�)
	DWORD		wTrash_Cost = 0;			// ������ �̿�� 2���� (���ӸӴ�)
	DWORD		wTrash_Point = 0;			// ������ �̿�� 3���� (�ΰ��� ĳ��)

	int			nBrightMAX = 250;			// ����Ʈ ���� ���� �ִ밪
	int			nBrightMIN = -250;			// ����Ʈ ���� ���� �ּҰ�

	float		fCONFRONT_TIME		= 180.0f;		//	��� ���� �ð�.
	float		fCONFRONT_ELAP		= 60.0f;		//	���� ��� ���� ���� �ð�.
	DWORD		dwCONFRONT_MAX		= 3;			//	���� ��� �� ���� Ƚ��.
	float		fCONFRONT_RESET		= 3600.0f;		//	���� ��� ���� ���� �ð�.
	float		fCONFRONT_ONE_DIST	= 0;			//	��� �Ÿ� ����. ( 0�� ��� ���Ѵ�. )
	float		fCONFRONT_PY_DIST	= 0;			//	��� �Ÿ� ����. ( 0�� ��� ���Ѵ�. )
	float		fCONFRONT_CLB_DIST	= 0;			//	��� �Ÿ� ����. ( 0�� ��� ���Ѵ�. )

	int			nCONFRONT_WIN_LP	= 1;			//	��� �¸��� living point ( ��Ȱ ���� ).
	int			nCONFRONT_LOSS_LP	= -1;			//	��� �й�� living point ( ��Ȱ ���� ).

	int			nCONFRONT_CLB_WIN_LP	= 5;
	int			nCONFRONT_CLB_LOSS_LP	= -5;

	WORD		wCONFRONT_SCHOOL_LIMIT_NUM		= 4;	//	�б��� ��� �ּ� �ο�.
	WORD		wCONFRONT_SCHOOL_LIMIT_LEVEL	= 20;	//	�б��� ��� �ּ� ����.
	float		fCONFRONT_SCHOOL_EXP_SCALE		= 1.5f;	//	�б��� ��ý� ���� ����ġ ������.

	WORD		wCONFRONT_SCHOOL_WIN_NUM		= 200;	//	�ְ� �¼� �б� ���� �ּ� �¼�.
	WORD		wCONFRONT_SCHOOL_WIN_RATE		= 30;	//	�ְ� �¼� �б� ���� �ּ� ����.
	float		fCONFRONT_SCHOOL_GENITEM		= 1.1f;	//	�ְ� �¼� �б����� ������ ��� ������.

    WORD		wCONFT_RC_TYPE[EMCONFT_RC_TYPENUM] = { 0, 4, 6, 8, 10, 12, 16, 20, 30, 0xffff };			//	��� ȸ���� ��밡�� Ƚ��.
	float		fCONFT_STATE_TYPE[EMCONFT_STATE_TYPENUM] = { 1.0f, 1.2f, 1.5f, 1.6f, 2.0f, 2.5f, 3.0f, 4.0f };	//	��� ü�� Ȯ�� ����.

	int			nSTATEBLOW_LEVEL[EMSTATEBLOW_LEVEL_SIZE] = { +10,  +8,  +6, +3, 0,  -2,  -4,  -6,  -8,  -10 };

	BOOL		bPK_MODE = TRUE;
	BOOL		bPKLESS = FALSE;
	int			nPK_LIMIT_LEVEL = 20;			//	pk ���� ����.
	int			nPK_LIMIT_DX_LEVEL = 10;		//	pk ������ ���� ����.
	int			nPK_TRY_BRIGHT_POINT = -1;		//	pk �õ��� ����Ʈ.
	int			nPK_KILL_BRIGHT_POINT = -9;		//	pk �׿��� ��� ����Ʈ.
	int			nPK_TRY_LIVING_POINT = -1;		//	pk �õ��� ����Ʈ.
	int			nPK_KILL_LIVING_POINT = -10;	//	pk �׿��� ��� ����Ʈ.
	float		fNONPK_ITEM_DROP = 0;			//	non pk �� ������ �����.
	float		fPK_ITEM_DROP = 0;				//	pk �� ������ �����.

	DWORD		dwPK_RECALL_ENABLE_LEVEL = 1;	//	��ȯ ��� ���� ���� ����.
	DWORD		dwPK_DRUG_ENABLE_LEVEL = 2;		//	ȸ���� ��� ���� ���� ����.

	float		fPK_JUSTNESS_TIME = 60.0f;		//	�ݰ� ������ ������� �ð�.
	float		fPK_SAFE_TIME = 10.0f;			//	pk ���� �ð�.
	
	//	CDM ���� �ð�.
	float		fCDM_SAFE_TIME_ACCRUE[CDM_ACCRUE_COUNT] =
	{
		60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 110.0f, 120.0f, 130.0f, 140.0f, 150.0f
	};

	float		fCDM_SAFE_TIME_MOVE_SPEED = 0.5f;	//	CDM���� ���� �ð� �̵��ӵ� ���ҷ� ( 0.0f ~ 1.0f )
	
	DWORD		dwPK_NORMAL_NAME_COLOR = 0xffffffff;	//	pk ��ġ�� ������ �̸� ����.
	float		fPK_POINT_DEC_RATE = 4.0f*3600.0f/GLPeriod::REALTIME_TO_VBR;	//	pk ��ġ ������. ( �ð���. )
	float		fPK_POINT_DEC_PHY = 0.5f;		//  pk ������ ������ ( ���� ).
	float		fPK_POINT_DEC_MAGIC = 0.5f;		//  pk ������ ������ ( ���� ).

	float		fReductionRate_PVP_PHY_P_Damage = 1.f;			// PVP ������ ( ���ݷ�, ����ġ );
	float		fReductionRate_PVP_PHY_S_Damage = 1.f;			// PVP ������ ( ���ݷ�, ���ġ );
	float		fReductionRate_PVP_MAGIC_Damage = 1.f;			// PVP ������ ( ���ݷ�, ���ġ );
	float		fReductionRate_PVP_PHY_Defense = 1.f;			// PVP ������ ( ���� );
	float		fReductionRate_PVP_MAGIC_Resistance = 1.f;		// PVP ������ ( ���� );


	SPLAYERKILL	sPK_STATE[EMPK_STATE_LEVEL] =
	{
		SPLAYERKILL(5,0xFFFF00,-20,"�Ϲ��л�",120.0f,1,10.0f,130.0f,90.0f),
		SPLAYERKILL(4,0xFFE400,-80,"�ҷ��л�",140.0f,2,10.0f,0.0f,0.0f),
		SPLAYERKILL(3,0xFFBC00,-160,"������",180.0f,3,30.0f,0.0f,0.0f),
		SPLAYERKILL(2,0xFF9700,-300,"���θ�",240.0f,4,60.0f,0.0f,0.0f),
		SPLAYERKILL(1,0xFF0000,-INT_MAX,"���α�",350.0f,5,90.0f,0.0f,0.0f),
	};

    int nNON_PK_CONNECT_BRIGHT_POINT = -20;

	// ����ġ Table
	WORD wEXPTABLE[EXPTABLE_SIZE][EXPTABLE_LVL] = 
	{
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 36, 44, 55, 70, 90, 116, 149, 190 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 37, 46, 58, 74, 95, 122, 156, 198 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 38, 48, 61, 78, 100, 128, 163, 206 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 39, 50, 64, 82, 105, 134, 170, 214 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 40, 52, 67, 86, 110, 140, 177, 222 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 41, 54, 70, 90, 115, 146, 184, 230 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 42, 56, 73, 94, 120, 152, 191, 238 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 31, 44, 59, 77, 99, 126, 159, 199, 247 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 16, 25, 32, 46, 62, 81, 104, 132, 166, 207, 256 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 16, 26, 33, 48, 65, 85, 109, 138, 173, 215, 265 },
		{ 0, 0, 1, 2, 2, 3, 5, 10, 17, 27, 34, 50, 68, 89, 114, 144, 180, 223, 274 },
		{ 0, 0, 1, 2, 2, 3, 5, 10, 17, 28, 35, 52, 71, 93, 119, 150, 187, 231, 283 },
		{ 0, 0, 1, 2, 2, 3, 5, 10, 18, 28, 36, 54, 74, 97, 124, 156, 194, 239, 292 },
		{ 0, 0, 1, 2, 2, 3, 5, 11, 18, 29, 37, 56, 77, 101, 129, 162, 201, 247, 301 },
		{ 0, 0, 1, 2, 3, 3, 5, 11, 19, 30, 38, 58, 80, 105, 134, 168, 208, 255, 310 },
		{ 0, 0, 1, 2, 3, 3, 5, 11, 19, 31, 39, 60, 83, 109, 139, 174, 215, 263, 319 },
		{ 0, 0, 1, 2, 3, 4, 6, 12, 20, 32, 40, 62, 86, 113, 144, 180, 222, 271, 328 },
		{ 0, 0, 1, 2, 3, 4, 6, 12, 20, 32, 41, 64, 89, 117, 149, 186, 229, 279, 337 },
		{ 0, 0, 1, 2, 3, 4, 6, 12, 21, 33, 42, 66, 92, 121, 154, 192, 236, 287, 346 },
		{ 0, 0, 1, 2, 3, 4, 6, 13, 22, 35, 44, 69, 96, 126, 160, 199, 244, 296, 356 },
		{ 0, 0, 1, 2, 3, 4, 6, 13, 23, 36, 46, 72, 100, 131, 166, 206, 252, 305, 366 },
		{ 0, 0, 1, 2, 3, 4, 7, 14, 24, 38, 48, 75, 104, 136, 172, 213, 260, 314, 376 },
		{ 0, 1, 2, 3, 4, 5, 7, 15, 25, 40, 50, 78, 108, 141, 178, 220, 268, 323, 386 },
		{ 0, 1, 2, 3, 4, 5, 7, 15, 26, 41, 52, 81, 112, 146, 184, 227, 276, 332, 396 },
		{ 0, 1, 2, 3, 4, 5, 8, 16, 27, 43, 54, 84, 116, 151, 190, 234, 284, 341, 406 },
		{ 0, 1, 2, 3, 4, 5, 8, 16, 28, 44, 56, 87, 120, 156, 196, 241, 292, 350, 416 },
		{ 0, 1, 2, 3, 4, 5, 8, 17, 29, 46, 58, 90, 124, 161, 202, 248, 300, 359, 426 },
		{ 0, 1, 2, 3, 4, 6, 9, 18, 30, 48, 60, 93, 128, 166, 208, 255, 308, 368, 436 },
		{ 0, 1, 2, 3, 4, 6, 9, 18, 31, 49, 62, 96, 132, 171, 214, 262, 316, 377, 446 },
		{ 0, 1, 2, 3, 5, 6, 9, 19, 32, 51, 64, 99, 136, 176, 220, 269, 324, 386, 456 },
		{ 0, 1, 2, 3, 5, 6, 9, 19, 33, 52, 66, 102, 140, 181, 226, 276, 332, 395, 466 },
		{ 0, 1, 2, 4, 5, 6, 10, 20, 34, 54, 68, 105, 144, 186, 232, 283, 340, 404, 476 },
		{ 0, 1, 2, 4, 5, 7, 10, 21, 35, 56, 71, 109, 149, 192, 239, 291, 349, 414, 487 },
		{ 0, 1, 2, 4, 5, 7, 11, 22, 37, 59, 74, 113, 154, 198, 246, 299, 358, 424, 498 },
		{ 0, 1, 3, 4, 6, 7, 11, 23, 38, 61, 77, 117, 159, 204, 253, 307, 367, 434, 509 },
		{ 0, 1, 3, 4, 6, 8, 12, 24, 40, 64, 80, 121, 164, 210, 260, 315, 376, 444, 520 },
		{ 0, 1, 3, 4, 6, 8, 12, 24, 41, 66, 83, 125, 169, 216, 267, 323, 385, 454, 531 },
		{ 0, 1, 3, 5, 6, 8, 12, 25, 43, 68, 86, 129, 174, 222, 274, 331, 394, 464, 542 },
		{ 0, 1, 3, 5, 7, 8, 13, 26, 44, 71, 89, 133, 179, 228, 281, 339, 403, 474, 553 },
		{ 0, 1, 3, 5, 7, 9, 13, 27, 46, 73, 92, 137, 184, 234, 288, 347, 412, 484, 564 },
		{ 0, 1, 3, 5, 7, 9, 14, 28, 47, 76, 95, 141, 189, 240, 295, 355, 421, 494, 575 },
		{ 0, 1, 3, 5, 7, 9, 14, 29, 49, 78, 98, 145, 194, 246, 302, 363, 430, 504, 586 },
		{ 1, 2, 4, 6, 8, 10, 15, 30, 50, 80, 101, 149, 199, 252, 309, 371, 439, 514, 597 },
		{ 1, 2, 4, 6, 8, 10, 15, 31, 52, 83, 104, 153, 204, 258, 316, 379, 448, 524, 608 },
		{ 1, 2, 4, 6, 8, 10, 16, 32, 54, 86, 108, 158, 210, 265, 324, 388, 458, 535, 620 },
		{ 1, 2, 4, 6, 8, 11, 16, 33, 56, 89, 112, 163, 216, 272, 332, 397, 468, 546, 632 },
		{ 1, 2, 4, 6, 9, 11, 17, 34, 58, 92, 116, 168, 222, 279, 340, 406, 478, 557, 644 },
		{ 1, 2, 4, 7, 9, 12, 18, 36, 60, 96, 120, 173, 228, 286, 348, 415, 488, 568, 656 },
		{ 1, 2, 4, 7, 9, 12, 18, 37, 62, 99, 124, 178, 234, 293, 356, 424, 498, 579, 668 },
		{ 1, 2, 5, 7, 10, 12, 19, 38, 64, 102, 128, 183, 240, 300, 364, 433, 508, 590, 680 },
		{ 1, 2, 5, 7, 10, 13, 19, 39, 66, 105, 132, 188, 246, 307, 372, 442, 518, 601, 692 },
		{ 1, 2, 5, 8, 10, 13, 20, 40, 68, 108, 136, 193, 252, 314, 380, 451, 528, 612, 704 },
	};

	float		fEXP_RATE_TABLE[EXPTABLE_RANGE] = 
	{
		//-30 -29   -28   -27   -26   -25   -24   -23   -22   -21
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		//-20 -19   -18   -17   -16   -15   -14   -13   -12   -11   -10   -9    -8    -7    -6    -5    -4    -3    -2    -1    0
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		//+1  +2   +3     +4    +5    +6    +7    +8    +9    +10   +11   +12   +13   +14   +15   +16   +17   +18   +19   +20
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		//+21 +22   +23   +24   +25   +26   +27   +28   +29   +30
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	LONGLONG	lnEXP_MAX_TABLE[MAX_LEVEL];
	LONGLONG	lnEXP_MAX_TABLE_2nd[MAX_LEVEL];
	DWORD		dwEXP_PC_TABLE[MAX_LEVEL];

	float		fDPS_TurnToPeaceModeInterval	= 8.0f; // ������ ���·� ���� ��� �ð�(DPS ���� �ð� ����);

	float		fPARTYEXPRANGE		= 80.0f;			//	����ġ ������ ������ �ִ� �Ÿ�.
	float		fPARTYEXP_S			= 0.006f;			//	��Ƽ ����ġ �й�� �������̿� ���� ���Һ���.
	float		fPartyTenderTime						= 30.0f;
	float		fExpeditionTenderTime				= 60.0f;
	float		fUPartyTenderTime					= (1.0f / 30.0f);
	float		fUExpeditionTenderTime				= (1.0f / 60.0f);
	float		fPartyTenderRange					= 140.0f;
	float		fExpeditionTenderRange				= 140.0;	
	float		fExpeditionDecreaseExpRate		= (1.0f / 40.0f);
	float		fSwapSlotBlankInterval				= 0.7f;

	//																		  1		 2		3	   4	  5		 6	   7	  8   
	WORD		aPARTYEXP_ONE[MAXPARTY]			= {   100, 100, 100, 100, 100, 100, 100, 100 };
	WORD		aPARTYKILLEXP_ONE[MAXPARTY]	= {   100, 100, 100, 100, 100, 100, 100, 100 };

	DWORD		dwPARTY_INVITE_MAX_WAIT_TIME	= 10 * 1000; // 10��; ��Ƽ �ʴ�� ���� ��� �ð�; 
	DWORD		dwExpeditionCheckReadyTime			= 1500;

	DWORD		dwCLUB_PARTYNUM				= 5;		//	Ŭ�� â���� �ּ� ��Ƽ(�ʱ�) �ο�.
	DWORD		dwCLUB_DISSOLUTION_DAY	= 7;		//	Ŭ�� ��ü ���� ��.
	DWORD		dwCLUB_JOINBLOCK_DAY		= 5;		//	Ŭ�� Ż��� ���� ���� ��.
	DWORD		dwCLUB_AUTHORITY				= 24;		//	Ŭ�������� ���� ���� �ð�			
	DWORD		dwCLUB_ALLIANCE_SEC			= 24;		//	Ŭ�� ���� Ż��, ����� �簡�� ���ѽð�.
	DWORD		dwCLUB_ALLIANCE_DIS			= 24;		//	Ŭ�� ���� ��ü�� �Ἲ ���ѽð�.
	DWORD		dwCLUB_ALLIANCE_NUM			= 3;		//	Ŭ�� ���� �ִ� ����.

    BOOL		bCLUB_BATTLE				= FALSE;	//	Ŭ�� ��Ʋ ���� ����
	DWORD		dwCLUB_BATTLE_RANK			= 1;		//	Ŭ�� ��ũ Ȯ��
	DWORD		dwCLUB_BATTLE_MAX			= 5;		//  Ŭ�� ��Ʋ ���� ���� ����
	DWORD		dwCLUB_BATTLE_TIME			= 20;		//  Ŭ�� ��Ʋ �ð�(��) - �Է� ������� ������
	DWORD		dwCLUB_BATTLE_TIMEMIN		= 10;		//	Ŭ�� ��Ʋ �ּҽð�(��)
	DWORD		dwCLUB_BATTLE_TIMEMAX		= 1200;		//	Ŭ�� ��Ʋ �ִ�ð�(��)
	DWORD		dwCLUB_BATTLE_GUID_TIME		= 24;		//  Ŭ�� ��Ʋ ������ ���� �ð�(��)
	DWORD		dwCLUB_BATTLE_DIS_TIME		= 1;		//  Ŭ�� ��Ʋ ���� �� �׺� ������ �ð�(��)
	BOOL		bCLUB_BATTLE_ALLIANCE		= FALSE;	//  ���� Ŭ�� ���� ����

	BOOL		bCLUB_DEATHMATCH			= FALSE;	//	Ŭ�� ������ġ ���� ����
	DWORD		dwCLUB_DEATHMATCH_MEM		= 7;		//	Ŭ�� ������ġ ���� �����
	DWORD		dwCLUB_DEATHMATCH_RANK		= 4;		//	Ŭ�� ������ġ ���� ��ũ
    DWORD       dwCLUB_CHANGE_CLUB_MARK_LIMIT_TIME = 60; // Ŭ�� ��ũ ���� ���� �ð�(��).

	// Ŭ�� ��ũ RGB;
	DWORD		dwClubMarkARGB				= D3DCOLOR_ARGB( 255, 255, 255, 255 );
	std::vector< DWORD > vecClubMarkARGB;

	DWORD		dwMAX_CLUBRANK				= 5;		//  �ִ� Ŭ�� ��ũ

	GLCLUBRANK	sCLUBRANK[MAX_CLUBRANK]		=			//	Ŭ�� ��ũ�� ���.
	{
		GLCLUBRANK(30,0,1000000,15),
		GLCLUBRANK(45,80,2000000,20),
		GLCLUBRANK(60,100,3500000,30),
		GLCLUBRANK(80,140,5000000,40),
		GLCLUBRANK(100,180,7000000,50),
		GLCLUBRANK(120,220,10000000,60),
		GLCLUBRANK(130,230,20000000,70),
		GLCLUBRANK(140,240,30000000,80),
		GLCLUBRANK(150,250,40000000,90),
		GLCLUBRANK(160,260,50000000,100),
	};

	float fMAX_COMMISSION				= 50;		//	�ִ� ���� ���� ������ ����.
	float fDEFAULT_COMMISSION			= 5;		//	�� ���� ������ ������.
	float fEARNING_RATE					= 0.2f;		//	Ŭ�� ������ ������.
	float fEARNING_RATE_NPC				= 0.2f;		//	NPC ȣ�� ������ ������ 

	float fCDCERTIFY_DIST				= 30;		//	���� ���� �Ÿ�.
	float fCDCERTIFY_DIST2				= 5;		//	���� ���� �Ÿ�.
	float fCDCERTIFY_TIME				= 60;		//	���� �ҿ� �ð�.

	float fEXP_SCALE					= 1.0f;		//	Note : ����ġ ����.
    float fITEM_DROP_SCALE				= 1.0f;		//	Note : ������ �����.
    float fITEM_DROP_LOWER_SCALE		= 1.0f;		//	Note : ��� ������ �����.
	float fMONEY_DROP_SCALE				= 1.0f;		//	Note : �� �����.

	float fEVENT_EXP_SCALE				= 1.0f;		//	Note : ����ġ ����.
	float fEVENT_ITEM_DROP_SCALE		= 1.0f;		//	Note : ������ �����.
	float fEVENT_MONEY_DROP_SCALE		= 1.0f;		//	Note : �� �����.

    float fITEM_DROP_SPECIFIC_GRADE_SCALE = 1.0f;   //  Note : Ư�� ������ ��� �����.(����� genitem���� ����)
    WORD  wITEM_DROP_SPECIFIC_GRADE       = 0;      //  Note : ������� ������ų Ư�� �������� ���

	//	Note : ----------------------------------------------------------------
	bool bSCHOOL_FREE_PK_ALWAYS = false;	//	�п��� ���� pk �׻� ���� ����.
	bool bSCHOOL_FREE_PK = false;			//	�п��� ���� pk on/off.
	bool bSCHOOL_FREE_PK_TIME_REAL = false;	//	�п��� ���� pk ���� �ð� ( on : ���� �ð�, off : ���ӽð� )
	bool bSCHOOL_FREE_PK_Z_FORCED = true;	//	�п��� ���� pk ���� ����.
	DWORD dwSCHOOL_FREE_PK_TIME_START = 21;	//	�п��� ���� pk ���� �ð�.
	DWORD dwSCHOOL_FREE_PK_TIME_TERM = 7;	//	�п��� ���� pk ���� �ð�.
	float fSCHOOL_FREE_PK_ITEM_DROP = 3;	//	�п��� ���� pk ������ ����� ���갪. ( �⺻ ����� + ���갪. )
	//	Note : ----------------------------------------------------------------

	//bool bENCHANT_TERMINATE_ITEM = false;	//	��þƮ�� ������ �ļ� ����.

	//	Note : ----------------------------------------------------------------��� ����.
	bool bMONEY_DROP2FIELD = true;			//	�� �ٴڿ� ������ ���� ����.

	//	Note : ----------------------------------------------------------------���� Ȯ��.
	//													  1 -15
	//float fGRADE_RATE[GRADE_LIMIT_MAX]								= {    100,    100,    100,    100, 33.33f, 
	//																	25.00f, 20.00f, 16.67f, 15.00f, 15.00f,
	//																	15.00f, 15.00f, 15.00f, 15.00f, 15.00f };
	//float fGRADE_RESET_RATE[GRADE_LIMIT_MAX]						= {      0,      0,      0,      0, 10.00f, 
	//																	20.00f, 30.00f, 40.00f, 50.00f, 50.00f, 
	//																	50.00f, 50.00f, 50.00f, 50.00f, 50.00f };
	//float fGRADE_TERMINATE_RATE[GRADE_LIMIT_MAX]					= {      0,      0,      0,      0, 40.00f, 
	//																	50.00f, 60.00f, 70.00f, 70.00f, 70.00f,
	//																	70.00f,	70.00f,	70.00f,	70.00f,	70.00f };
	//float fGRADE_ITEMOPTION_INC_GRINDINGRATE[GRADE_LIMIT_MAX]		= {    100,    100,    100,    100, 33.33f, 
	//																	25.00f, 20.00f, 16.67f, 15.00f, 15.00f,
	//																	15.00f, 15.00f, 15.00f, 15.00f, 15.00f };
	//int   nGRADE_ITEMOPTION_ANTIRESET[GRADE_LIMIT_MAX]				= { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	float fGRADE_EVENT_RATE = 0.0f;


	//	Note : ---------------------------------------------------------------- ä�� ����
	bool	bCHAT_EXTEND = false;					// ����, ��Ƽ���� Ȱ��ȭ ����
	float	dwCHAT_AREA_CHARGE = 0;					// ���� ä�� ���
	float	dwCHAT_PARTY_RECRUIT_CHARGE = 0;		// ��Ƽ ���� ���
	float	fCHAT_PARTY_RECRUIT_TIME = 300.0f;		// ��Ƽ ���� ���� �ð�

	//	Note : ----------------------------------------------------------------���� ����.
	std::string	strGRINDING_SUCCEED			= "";
	std::string	strGRINDING_FAIL			= "";
	std::string	strGRINDING_RESET			= "";
	std::string	strGRINDING_BROKEN			= "";

	std::string	strGAMBLING_SHUFFLE			= "";		// Monster7j
	std::string	strGAMBLING_WIN				= "";
	std::string	strGAMBLING_LOSE			= "";
	std::string	strDICE_ROLL					= "";

	std::string	strITEMDROP_SUIT			= "";
	std::string	strITEMDROP_WAPON			= "";
	std::string	strITEMDROP_SHOES			= "";
	std::string	strITEMDROP_RING			= "";
	std::string	strITEMDROP_QBOX			= "";

	std::string	strITEMDROP_SCROLL			= "";
	std::string	strITEMDROP_COIN			= "";
	std::string	strITEMDROP_DRUGS			= "";

	std::string	strPICKUP_ITEM				= "";
	std::string	strQITEM_FACT				= "";
	std::string	strQITEM_BGM[QUESTION_SIZE];
	std::string	strITEMDROP_RANK[EMITEM_LEVEL_NSIZE];

	std::string	strSELECT_CHAR				= "";

	std::string	strREBIRTH_EFFECT			= "";
	std::string	strLEVELUP_EFFECT			= "";
	std::string	strSRIKE_EFFECT				= "";
	std::string	strSRIKE_ELMT_EFFECT[EMELEMENT_MAXNUM]	= { "", "", "", "", "", "", "", "", "" };
	std::string	strSTRIKE_ARROW_EFFECT		= "";
	std::string	strSTRIKE_BULLET_EFFECT		= "";

	std::string	strAMBIENT_EFFECT			= "";
	std::string	strERASE_EFFECT				= "";

	std::string	strERASE_SUMMON_EFFECT		= "";   // ��ȯ���� ���� ����Ʈ
	float		fERASE_SUMMON_BEGIN_TIME	= 1.0f; // ��ȯ�� �������� �����ϴ� �ð�(��) ( EndTime - fERASE_SUMMON_BEGIN_TIME ���� )

	std::string	strSKILL_LEARN_EFFECT		= "";
	std::string	strSKILL_UP_EFFECT			= "";

	std::string strQUEST_START				= "";
	std::string strQUEST_END_SUCCEED		= "";
	std::string strQUEST_END_FAIL			= "";

	std::string	strMOB_GEM_EFFECT			= "";
	std::string	strMOB_DIE_BLOOD			= "";

	// PET
	std::string	strPET_GEN_EFFECT           = "";
	
	// vehicle
	std::string strVEHICLE_GEN_EFFECT		= "";

	std::string	strCONFRONT_BOUND			= "";
	std::string	strHALFALPHA_EFFECT			= "";

	std::string	strBLOW_NUMB_EFFECT			= "";
	std::string	strBLOW_STUN_EFFECT			= "";
	std::string	strBLOW_STONE_EFFECT		= "";
	std::string	strBLOW_BURN_EFFECT			= "";
	std::string	strBLOW_FROZEN_EFFECT		= "";

	std::string	strBLOW_MAD_EFFECT			= "";
	std::string	strBLOW_POISON_EFFECT		= "";
	std::string	strBLOW_CURSE_EFFECT		= "";

	std::string	strBLOW_BODY_NUMB_EFFECT	= "";
	std::string	strBLOW_BODY_STUN_EFFECT	= "";
	std::string	strBLOW_BODY_STONE_EFFECT	= "";
	std::string	strBLOW_BODY_BURN_EFFECT	= "";
	std::string	strBLOW_BODY_FROZEN_EFFECT	= "";

	std::string	strBLOW_BODY_MAD_EFFECT		= "";
	std::string	strBLOW_BODY_POISON_EFFECT	= "";
	std::string	strBLOW_BODY_CURSE_EFFECT	= "";

	std::string	strCOMPLETE_NPC_EFFECT		= ""; // �Ϸ� ����Ʈ ����Ʈ

	std::string strCLICK_EFFECT				= ""; // �̵� ���� ��ġ ǥ�� ����Ʈ
	std::string strMINE_EFFECT				= ""; // �ڱ� �ڽ��� ���������� ������ ����Ʈ
	std::string strCURSOR_TARGET_EFFECT		= ""; // Ŀ�� ���� ��� ����Ʈ
	std::string strTARGET_EFFECT			= ""; // ���� ��� ����Ʈ
	std::string strTARGET_EFFECT_ENEMY		= ""; // ���� ��� ����Ʈ(��)
	std::string strTARGET_EFFECT_NPC		= ""; // ���� ��� ����Ʈ(npc)
	std::string strTARGET_EFFECT_ALLIANCE	= ""; // ���� ��� ����Ʈ(�Ʊ�)
	std::string strARROW_EFFECT_ENEMY		= ""; // ȭ��ǥ ����Ʈ(��)
	std::string strARROW_EFFECT_NPC			= ""; // ȭ��ǥ ����Ʈ(npc)
	std::string strARROW_EFFECT_ALLIANCE	= ""; // ȭ��ǥ ����Ʈ(�Ʊ�)
	std::string strSCOPE_TEXTURE_CIRCLE		= ""; // ���� ����Ʈ (��)
	std::string strSCOPE_TEXTURE_FANWISE	= ""; // ���� ����Ʈ (��ä��)
    std::string strCOMPLATE_ACTIVITY_EFFECT = "";
	std::string strBattleIcon				= "";
	SNATIVEID	sBattleIconIndex;				// ������� ������ �ε���.

	std::string	strBLOW_EFFECTS[EMBLOW_SIZE];
	std::string	strBLOW_BODY_EFFECTS[EMBLOW_SIZE];

    std::string strCRUSHING_BLOW_EFFECT;								// ����Ÿ�� ����Ʈ
    std::string strAUTH_EFFECT    			= "account_success.egp";	// ���� ���� ���� ����Ʈ
    std::string strAUTH_SM_EFFECT 			= "account_sacred.egp";		// �����п� ���� ����Ʈ
    std::string strAUTH_HA_EFFECT 			= "account_mp.egp";			// �����п� ���� ����Ʈ
    std::string strAUTH_BH_EFFECT 			= "account_phoenix.egp";	// ��Ȳ�п� ���� ����Ʈ

	std::string strCA_EFFECT				= "";

	BOOL		bCOMBAT_POINT				= FALSE;//	CP ��� ����
	WORD		wPK_KILL_Combat_Point		= 10;	//	CP �߰� ����Ʈ
	float		fIllusionInterval			= 3.0f;	//	

    // ������ - 2013-04-17 cwBack
    // �������� ����� ����� ��ġ		 	( Ư�� ���(EMSUIT) ������ ������ �ý����� ���� ���� 0=������,1=����� )
    bool  bDurabilityEmsuit[SUIT_NSIZE]     = {
		1,1,1,1,1,
		1,1,1,0,0,
		0,0,0,0,0,
		0,0,0,0,1,
		1,1,1}; 
    //--- ī���� �� �ʿ� �ٷ� ������ ���� ---
    WORD wDurabilityDropDeathPvE    = 25;       // ����ϸ� �������� �����մϴ�. (��)
    WORD wDurabilityDropDeathPvP    = 1;        // ����ϸ� �������� �����մϴ�. (���)
    //--- ī���� �ʿ� ---
    DWORD dwDurabilityCountLimit    = 10000000; // �������� ī���ü��� �����ϸ� 1 �����մϴ�. (ex ī���� 1000�� == ������ 1���� )
    DWORD dwDurabilityDropSkillUse  = 200000;   // ��ų�� ����ϸ� �����ϴ� ī��Ʈ
    DWORD dwDurabilityDropHitPvE    = 20;       // ����� 1Point�� �����ϴ� ī��Ʈ (��)
    DWORD dwDurabilityDropHitPvP    = 2;        // ����� 1Point�� �����ϴ� ī��Ʈ (���)

    int   nDurabilityRateTable[DURABILITY_RATE_COUNT] = 
    {	100,	85,	70,	55,	40,	25,	10,	0	};	                // ������ ������ ���� ������ ������
    int   nDurabilityRatePerformance[DURABILITY_RATE_COUNT]	= 
    { 	100,	98,	95,	90,	85,	75,	70,	60	};	                // ������ ���ҿ� ���� ������ �ɷ�ġ ����( ������(0%)=����(40%) )
	int   nWPowDurRatePerformance[DURABILITY_RATE_COUNT]	= 
	{ 	100,	98,	95,	90,	85,	75,	70,	60	};	                // �� �迭
	int   nWDexDurRatePerformance[DURABILITY_RATE_COUNT]	= 
	{ 	100,	98,	95,	90,	85,	75,	70,	60	};	                // ��ø �迭
	int   nWSpiDurRatePerformance[DURABILITY_RATE_COUNT]	= 
	{ 	100,	98,	95,	90,	85,	75,	70,	60	};	                // ���� �迭
	int   nWMixDurRatePerformance[DURABILITY_RATE_COUNT]	= 
	{ 	100,	98,	95,	90,	85,	75,	70,	60	};	                // ȥ�� 2�� �̻� �迭
	// �ɷ�ġ �׽�Ʈ��
//#ifdef _RELEASED
	// �ɷ�ġ �׷캰 on/off
	BYTE   nDurabilityApply[DURABILITY_APPLY_COUNT] =
	{   1, 1, 1, 1, 1, 1  };
	float	fDurabilityAlarmRemainPer = 0.2f;							// ���� ���� ���Ͻ� ���޴��� �˸������� �ν�Ʈ���� ����, ���޴����� �˸���;
//#endif

	bool bAdditional_ability = FALSE;

	GLCONST_CHARCLASS	cCONSTCLASS[GLCI_NUM_ACTOR] = 
	{
		//	FighterM
		GLCONST_CHARCLASS ( 10.0f, 0.9f, 1.00f, 0.08f,
							0.08f, 0.4f, 0.4f, 0.2f,
							SCHARSTATS(15,13,20,10,0,20), FCHARSTATS(2,3,1,1,1,3),
							35, 15, 5, 4,
							2, 2, 3, 1 ),
		//	ArmsM
		GLCONST_CHARCLASS ( 12.0f, 0.8f, 1.00f, 0.07f,
							0.07f, 0.5f, 0.5f, 0.1f,
							SCHARSTATS(20,15,10,8,0,15), FCHARSTATS(3,2,1,1,1,2),
							40, 25, 5, 3,
							1, 3, 2, 1 ),
		//	ArcherW
		GLCONST_CHARCLASS ( 9.0f, 1.0f, 1.00f, 0.09f,
							0.09f, 0.4f, 0.4f, 0.3f,
							SCHARSTATS(10,11,15,15,0,10), FCHARSTATS(2,1,1,1,1,1),
							25, 20, 3, 7,
							2, 2, 1, 3 ),
		//	SpiritW
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
		//	ExtremeM
		GLCONST_CHARCLASS ( 1.7f, 0.84f, 1.1f, 0,
							0, 0.1f, 0.22f, 0.1f,
							SCHARSTATS(7,34,18,10,0,7), FCHARSTATS(0.3f,4.3f,0.66f,0.4f,0,2.7f),
							4, 5, 4, 3,
							0.9f, 0.1f, 0.4f, 0.3f ),
		//	ExtremeW
		GLCONST_CHARCLASS ( 1.7f, 0.8f, 1.1f, 0,
							0, 0.1f, 0.22f, 0.1f,
							SCHARSTATS(7,34,18,10,0,7), FCHARSTATS(0.3f,4.3f,0.66f,0.38f,0,2.7f),
							4, 5, 4, 3,
							1.0f, 0.1f, 0.4f, 0.2f ),
		//	FighterW
		GLCONST_CHARCLASS ( 10.0f, 0.9f, 1.00f, 0.08f,
							0.08f, 0.4f, 0.4f, 0.2f,
							SCHARSTATS(15,13,20,10,0,20), FCHARSTATS(2,3,1,1,1,3),
							35, 15, 5, 4,
							2, 2, 3, 1 ),
		//	ArmsW
		GLCONST_CHARCLASS ( 12.0f, 0.8f, 1.00f, 0.07f,
							0.07f, 0.5f, 0.5f, 0.1f,
							SCHARSTATS(20,15,10,8,0,15), FCHARSTATS(3,2,1,1,1,2),
							40, 25, 5, 3,
							1, 3, 2, 1 ),
		//	ArcherM
		GLCONST_CHARCLASS ( 9.0f, 1.0f, 1.00f, 0.09f,
							0.09f, 0.4f, 0.4f, 0.3f,
							SCHARSTATS(10,11,15,15,0,10), FCHARSTATS(2,1,1,1,1,1),
							25, 20, 3, 7,
							2, 2, 1, 3 ),
		//	SpiritM
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
		//	ScientistM
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
		//	ScientistW
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),

        //	AssassinM
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
		//	AssassinW
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),

		//	TrickerM
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
		//	TrickerW
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),

        //	EtcM
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
		//	EtcW
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
		//	ActorM
		GLCONST_CHARCLASS ( 1.7f, 0.84f, 1.1f, 0,
							0, 0.1f, 0.22f, 0.1f,
							SCHARSTATS(7,34,18,10,0,7), FCHARSTATS(0.3f,4.3f,0.66f,0.4f,0,2.7f),
							4, 5, 4, 3,
							0.9f, 0.1f, 0.4f, 0.3f ),
		//	ActorW
		GLCONST_CHARCLASS ( 1.7f, 0.8f, 1.1f, 0,
							0, 0.1f, 0.22f, 0.1f,
							SCHARSTATS(7,34,18,10,0,7), FCHARSTATS(0.3f,4.3f,0.66f,0.38f,0,2.7f),
							4, 5, 4, 3,
							1.0f, 0.1f, 0.4f, 0.2f ),

	};

	std::vector<std::string>	vecGUIDANCE_FILE;
	std::vector<std::string>	vecClubDM_FILE;
	std::vector<DWORD>			vecCTFNotify;
	float fCTF_BECOME_AWAY_TIME; // CTF ���� �Է��� ���� �� �ڸ���� ���� �ɸ��� �ð�(��).

	float fPremiumMapExitCheckTime;							// �����̾� �� �������� üũ �ð� (��)
	std::vector< DWORD > vecPremiumMapForcedExitNotify;		// �����̾� �� �������� ���� �ð� �˸�

	LONGLONG NotifyMoney = 100000000;
	int nDecomposeCost = 10000;
	float fQBoxPickableTime = 5.0f;
	float fMacro_TermsofClose_LimitTime = 30.0f; // �����ð��� ���� ��ũ�� ��������;
	int nMacro_TermsofClose_Potion = 5; // ���� ���࿡ ���� ��ũ�� ��������;
	float fMacro_MaxFillTime = 21600; // �������� ä����ִ� ��ũ�� �ð�(����-��);

	char szCharSkin[GLCI_NUM_ACTOR][MAX_PATH] =
	{
				"o_m.chf",		//	FighterM
				"o_m.chf",		//	ArmsM
				"o_w.chf",		//	ArcherW
				"o_w.chf",		//	SpiritW
				"o_m.chf",		//	Extreme M
				"o_w.chf",		//	Extreme W
				"o_w.chf",		//	FighterW
				"o_w.chf",		//	ArmsW
				"o_m.chf",		//	ArcherM
				"o_m.chf",		//	SpiritM
				"o_m.chf",		//	ScientistM
				"o_w.chf",		//	ScientistW
                "o_m.chf",		//	AssassinM
                "o_w.chf",		//	AssassinW
				"o_m.chf",		//	TrickerM
				"o_w.chf",		//	TrickerW
                "o_m.chf",		//	EtcM
                "o_w.chf",		//	EtcW
				"o_m.chf",		//	ActorM
				"o_w.chf",		//	ActorW
	};


	char szManSimpleSkin[EM_SIMPLESKIN_NUM][MAX_PATH] =
	{
		"m_cos_min_cap.cps",		//	CAP
		"m_cos_min_body.cps",		//	BODY
		"m_cos_min_leg.cps",		//	LEG
		"m_cos_min_foot.cps",		//	FOOT

	};

	char szWomenSimpleSkin[EM_SIMPLESKIN_NUM][MAX_PATH] =
	{
		"w_cos_min_cap.cps",		//	CAP
		"w_cos_min_body.cps",		//	BODY
		"w_cos_min_leg.cps",		//	LEG
		"w_cos_min_foot.cps",		//	FOOT

	};

    std::string strCharBoard[GLCI_NUM_ACTOR] = // Note : DxEffBillboardChar.h�� enum EMBOARD_TYPE�� �ڵ嵵 �����ؾ��Ѵ�.
	{
		"M_FIGHT_BOARD.dds",	//	FIGHT MAN
		"M_ARMS_BOARD.dds",		//	ARMS MAN
		"W_ARCHER_BOARD.dds",		//	ARCHER WOMEN
		"W_SPIRIT_BOARD.dds",		//	SPIRIT WOMEN

		"M_EXTREME_BOARD.dds",		//	EXTREME MAN
		"W_EXTREME_BOARD.dds",		//	EXTREME WOMEN

		"W_FIGHT_BOARD.dds",	//	FIGHT WOMEN
		"W_ARMS_BOARD.dds",		//	ARMS WOMEN
		"M_ARCHER_BOARD.dds",		//	ARCHER MAN
		"M_SPIRIT_BOARD.dds",		//	SPIRIT MAN
		"M_SCIENTIST_BOARD.dds",	//	SCIENTIST MAN
		"W_SCIENTIST_BOARD.dds",	//	SCIENTIST WOMEN
        "M_ASSASSIN_BOARD.dds",	//	ASSASSIN MAN
        "W_ASSASSIN_BOARD.dds",	//	ASSASSIN WOMEN
		"m_tricker_board.dds",	//	TRICKER MAN
		"w_tricker_board.dds",	//	TRICKER WOMEN
        "M_ETC_BOARD.dds",	//	ETC MAN
        "W_ETC_BOARD.dds",	//	ETC WOMEN
		"M_EXTREME_BOARD.dds",	//	Actor MAN
		"W_EXTREME_BOARD.dds",	//	Actor WOMEN
	};


	std::string strCLASS_INIT[MAX_SCHOOL][GLCI_NUM_ACTOR] =
	{
		{ "class00.charset", "class01.charset", "class02.charset", "class03.charset", "class04.charset", "class05.charset", "class06.charset", "class07.charset", "class08.charset", "class09.charset", "class0A.charset", "class0B.charset", "class0C.charset", "class0D.charset", "class0E.charset", "class0F.charset", "class0G.charset", "class0H.charset", "class0I.charset", "class0J.charset" },
		{ "class10.charset", "class11.charset", "class12.charset", "class13.charset", "class14.charset", "class15.charset", "class16.charset", "class17.charset", "class18.charset", "class19.charset", "class1A.charset", "class1B.charset", "class1C.charset", "class1D.charset", "class1E.charset", "class1F.charset", "class1G.charset", "class1H.charset", "class1I.charset", "class1J.charset" },
		{ "class20.charset", "class21.charset", "class22.charset", "class23.charset", "class24.charset", "class25.charset", "class26.charset", "class27.charset", "class28.charset", "class29.charset", "class2A.charset", "class2B.charset", "class2C.charset", "class2D.charset", "class2E.charset", "class2F.charset", "class2G.charset", "class2H.charset", "class2I.charset", "class2J.charset" },
	};

	std::string strJUMPING_CLASS_INIT[MAX_SCHOOL][GLCI_NUM_ACTOR] =
	{
		{ "jumpingclass00.charset", "jumpingclass01.charset", "jumpingclass02.charset", "jumpingclass03.charset", "jumpingclass04.charset", "jumpingclass05.charset", "jumpingclass06.charset", "jumpingclass07.charset", "jumpingclass08.charset", "jumpingclass09.charset", "jumpingclass0A.charset", "jumpingclass0B.charset", "jumpingclass0C.charset", "jumpingclass0D.charset", "jumpingclass0E.charset", "jumpingclass0F.charset", "jumpingclass0G.charset", "jumpingclass0H.charset", "jumpingclass0I.charset", "jumpingclass0J.charset" },
		{ "jumpingclass10.charset", "jumpingclass11.charset", "jumpingclass12.charset", "jumpingclass13.charset", "jumpingclass14.charset", "jumpingclass15.charset", "jumpingclass16.charset", "jumpingclass17.charset", "jumpingclass18.charset", "jumpingclass19.charset", "jumpingclass1A.charset", "jumpingclass1B.charset", "jumpingclass1C.charset", "jumpingclass1D.charset", "jumpingclass1E.charset", "jumpingclass1F.charset", "jumpingclass1G.charset", "jumpingclass1H.charset", "jumpingclass1I.charset", "jumpingclass1J.charset"  },
		{ "jumpingclass20.charset", "jumpingclass21.charset", "jumpingclass22.charset", "jumpingclass23.charset", "jumpingclass24.charset", "jumpingclass25.charset", "jumpingclass26.charset", "jumpingclass27.charset", "jumpingclass28.charset", "jumpingclass29.charset", "jumpingclass2A.charset", "jumpingclass2B.charset", "jumpingclass2C.charset", "jumpingclass2D.charset", "jumpingclass2E.charset", "jumpingclass2F.charset", "jumpingclass2G.charset", "jumpingclass2H.charset", "jumpingclass2I.charset", "jumpingclass2J.charset"  },
	};

    std::string strIPEventFileName = "IPEvent.luc";

	std::string strQuestGroupFileName = "QuestMapping.lua";

	std::string strScreenEffect[SCREEN_EFFECT_NUM] =
	{
		"EFFECT_BROKEN_GLASS",
		"EFFECT_WATER",
		"EFFECT_DUST" ,
	};

	std::string strCameraEffect[CAMERA_EFFECT_NUM] = 
	{
		"",
		"",
		"",
		"",
		"",
	};

	std::string strItemLevelIcon[EMITEMLEVEL_NUM] =
	{
		"ITEM_LEVEL_NORMAL",
		"ITEM_LEVEL_HIGH",
		"ITEM_LEVEL_RARE",
		"ITEM_LEVEL_UNIQUE",
		"ITEM_LEVEL_EPIC",
		"ITEM_LEVEL_LEGENDARY",
	};

	std::string strOPItemIcon = "";
	std::vector< SSTATPOINT_BYLEVEL > vecMaxStatPoint;
	std::vector< SSTATPOINT_BYLEVEL > vecExtremeMaxStatPoint;
	std::vector<INT> vecMacroRemainTimeAlarm;

	float fIgnore_BossDamageMax = 0.2f;
	int nBossFixedDamageMax = 2000;
	float	fIncRCriticalMax_EmergencyMob = 0.15f;
	float	fIncRCriticalMax_EmergencyBoss = 0.15f;
	float	fDecRDamageMax_EmergencyState = 0.3f;

	float	fEmergency_CharHPR = 0.2f;
	float	fEmergency_MobHPR = 0.2f;
	float	fEmergency_BossHPR = 0.2f;
	int		nEmergency_BlinkCount = 2;
	float	fIncRExpAddEffectMax = 0.1f;
	float	fIncRDropGenMoneyMax = 0.1f;
	float	fDecRDamageMax_MeleeSkill = 0.1f;
	float	fDecRDamageMax_RangeSkill = 0.1f;
	float	fDecRDamageMax_MagicSkill = 0.1f;
	int		nInc_HPMax_SoloPlay = 300;
	int		nInc_HPMax_PartyPlay = 300;
	float	fIncR_AtkSpeedMax_SoloPlay = 0.1f;	
	float	fIncR_AtkSpeedMax_PartyPlay = 0.1f;
	float	fInc_RandomOp_SkillDurationAddTimeMax = 2.0f;
	float	fIncR_RandomOp_SkillDamageAddMax = 0.3f;
	float	fDecR_SkillDelayTMax = 3.0f;
	float	fIncR_CriticalDamageMax = 0.3f;
	float	fIncR_CrushingBlowMax = 0.3f;
	float	fIncR_LifePerKillMax = 0.01f;
	float	fIncR_MPPerKillMax = 0.02f;
	float	fIncR_SPPerKillMax	= 0.02f;
	float	fIncR_HPMPSPPerKillMax = 0.01f;		
	float	fIncR_AllMPRatioToAttackMax = 0.2f;
	float	fIncR_AllSPRatioToHpMax = 0.3f;	
	float	fIncR_AllHitRatioToMeleeMax = 0.3f;
	float	fIncR_AllHitRatioToRangeMax = 0.3f;
	float	fIncR_AllHitRatioToMagicMax = 0.3f;

    void SET_EXCESS_EXP_TYPE( BYTE expType )
    {
        ExcessExpProcessType = expType;
    }

    BYTE GET_EXCESS_EXP_TYPE()
    {
        return ExcessExpProcessType;
    }

	void SET_EVENT_EXP_SCALE ( float fScale )
	{
		if ( fScale < 1.0f )	fScale = 1.0f;
		if ( fScale > 4.0f )	fScale = 4.0f;

		fEVENT_EXP_SCALE = fScale;
	}

	float GET_EXP_SCALE ()
	{
		return (fEXP_SCALE * fEVENT_EXP_SCALE);
	}

	void SET_EVENT_ITEM_SCALE ( float fScale )
	{
		if ( fScale < 1.0f )	fScale = 1.0f;
		if ( fScale > 10.0f )	fScale = 10.0f;

		fEVENT_ITEM_DROP_SCALE = fScale;
	}
	
	float GET_ITEM_SCALE ()
	{
		return (fITEM_DROP_SCALE * fEVENT_ITEM_DROP_SCALE);
	}

	void SET_EVENT_MONEY_SCALE ( float fScale )
	{
		if ( fScale < 1.0f )	fScale = 1.0f;
		if ( fScale > 10.0f )	fScale = 10.0f;

		fEVENT_MONEY_DROP_SCALE = fScale;
	}
	
	float GET_MONEY_SCALE ( const float fAddScale )
	{
		return ( (fMONEY_DROP_SCALE + (fMONEY_DROP_SCALE*fAddScale) ) * fEVENT_MONEY_DROP_SCALE);
	}

	const char* GETSCHOOLNAME ( WORD wSCHOOL )
	{
		if ( wSCHOOL >= MAX_SCHOOL )	return NULL;
		return strSCHOOLNAME[wSCHOOL].c_str();
	}

	DWORD GETEXP ( int nAttackerLev, int nDefenserLev )
	{
		//	Note : ����ġ ���̺� ������.  (GLCONST_CHAR::EXPBASEINDEX+1) ���� +1�� �������̸� ������ �ȵ�����
		//			�ʹ� ������ +1 ������ �����Ǿ� ��� �����ǰ� �ִ� ��Ȳ��.
		int	nDiffLev = nDefenserLev - nAttackerLev;
		int nResultIndex = nDiffLev + GLCONST_CHAR::EXPBASEINDEX+1;
		
		if ( nResultIndex > (GLCONST_CHAR::EXPTABLE_LVL - 1) )		nResultIndex = GLCONST_CHAR::EXPTABLE_LVL - 1;
		else if ( nResultIndex < 0 )								nResultIndex = 0;
		GASSERT ( 0 <= nResultIndex && nResultIndex < GLCONST_CHAR::EXPTABLE_LVL );
		
		int nexp_lvl = nAttackerLev / EXP_LVL_STEP;
		if ( nexp_lvl < 0 )						nexp_lvl = 0;
		else if ( nexp_lvl >= EXPTABLE_SIZE )	nexp_lvl = EXPTABLE_SIZE-1;

		return (DWORD)(GLCONST_CHAR::wEXPTABLE[nexp_lvl][nResultIndex]*GET_EXP_SCALE());
	}

	DWORD GETEXP_PC ( WORD wLev )
	{
		WORD wLEVEL = wLev-1;
		if ( wLEVEL >= MAX_LEVEL )		return 0;
		
		return dwEXP_PC_TABLE[wLEVEL];
	}

	float GETEXP_RATE ( int nAttackerLev, int nDefenserLev )
	{
		int	nDiffLev = nDefenserLev - nAttackerLev;
		int nResultIndex = nDiffLev + GLCONST_CHAR::EXPTABLE_RANGE_BASE;

		if (nResultIndex >= GLCONST_CHAR::EXPTABLE_RANGE)
            nResultIndex = GLCONST_CHAR::EXPTABLE_RANGE-1;
		else if (nResultIndex < 0)
            nResultIndex = 0;

        return GLCONST_CHAR::fEXP_RATE_TABLE[nResultIndex];
	}

    int GetNonPkConnectBrightPoint()
    {
        return nNON_PK_CONNECT_BRIGHT_POINT;
    }

	EMCLASSINDEX GetClassInfoUsingSkill(SNATIVEID sNID)
	{
		GASSERT(sNID.wMainID<EMSKILLCLASS_NSIZE);
		switch ( static_cast<EMSKILLCLASS>(sNID.wMainID) )
		{
		case EMSKILL_FIGHTER_01:
		case EMSKILL_FIGHTER_02:
		case EMSKILL_FIGHTER_03:
		case EMSKILL_FIGHTER_04:
			return GLCL_FIGHTER;

		case EMSKILL_ARMS_01:
		case EMSKILL_ARMS_02:
		case EMSKILL_ARMS_03:
		case EMSKILL_ARMS_04:
			return GLCL_ARMS;

		case EMSKILL_ARCHER_01:
		case EMSKILL_ARCHER_02:
		case EMSKILL_ARCHER_03:
		case EMSKILL_ARCHER_04:
			return GLCL_ARCHER;

		case EMSKILL_SPIRIT_01:
		case EMSKILL_SPIRIT_02:
		case EMSKILL_SPIRIT_03:
		case EMSKILL_SPIRIT_04:
			return GLCL_SPIRIT;

		case EMSKILL_EXTREME_01:
		case EMSKILL_EXTREME_02:
		case EMSKILL_EXTREME_03:
		case EMSKILL_EXTREME_04:
			return GLCL_EXTREME;

		case EMSKILL_SCIENTIST_01:
		case EMSKILL_SCIENTIST_02:
		case EMSKILL_SCIENTIST_03:
		case EMSKILL_SCIENTIST_04:
			return GLCL_SCIENTIST;

		case EMSKILL_ASSASSIN_01:
		case EMSKILL_ASSASSIN_02:
		case EMSKILL_ASSASSIN_03:
		case EMSKILL_ASSASSIN_04:
			return GLCL_ASSASSIN;

		case EMSKILL_TRICKER_01:
		case EMSKILL_TRICKER_02:
		case EMSKILL_TRICKER_03:
		case EMSKILL_TRICKER_04:
			return GLCL_TRICKER;

		case EMSKILL_ETC_01:
		case EMSKILL_ETC_02:
		case EMSKILL_ETC_03:
		case EMSKILL_ETC_04:
			return GLCL_ETC;

		case EMSKILL_ACTOR_01:
		case EMSKILL_ACTOR_02:
		case EMSKILL_ACTOR_03:
		case EMSKILL_ACTOR_04:
		case EMSKILL_ACTOR_05:
		case EMSKILL_ACTOR_06:
		case EMSKILL_ACTOR_07:
		case EMSKILL_ACTOR_08:
			return GLCL_ACTOR;

		default:
			return GLCL_NUM_CLASS;
		}
		return GLCL_NUM_CLASS;
	}
}
	
namespace GLCONST_PET
{
	VEC_PGLPET pGLPET;
	VEC_PET_STYLE sPETSTYLE;

	float			fRunArea	   = 60.0f; // �޷��߸� �ϴ� �Ÿ�
	float			fWalkArea	   = 40.0f; // �ɾ�߸� �ϴ� �Ÿ�
	float			fOwnerDistance = 20.0f; // ���ΰ��� ���� �Ÿ�
	int				nMAXVIEWRANGE  = 50;    // �ֺ� �������� Ž���� �� �ִ� �Ÿ�
	float			fMaginotLineHP  = 20.0f; // �ڵ����� ��ų�� ������ ������ �Դ� ��ġ(%) - HP
	float			fMaginotLineMP  = 20.0f; // �ڵ����� ��ų�� ������ ������ �Դ� ��ġ(%) - MP
	float			fMaginotLineSP  = 20.0f; // �ڵ����� ��ų�� ������ ������ �Դ� ��ġ(%) - SP

	PGLPET GetPetData(int emTYPE)
	{
		for (WORD i = 0; i < COMMENT::PET_DEFINE.size(); ++i)
		{
			if (pGLPET[i]->m_emTYPE == emTYPE)
				return pGLPET[i];
		}

		return NULL;
	}

	PETSTYLE GetPetStyle(int emTYPE)
	{
		for (WORD i = 0; i < COMMENT::PET_DEFINE.size(); ++i)
		{
			if (pGLPET[i]->m_emTYPE == emTYPE)
				return sPETSTYLE[i];
		}
		return PETSTYLE();
	}
	bool IsChangeablePetColor(int emTYPE)
    {
		for (WORD i = 0; i < COMMENT::PET_DEFINE.size(); ++i)
		{
			if (pGLPET[i]->m_emTYPE == emTYPE)
				return pGLPET[i]->bChangeableColor;
		}
        return false;
    }
}

namespace MINIGAME_ODDEVEN // �̴� ����, Ȧ¦ ����
{
	float	fReturnRate[MAX_ROUND]	= {1.5f,2.0f,3.0f,4.0f,5.0f};	// ���� ���� �μ�(�������� = ����*1ȸ���μ�*2ȸ���μ�...)
	float	fSuccessRate[MAX_ROUND]	= {0.5f,0.4f,0.2f,0.1f,0.05f};	// ���� Ȱ��
	float	fTimeLimit				= 60.0f;						// ���� �ð�(��)
	float	fShuffleTime			= 2.0f;							// ���� �ð�(��)
	UINT	uiMaxBattingMoney		= 1000000000;					// �ִ� ���� ���� �ݾ�
}

namespace GLCONST_SUMMON
{
	PGLSUMMON pGLSUMMON[SUMMON_TYPE_SIZE] = { NULL };

	// Note : ��ȯ����Ʈ �����Ÿ� ����
	FLOAT fSummonGateDistanceRatio[SUMMON_TYPE_SIZE] = { 1.0f 
														,1.0f 
														,1.0f  
													    ,1.0f };
	// IDLE Gesture �ִ� ��� �ð�
	DWORD dwIdleGestureTimeMax[SUMMON_TYPE_SIZE] = { 20 
													,20 
													,20  
													,20 };
	// IDLE Gesture �ּ� ��� �ð�
	DWORD dwIdleGestureTimeMin[SUMMON_TYPE_SIZE] = { 5 
													,5 
													,5  
													,5 };
	// �ִ� ��ȯ ����
	DWORD dwMaxSummon[SUMMON_TYPE_SIZE] = { 10 
										   ,10 
										   ,10  
										   ,10 };

	// �ִ� ������ ����
	WORD wMaxMobTracking[SUMMON_TYPE_SIZE] =  {  3
												,3
												,3
												,3 };

	// Note : CharSet
	char szSummonInit[SUMMON_TYPE_SIZE][MAX_PATH] = { "Summon_01.charset"
	                                                 ,"Summon_02.charset"
	                                                 ,"Summon_03.charset"
	                                                 ,"Summon_04.charset"
													 ,"Summon_05.charset"
													 ,"Summon_06.charset"
													 ,"Summon_07.charset"};

	PGLSUMMON GetSummonData ( SUMMON_TYPE emTYPE )
	{	
		if ( emTYPE >= SUMMON_TYPE_SIZE || emTYPE == SUMMON_TYPE_NONE )
		{
			return NULL;
		}

		return pGLSUMMON[emTYPE];
	}

}

namespace GLCONST_VEHICLE
{
	PGLVEHICLE		pGLVEHICLE[VEHICLE_TYPE_SIZE] = { NULL };
	char			szVehicleInit[VEHICLE_TYPE_SIZE][MAX_PATH] = { "Vehicle_01.charset",
																"Vehicle_02.charset",
																"Vehicle_03.charset",
																"Vehicle_04.charset" };

	int				nFullDecrVehicle[VEHICLE_TYPE_SIZE]; // ������ ����ġ
	
	PGLVEHICLE GetVehicleData ( VEHICLE_TYPE emTYPE )
	{
		for ( WORD i = 0; i < VEHICLE_TYPE_SIZE; ++i )
		{
			if ( pGLVEHICLE[i]->m_emTYPE == emTYPE ) return pGLVEHICLE[i];
		}

		return NULL;
	}
}

namespace GLCONST_ATTENDANCE
{	
	bool	bUse = false;
	DWORD	dwMAXREWARD = 0;
	DWORD	dwAttendTime = 0;
	std::vector<ATTEND_REWARD>	vecAttendReward;
}

namespace HAIRCOLOR
{
	WORD wHairColor[MAXSEX][MAX_HAIR] = 
	{ 
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// ���� 8
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// ���� 16
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// ���� 24
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// ���� 32
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// ���� 40
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// ���� 8
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// ���� 16
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// ���� 24
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// ���� 32
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// ���� 40
	};

	WORD GetHairColor ( WORD wClass, WORD wStyle )
	{
		int nSex(0);
		switch( wClass )
		{
		case GLCI_ARCHER_W:
		case GLCI_SPIRIT_W:
		case GLCI_EXTREME_W:
		case GLCI_FIGHTER_W:
		case GLCI_ARMS_W:
		case GLCI_SCIENTIST_W:
		case GLCI_ASSASSIN_W:
		case GLCI_TRICKER_W:
		case GLCI_ETC_W:
		case GLCI_ACTOR_W:
			nSex = 1;
			break;

		case GLCI_ARCHER_M:
		case GLCI_SPIRIT_M:
		case GLCI_EXTREME_M:
		case GLCI_FIGHTER_M:
		case GLCI_ARMS_M:
		case GLCI_SCIENTIST_M:
		case GLCI_ASSASSIN_M:
		case GLCI_TRICKER_M:
		case GLCI_ETC_M:
		case GLCI_ACTOR_M:
		default:
			nSex = 0;
			break;
		};
		return wHairColor[nSex][wStyle];
	}
}

namespace MONEYCOLOR
{
    DWORD dwTEXT_MONEY_COLOR_BEGIN  = 100000;
    DWORD dwMONEY_COLOR_DEFAULT     = 0xFF98FB98;

	DWORD dwMONEY_COLOR[MAX_COLOR] =
	{
		0xFF98FB98,
		0xFF98FB98,
		0xFF98FB98,
        0xFF98FB98,
        0xFF98FB98,
        0xFF98FB98,
	};

    const DWORD GetDigit (const LONGLONG lNumber)
    {
        LONGLONG lTemp    = lNumber;
        DWORD    dwDigits = 0;

        do {
            lTemp /= 10;
            ++dwDigits;
        } while ( lTemp );

        return dwDigits;
    }

    const DWORD GetMoneyColor (const LONGLONG lnMoney)
    {
        if ( lnMoney < (LONGLONG)dwTEXT_MONEY_COLOR_BEGIN )
        {
            return dwMONEY_COLOR_DEFAULT;
        }

        DWORD dwBeginDigit = GetDigit ((LONGLONG)dwTEXT_MONEY_COLOR_BEGIN);
        DWORD dwCurDigit   = GetDigit (lnMoney);

        DWORD dwIndex = dwCurDigit - dwBeginDigit;

        if ( MONEYCOLOR::MAX_COLOR <= dwIndex )
        {
            return dwMONEY_COLOR [MONEYCOLOR::MAX_COLOR-1];
        }

        return dwMONEY_COLOR[dwIndex];
    }
}

namespace ITEM_INFOR_COLOR
{
    DWORD dwDefault = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // �⺻ �÷�.
    DWORD dwItemRank[ EMITEM_LEVEL_NSIZE ] =
    {
          D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF)
        , D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF)
        , D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF)
        , D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF)
        , D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF)
		, D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF)
    }; // ���� ��޿� ���� �÷�.
    DWORD dwReqWrong = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); 			// ���� ���� �÷�.
    DWORD dwInchant1 = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); 			// �⺻ ���ݷ� + ���� �ջ� �÷�.
    DWORD dwInchant2 = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); 			// ���� ���� �÷�.
    DWORD dwEquipCostum = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);		// �ڽ�Ƭ �� ���� ���� ���� �÷�.
    DWORD dwRankUp1 = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);			// �⺻ �ɷ�ġ + ���� �ջ� �÷�.
    DWORD dwRankUp2 = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);			// ���� ���� �÷�.
    DWORD dwSPReduce = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);			// SP �߰� �Ҹ� �÷�.
    DWORD dwAddAbility = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);		// ���� ȿ�� ��� �÷�.
    DWORD dwRandomAbility = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);		// ���� �ɼ� ��� �÷�.
    DWORD dwElementalAbility = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);	// �����̻� ��� �÷�.
    DWORD dwTrade = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);				// �ŷ� �� ��ȯ �Ұ� ��� �÷�.
    DWORD dwLimitTime = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);			// ������ ��ȿ�Ⱓ ��� �÷�.
	DWORD dwAddDisguise = D3DCOLOR_ARGB( 0xFF,0xFF,0xFF,0xFF );		// �ڽ�Ƭ ������ ���� ���� �ɼ� �÷�.
	DWORD dwMoney = D3DCOLOR_ARGB( 0xFF,0xFF,0xFF,0xFF );			// ��� �ؽ�Ʈ �÷�.
	DWORD dwSetItem = D3DCOLOR_ARGB( 0xFF,0xFF,0xFF,0xFF );			// ��Ʈ ������ �÷�.

	ITEM_GRIND dwItemGrind[ITEM_GRIND::TOTAL_COUNT];
}

namespace SKILL_INFOR_COLOR
{
    DWORD dwDefault = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // �⺻ �÷�.
    DWORD dwSkillName = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // ��ų �̸� ��� �÷�.
    DWORD dwSkillGrade = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // ��ų ��� ��� �÷�.
    DWORD dwSkillType = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // ��ų Ÿ�� ��� �÷�.(�Ϲ���, �нú��� ���)
    DWORD dwSkillElemental = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // ��ų �Ӽ� ��� �÷�.
    DWORD dwReqWrong = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // ��ų ���� ���� ���� ��� �÷�.
    DWORD dwSkillRange = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // ��ų ���� ���� ��� �÷�.
    DWORD dwAddAbility = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // ��ų ȿ�� ���� ��� �÷�.
    DWORD dwElementalAbility = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF); // ��ų �����̻� ��� �÷�.
}

namespace CHAT_INFOR_COLOR
{
	DWORD CHAT_NORMAL			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_PRIVATE			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_PARTY			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_MASTER			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_GUILD			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_ALLIANCE			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_AREA				= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_PARTY_RECRUIT	= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_TOALL			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_SYSTEM			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_FACTION			= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_CHAR_NAME		= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
	DWORD CHAT_GM				= D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
}

//----------------------------------------------------------------------------------------
