#include "pch.h"

#include "../SigmaCore/Util/EBTime.h"
#include "../SigmaCore/Math/Random.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/G-Logic/GLPeriod.h"
#include "../EngineLib/Common/seqrandom.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/GameTextLoader.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/G-Logic/DxConsoleMsg.h"
#include "../EngineLib/G-Logic/DxMsgServer.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/Item/GLITEMLMT.h"
#include "../RanLogic/Transport/GLTaxiStation.h"
//#include "../RanLogic/Item/GLItemMixMan.h"
#include "../RanLogic/Product/GLProductRecipeMan.h"
#include "../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../RanLogic/Quest/GLQuestMan.h"
#include "../RanLogic/Transport/GLBusStation.h"
#include "../RanLogic/Activity/ActivityBase.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/RANPARAM.h"

#include "../MfcExLib/RanFilter.h"

#include "./Guidance/GLGuidance.h"
#include "./ClubDeathMatch/GLClubDeathMatch.h"
#include "./AutoLevel/GLAutoLevelMan.h"
#include "./Server/AgentServerConfigXml.h"
#include "./Server/FieldServerConfigXml.h"
#include "./FieldServer/GLGaeaServer.h"
#include "./AgentServer/GLAgentServer.h"
#include "./DxServerInstance.h"
#include "Util/GLItemLimit.h"


// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace DxServerInstance
{
	HRESULT FieldCreate(
        GLGaeaServer* pGaeaServer,
        const char* _szAppPath,
        DxMsgServer* pMsgServer, /*DxConsoleMsg* pConsoleMsg,*/
        db::IDbManager* pDBMan,
		db::IDbManager* pAdoMan,
        EMSERVICE_PROVIDER emServiceProvider,        
        SUBPATH* pPath,
        const char* szMapList,
        DWORD dwFieldSID,
        DWORD dwMaxClient,
        bool bPK_MODE,
        int nChannel,
        BOOL bPKLess,
        BYTE ExcessExpProcessType,
        language::LANGFLAG dwLangSet,
        bool bPKServer,
        bool bToolMode,
		FieldServerConfigXml* pConfig)
	{
		HRESULT hr = S_OK;

		// ���̾� ������ �ʿ��� ���� ��ġ���� �ʱ�ȭ.
		hr = CreateBefore(_szAppPath, pPath, dwLangSet, emServiceProvider, bPK_MODE, bPKLess, bToolMode);
        if (FAILED(hr))
        {
            sc::writeLogError(std::string("CreateBefore failed"));
            return E_FAIL;
        }

		// ���� ����
        hr = pGaeaServer->Create(
            dwMaxClient,
            pMsgServer, /*pConsoleMsg,*/
            pDBMan,
            pAdoMan,
            emServiceProvider,
            szMapList,
            dwFieldSID,
            nChannel,
            ExcessExpProcessType,
            bToolMode);

		if (FAILED(hr))
		{
			sc::writeLogError(std::string("[Gaea Server] Start Failed : GLGaeaServer::Create ()"));
			return E_FAIL;
		}
        
		if ( SP_GS == emServiceProvider && pConfig )
		{
			// ����Ʈ���� �и��� �ɼ� ó��
			GLCONST_CHAR::bPARTY_2OTHERSCHOOL = pConfig->GetPARTY_2OTHERSCHOOL();
			GLCONST_CHAR::bCLUB_2OTHERSCHOOL = pConfig->GetCLUB_2OTHERSCHOOL();
			GLCONST_CHAR::fITEM_DROP_SCALE = pConfig->GetITEM_DROP_SCALE();
			GLCONST_CHAR::fMONEY_DROP_SCALE = pConfig->GetMONEY_DROP_SCALE();
			GLCONST_CHAR::fITEM_DROP_LOWER_SCALE = pConfig->GetITEM_DROP_LOWER_SCALE();
			GLCONST_CHAR::dwCRITICAL_MAX = pConfig->GetCRITICAL_MAX();
			GLCONST_CHAR::fNONPK_ITEM_DROP = pConfig->GetNONPK_ITEM_DROP();
			GLCONST_CHAR::fPK_ITEM_DROP = pConfig->GetPK_ITEM_DROP();
		}

        sc::writeLogInfo(std::string("[GLGaeaServer Server] Created"));

		// FreePKServer ( ��� ���� ���� ) 
		pGaeaServer->m_bPKServer = bPKServer;

        CreateAfter();		
		
		sc::writeLogInfo(sc::string::format("[Gaea Server] Excess Experience Process Type : %d", (int)ExcessExpProcessType ));
		sc::writeLogInfo(std::string("[Gaea Server] Start Succeeded"));
		sc::writeLogInfo(std::string("[Gaea Server] ---------------"));


		return S_OK;
	}

	HRESULT FieldFrameMove(GLGaeaServer* pGaeaServer)
	{
		HRESULT hr;

		//mjeon.ToDo
		//Why these time variables and timecheck rountine required??
		//Timecheck has alread done in Update(), UpdateProc, when this FrameMove() is called.
		//( Before calling of DxServerInstance::FieldFrameMove() )
		//
		//Following timecheck routine might be removed after some test.
		//
		static float fAppTime(0.0f), fNewAppTime(0.0f), fElapsedAppTime(0.0f), fReservationTime(0.0f);

		fNewAppTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );

		if ( fAppTime == 0.0f )
			fAppTime = fNewAppTime;

		if ( (fNewAppTime - fAppTime) < 0.020f )
		{
			Sleep( 0 );
			//return S_FALSE;
		}

		//
		//mjeon
		//There is only one Update thread and it only accesses this variable.
		//Following comments seem to be wrong and never mind.
		//
		//�� 2���� ������????
		//
		// fElapsedAppTime ==> 2���� �����尡 ȣ���Ͽ� �� ������ ����ϴµ�
		// Sleep(0) �� �����ϴ� ���� �����尡 fElapsedAppTime ������ ����� ���� �ִ�.
		// �׷��Ƿ� ��Ȯ�ϰ� ������ �� ���� �ʴٸ� �������� �ʴ� ���� ����!!!

		fElapsedAppTime = fNewAppTime - fAppTime;
		fAppTime = fNewAppTime;

		//
		//mjeon
		//Is the following operation, CheckControlfp(), necessary?
		//
		// float ��Ȯ�� ���
		CheckControlfp();		

		// ���̾� ����.
		hr = pGaeaServer->FrameMove(fAppTime, fElapsedAppTime);		
		if (FAILED(hr))
			return hr;
		return S_OK;
	}

	void FieldCleanUp(GLGaeaServer* pGaeaServer)
	{
		// ����׻��� �ʱ�ȭ
		CDebugSet::FinalCleanup ();
		pGaeaServer->CleanUp ();
	}

	HRESULT AgentCreate(
        GLAgentServer* pGLAgentServer,
        const char* _szAppPath,
        DxMsgServer* pMsgServer, /*DxConsoleMsg* pConsoleMsg,*/
        db::IDbManager* pDBMan,
		db::IDbManager* pAdoMan,
        EMSERVICE_PROVIDER ServiceProvider,
        SUBPATH* pPath,
        char* szMapList,
        DWORD dwMaxClient,
        F_SERVER_INFO* pFieldInfo,
        bool bPK_MODE,
        BOOL bPKLess,
        language::LANGFLAG dwLangSet,
        bool bToolMode,
		AgentServerConfigXml* pConfig)
	{
		HRESULT hr = S_OK;	

		// ���̾� ������ �ʿ��� ���� ��ġ���� �ʱ�ȭ.
        hr = CreateBefore(_szAppPath, pPath, dwLangSet, ServiceProvider, bPK_MODE, bPKLess, bToolMode);
        if (FAILED(hr))
        {
            sc::writeLogError(std::string("CreateBefore failed"));
            return E_FAIL;
        }

		// ���� ����.
		hr = pGLAgentServer->Create(
            dwMaxClient,
            pMsgServer, /*pConsoleMsg,*/
            pDBMan,
			pAdoMan,
            szMapList,
            pFieldInfo,
            ServiceProvider,
            bToolMode);
        if (FAILED(hr))
        {
            sc::writeLogError(std::string("[Agent Server] Start Failed : GLAgentServer::Create()"));
            return E_FAIL;
        }

		if ( SP_GS == ServiceProvider && pConfig )
		{
			// ����Ʈ���� �и��� �ɼ� ó��
			GLCONST_CHAR::bPARTY_2OTHERSCHOOL = pConfig->GetPARTY_2OTHERSCHOOL();
			GLCONST_CHAR::bCLUB_2OTHERSCHOOL = pConfig->GetCLUB_2OTHERSCHOOL();
			GLCONST_CHAR::fITEM_DROP_SCALE = pConfig->GetITEM_DROP_SCALE();
			GLCONST_CHAR::fMONEY_DROP_SCALE = pConfig->GetMONEY_DROP_SCALE();
			GLCONST_CHAR::fITEM_DROP_LOWER_SCALE = pConfig->GetITEM_DROP_LOWER_SCALE();
			GLCONST_CHAR::dwCRITICAL_MAX = pConfig->GetCRITICAL_MAX();
			GLCONST_CHAR::fNONPK_ITEM_DROP = pConfig->GetNONPK_ITEM_DROP();
			GLCONST_CHAR::fPK_ITEM_DROP = pConfig->GetPK_ITEM_DROP();
		}
        
        sc::writeLogInfo(std::string("GLAgentServer created"));

		CreateAfter();
		
		sc::writeLogInfo(std::string("[Gaea Server] Start Succeeded"));
		sc::writeLogInfo(std::string("[Gaea Server] ---------------"));

		return S_OK;
	}

	HRESULT AgentFrameMove(GLAgentServer* pGLAgentServer)
	{
		static float fAppTime(0.0f), fNewAppTime(0.0f), fElapsedAppTime(0.0f);

		fNewAppTime = sc::DXUtil_Timer(sc::TIMER_GETAPPTIME);
		if (fAppTime == 0.0f)
            fAppTime = fNewAppTime;

		if ((fNewAppTime-fAppTime) < 0.020f)
			Sleep(0);

		// fElapsedAppTime ==> 2���� �����尡 ȣ���Ͽ� �� ������ ����ϴµ�
		// Sleep(0) �� �����ϴ� ���� �����尡 fElapsedAppTime ������ ����� ���� �ִ�.
		// �׷��Ƿ� ��Ȯ�ϰ� ������ �� ���� �ʴٸ� �������� �ʴ� ���� ����!!!
		fElapsedAppTime = fNewAppTime-fAppTime;
		fAppTime = fNewAppTime;

		// float ��Ȯ�� ���
		CheckControlfp();

        CTime CurrentTime(CTime::GetCurrentTime());

		// ���̾�(AGENT) ����.
		pGLAgentServer->FrameMove(fAppTime, fElapsedAppTime, CurrentTime);

		return S_OK;
	}

	void AgentCleanUp(GLAgentServer* pGLAgentServer)
	{
		// ����׻��� �ʱ�ȭ.
		CDebugSet::FinalCleanup();
		pGLAgentServer->CleanUp();
	}
}

