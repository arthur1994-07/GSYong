#include "pch.h"
#include <shlobj.h>
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
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogic/Activity/ActivityBase.h"
#include "../RanLogic/GLUseFeatures.h"



#include "../RanLogic/RANPARAM.h"
#include "../MfcExLib/RanFilter.h"

#include "./Guidance/GLGuidance.h"
#include "./ClubDeathMatch/GLClubDeathMatch.h"
#include "./AutoLevel/GLAutoLevelMan.h"
#include "./FieldServer/GLGaeaServer.h"
#include "./AgentServer/GLAgentServer.h"
#include "./DxServerInstance.h"
#include "Util/GLItemLimit.h"
#include "DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace DxServerInstance
{
    HRESULT CreateBefore(
        const char* _szAppPath,
        SUBPATH* pPath,
        language::LANGFLAG dwLangSet,
        EMSERVICE_PROVIDER ServiceProvider,
        bool bPK_MODE,
        BOOL bPKLess,
        bool bToolMode)
    {
        HRESULT hr = S_OK;
        const std::string strAppPath(_szAppPath);        
        char szFullPath[MAX_PATH] = {0};       

        // ����� ���� ������ �����Ǿ�� �ϴ� ���� ����.
        TCHAR szPROFILE[MAX_PATH] = {0};
        SHGetSpecialFolderPath(NULL, szPROFILE, CSIDL_PERSONAL, FALSE);

        StringCchCopy(szFullPath, MAX_PATH, szPROFILE);
        StringCchCat (szFullPath, MAX_PATH, pPath->SaveRoot().c_str());
        CreateDirectory(szFullPath, NULL);

        StringCchCopy(szFullPath, MAX_PATH, szPROFILE);
        StringCchCat (szFullPath, MAX_PATH, pPath->DebugInfoRoot().c_str());
        CreateDirectory(szFullPath, NULL);

        StringCchCopy(szFullPath, MAX_PATH, szPROFILE);
        StringCchCat (szFullPath, MAX_PATH, pPath->PlayInfoRoot().c_str());
        CreateDirectory(szFullPath, NULL);

        StringCchCopy(szFullPath, MAX_PATH, szPROFILE);
        StringCchCat (szFullPath, MAX_PATH, pPath->Capture().c_str());
        CreateDirectory(szFullPath, NULL);

        StringCchCopy(szFullPath, MAX_PATH, szPROFILE);
        StringCchCat (szFullPath, MAX_PATH, pPath->UiInfoRoot().c_str());
        CreateDirectory(szFullPath, NULL);

        // �⺻ ���� ���� ����
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->Cache().c_str());
        CreateDirectory(szFullPath, NULL);

        // ����׻��� �ʱ�ȭ
        CDebugSet::OneTimeSceneInit(pPath, false);

        // �������� �б�.
        RANPARAM::LOAD(strAppPath.c_str(), ServiceProvider, bToolMode, true);

        // ������ ���� �ʱ�ȭ.
        sc::Random::getInstance();

        // DxSkeletonManager �⺻ ���� ����.
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->ObjFileSkeleton().c_str());
        DxSkeletonManager::GetInstance().SetPath(szFullPath);

        // Animation �⺻ ���� ����.
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->ObjFileAnimation().c_str());
        DxSkinAnimationManager::GetInstance().SetPath(szFullPath);

        // SkinObject �⺻ ���� ����.
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->ObjFileSkinObject().c_str());
        DxSkinObject::SetPath(szFullPath);
        DxSkinCharDataContainer::GetInstance().SetPath(szFullPath);
        DxAttBoneDataContainer::GetInstance().SetPath(szFullPath);
        DxSkinPieceContainer::GetInstance().SetPath(szFullPath);
        DxAttBoneLink::SetPath (szFullPath);

        // DxLandMan
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->MapFile().c_str());
        DxLandMan::SetPath(szFullPath);

        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->MapFile().c_str());
        GLMapAxisInfo::SetPath(szFullPath);

        // GLogic
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->GLogicPath().c_str());
        GLOGIC::SetPath(szFullPath);

        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->GLogicServerPath().c_str());
        GLOGIC::SetServerPath(szFullPath);

        GLogicData::GetInstance().SetLogicFileFullPath(GLOGIC::UseLogicPackFile());
        GLOGIC::SetEngFullPath();
        GLOGIC::OpenPackFile(strAppPath.c_str(),RANPARAM::RPFDataPath);

		if ( !GLUseFeatures::GetInstance().LoadFile(GLogicData::GetInstance().GetUseFeaturesFileName(), pPath, FALSE))
		{
			//::MessageBox(NULL, "Usefeatures load error", "err", MB_OK|MB_ICONEXCLAMATION);
			sc::writeLogError("CreateBefore-Usefeatures load error");
		}

        // Gui, Load ServerText_kr.txt
        // By ���
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->GuiFileRoot().c_str());
		CGameTextMan::GetInstance().SetPath( szFullPath );
        CGameTextMan::GetInstance().LoadText(RANPARAM::strServerText.GetString(), CGameTextMan::EM_SERVER_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT|CGameTextMan::GAMETEXTMAN_LOADOP_SERVEREXE);
		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameInText.GetString(), CGameTextMan::EM_GAME_IN_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT|CGameTextMan::GAMETEXTMAN_LOADOP_SERVEREXE);
		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameWord.GetString(), CGameTextMan::EM_GAME_WORD, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT|CGameTextMan::GAMETEXTMAN_LOADOP_SERVEREXE);
		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameExText.GetString(), CGameTextMan::EM_GAME_EX_TEXT,  RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT|CGameTextMan::GAMETEXTMAN_LOADOP_SERVEREXE);


        // Npc ��ȭ ����
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->NtkFileRoot().c_str());
        CNpcDialogueSet::SetPath(szFullPath);

        // Quest ����
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->QuestFileRoot().c_str());
        GLQuestMan::GetInstance().SetPath(szFullPath);


		// Activity ����
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->ActivityFileRoot().c_str());        


        // ���� ����. �ý�����
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->GLogicPath().c_str());
        GLBusStation::GetInstance().SetPath (szFullPath);
        GLTaxiStation::GetInstance().SetPath(szFullPath);
        GLProductRecipeMan::GetInstance().SetPath (szFullPath);

        // GLLandMan ��� ����.
        StringCchCopy(szFullPath, MAX_PATH, strAppPath.c_str());
        StringCchCat (szFullPath, MAX_PATH, pPath->GLMapFile().c_str());
        GLLandMan::SetPath(szFullPath);

        GLPeriod::GetInstance().LoadFile("period.ini", bToolMode);
        
        // Logic data �ʱ�ȭ.
        hr = GLogicData::GetInstance().LoadData(TRUE, pPath, bToolMode, FALSE, NULL);
        if (FAILED(hr))
        {
            sc::writeLogError(std::string("GLogicData.LoadData Failed"));
            return hr;
        }		

		CRanFilter::GetInstance().SetCodePage(CD3DFontPar::nCodePage[dwLangSet]);
		if (!CRanFilter::GetInstance().Init(
			GLOGIC::UseLogicZipFile(),
			GLOGIC::UseLogicPackFile(),
			GLogicData::GetInstance().GetGlogicZipFile().c_str(), 
			GLOGIC::GetPath())) 
		{
			GASSERT(0 && "[ERROR] : Ran Filter Initialize.");
			return E_FAIL;
		}

        // pk ��� ����.
        GLCONST_CHAR::bPK_MODE = bPK_MODE;
        GLCONST_CHAR::bPKLESS  = bPKLess;		
        sc::writeLogInfo(sc::string::format("PK MODE : %d", bPK_MODE ));

		// �Ϻη� Memory Leak �� �������.
		// vld �� �� �پ��ִ��� Ȯ�� �ϱ� ����.
		bool* pMemoryLeak = new bool;

        return S_OK;
    }

    void CreateAfter()
    {
        SPRERIODTIME sPRERIODTIME;
        GETSEEDGAMETIME(GLPeriod::REALTIME_TO_VBR, sPRERIODTIME);
        GLPeriod::GetInstance().SetPeriod(sPRERIODTIME);
        GLPeriod::GetInstance().UpdateWeather(0.0f);
    }
}