HRESULT GMTOOL::Create(
    const char* szAppPath,
    SUBPATH* pPath,
    EMSERVICE_PROVIDER ServiceProvider,
    const FUNCPROC* pLoadProcedure,
	boost::function<void (std::string)> PercentCallbackFunc,
	boost::function<void (unsigned int)> PercentCallbackFunc2)
{
    HRESULT hr = S_OK;

	// ���� ��ġ���� �ʱ�ȭ.
	char szFullPath[MAX_PATH] = {0};

	// ������ ���� �ʱ�ȭ.
	sc::Random::getInstance();

	// ����׻��� �ʱ�ȭ.
	CDebugSet::OneTimeSceneInit(pPath, false);

	// DxSkeletonManager �⺻ ���� ����.
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat(szFullPath, MAX_PATH, pPath->ObjFileSkeleton().c_str()); //SUBPATH::OBJ_FILE_SKELETON);
	DxSkeletonManager::GetInstance().SetPath(szFullPath);

	// Animation �⺻ ���� ����.
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->ObjFileAnimation().c_str()); //SUBPATH::OBJ_FILE_ANIMATION );
	DxSkinAnimationManager::GetInstance().SetPath ( szFullPath );

	// SkinObject �⺻ ���� ����.
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->ObjFileSkinObject().c_str()); //SUBPATH::OBJ_FILE_SKINOBJECT );
	DxSkinObject::SetPath ( szFullPath );
	DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
	DxAttBoneDataContainer::GetInstance().SetPath ( szFullPath );
	DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );
	DxAttBoneLink::SetPath ( szFullPath );

	// DxLandMan
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->MapFile().c_str()); //SUBPATH::MAP_FILE );
	DxLandMan::SetPath ( szFullPath );

	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->GLogicPath().c_str()); //SUBPATH::GLOGIC_FILE );
	GLOGIC::SetPath ( szFullPath );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, pPath->GLogicServerPath().c_str()); //SUBPATH::GLOGIC_SERVER_FILE );
	GLOGIC::SetServerPath ( szFullPath );

	// GLogic zip ���� �н� ����
	// zip ���� �н��� ��� �߰� by ���
	GLogicData::GetInstance().SetLogicFileFullPath(GLOGIC::UseLogicPackFile());
	GLOGIC::SetEngFullPath();
	GLOGIC::OpenPackFile( szAppPath ,RANPARAM::RPFDataPath);

	// Npc ��ȭ ����
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->NtkFileRoot().c_str()); //SUBPATH::NTK_FILE_ROOT );
	CNpcDialogueSet::SetPath ( szFullPath );

	// Quest ����
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->QuestFileRoot().c_str()); //SUBPATH::QST_FILE_ROOT );
	GLQuestMan::GetInstance().SetPath ( szFullPath );

	// Activity ����
    StringCchCopy(szFullPath, MAX_PATH, szAppPath);
    StringCchCat (szFullPath, MAX_PATH, pPath->ActivityFileRoot().c_str()); //SUBPATH::ACTV_FILE_ROOT);
	//ActivityBaseData::GetInstance()->SetPath(std::string(szFullPath));

	// ���� ����. �ý� ����
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->GLogicPath().c_str()); //SUBPATH::GLOGIC_FILE );
	GLBusStation::GetInstance().SetPath ( szFullPath );
	GLTaxiStation::GetInstance().SetPath ( szFullPath );
	GLTaxiStation::GetInstance().SetPath ( szFullPath );
	GLProductRecipeMan::GetInstance().SetPath ( szFullPath );

	// GLLandMan ��� ����.
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->GLMapFile().c_str()); //SUBPATH::GLMAP_FILE );
	GLLandMan::SetPath ( szFullPath );

    // RANPARAM �ε�
    RANPARAM::LOAD(szAppPath, ServiceProvider, true, false);

    // Note : �������̽� ��ų �ؽ�Ʈ �ε�
    StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
    StringCchCat  ( szFullPath, MAX_PATH, pPath->GuiFileRoot().c_str()); //SUBPATH::GUI_FILE_ROOT);
    CGameTextMan::GetInstance().SetPath(szFullPath);
    CGameTextMan::GetInstance().LoadText(RANPARAM::strGameInText.GetString(), CGameTextMan::EM_GAME_IN_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);
	CGameTextMan::GetInstance().LoadText(RANPARAM::strGameWord.GetString(), CGameTextMan::EM_GAME_WORD, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);
	// ���� Data �ʱ�ȭ
    hr = GLogicData::GetInstance().LoadData(TRUE, pPath, true, PercentCallbackFunc, PercentCallbackFunc2);	//loading bottle-neck
	

	if (FAILED(hr))
        return hr;

    // Load Procedure
    if ( pLoadProcedure )
    {
        (*pLoadProcedure)();
    }

    return S_OK;
}

void GMTOOL::CleanUp()
{
	CDebugSet::FinalCleanup();
}
