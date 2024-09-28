#include "pch.h"
#include "../SigmaCore/String/basestring.h"
#include "../SigmaCore/Container/MList.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Util/MinMessageBox.h"

#include "../EngineLib/Common/GLTexFile.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"

#include "./Character/GLCharData.h"
#include "./Crow/GLCrowData.h"
#include "./Item/GLItemMan.h"
#include "./GLogicData.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

char szErrorFileName[MAX_PATH];

namespace GLCONST_CHAR
{
    BOOL LOADFILE(const std::string& FileName, BOOL bServer)
	{
		if (FileName.empty())
            return FALSE;

		//	Note : 
		std::string strPath;
		strPath = GLOGIC::GetPath();
		strPath += FileName;

		gltexfile cFILE;
		cFILE.reg_sep("\t ,{}[]()|\"");

		if (GLOGIC::UseLogicZipFile()) // by 경대
            cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

		if (!cFILE.open(strPath, true, GLOGIC::UseLogicPackFile()))
		{
            sc::writeLogError(
                sc::string::format(
                    "GLCONST_CHAR::LOADFILE %1% %2%", strPath, FileName));
			return FALSE;
		}

		WORD dwVERSION = 0;
		cFILE.getflag ( "VERSION", 1, 1, dwVERSION, true );

		//int i = 0;

		cFILE.getflag( "bTESTSERVER", 1, 1, bTESTSERVER, true );
		cFILE.getflag( "nUI_KEYBOARD", 1, 1, nUI_KEYBOARD, true ); // Note : 설정하지 않는 flag.
		cFILE.getflag( "nUI_DEFAULTKEYTYPE", 1, 1, nUI_DEFAULTKEYTYPE, true ); // 기본 키타입
		cFILE.getflag( "bBATTLEROYAL", 1, 1, bBATTLEROYAL, true );
		cFILE.getflag( "nMAX_FRIEND_NUMBER", 1, 1, nMAX_FRIEND_NUMBER, true );
        cFILE.getflag( "nMAX_FRIEND_LIST", 1, 1, nMAX_FRIEND_LIST, true );
        cFILE.getflag( "nMAX_FRIEND_GROUP", 1, 1, nMAX_FRIEND_GROUP, true );
        cFILE.getflag( "nMAX_FRIEND_BLOCK", 1, 1, nMAX_FRIEND_BLOCK, true );
		cFILE.getflag( "wLEVEL_FOR_EXTREME", 1, 1, wLEVEL_FOR_EXTREME, true ); // 극강부 생성 가능 레벨;
		cFILE.getflag( "wSTART_LEVEL_FOR_EXTREME", 1, 1, wSTART_LEVEL_FOR_EXTREME, true ); // 극강부 시작 레벨;
		cFILE.getflag( "wLEVEL_FOR_ACTOR", 1, 1, wLEVEL_FOR_ACTOR, true );	// 변술부 생성 만족 레벨;

		cFILE.getflag( "wSERVER_NORMAL", 1, 1, wSERVER_NORMAL, true ); // Note : 설정하지 않는 flag.
		cFILE.getflag( "wSERVER_CONGEST", 1, 1, wSERVER_CONGEST, true ); // Note : 설정하지 않는 flag.
		cFILE.getflag( "wSERVER_NOVACANCY", 1, 1, wSERVER_NOVACANCY, true ); // Note : 설정하지 않는 flag.

		//		cFILE.getflag( "fDISPRICE", 1, 1, fDISPRICE );
		cFILE.getflag( "wMAXITEM_AGE", 1, 1, wMAXITEM_AGE, true );
		cFILE.getflag( "wMAXITEM_HOLD", 1, 1, wMAXITEM_HOLD, true );
		cFILE.getflag( "wMAXMOB_TRACKING", 1, 1, wMAXMOB_TRACKING, true );

		for (int i=0; i<GLCI_NUM_ACTOR; ++i )
			cFILE.getflag( "fCHAR_HEIGHT", i+1, GLCI_NUM_ACTOR, fCHAR_HEIGHT[i], true );

		cFILE.getflag( "wSCHOOLNUM", 1, 1, wSCHOOLNUM, true );
		if ( wSCHOOLNUM >= MAX_SCHOOL )
            wSCHOOLNUM = MAX_SCHOOL;

        cFILE.getflag( "wENABLE_NEW_CLASS", 1, 1, wENABLE_NEW_CLASS );

		for (int i=0; i<wSCHOOLNUM; ++i )
		{
			CString strFLAG;

			strFLAG.Format ( "strSCHOOLNAME%02d", i );
			cFILE.getflag( strFLAG.GetString(), 1, 1, strSCHOOLNAME[i], true );

			strFLAG.Format ( "nidSTARTMAP%02d", i );
			cFILE.getflag( strFLAG.GetString(), 1, 2, nidSTARTMAP[i].wMainID, true );
			cFILE.getflag( strFLAG.GetString(), 2, 2, nidSTARTMAP[i].wSubID, true );

			strFLAG.Format ( "dwSTARTGATE%02d", i );
			cFILE.getflag( strFLAG.GetString(), 1, 1, dwSTARTGATE[i], true );
		}

		cFILE.getflag( "bPARTY_2OTHERSCHOOL", 1, 1, bPARTY_2OTHERSCHOOL, true );
		cFILE.getflag( "bCLUB_2OTHERSCHOOL", 1, 1, bCLUB_2OTHERSCHOOL, true );

        cFILE.getflag( "fCHAT_OVERINPUT_TIMER", 1, 1, fCHAT_OVERINPUT_TIMER, true );
        cFILE.getflag( "wCHAT_OVERINPUT_CANCEL_TIMER", 1, 1, wCHAT_OVERINPUT_CANCEL_TIMER, true );
        cFILE.getflag( "nCHAT_OVERINPUT_COUNT", 1, 1, nCHAT_OVERINPUT_COUNT, true );

        cFILE.getflag( "nidREBIRTH_RECOMMEND_ITEM", 1, 2, nidREBIRTH_RECOMMEND_ITEM.wMainID, true );
        cFILE.getflag( "nidREBIRTH_RECOMMEND_ITEM", 2, 2, nidREBIRTH_RECOMMEND_ITEM.wSubID, true );
        cFILE.getflag( "strREBIRTH_RECOMMEND_ITEM", 1, 1, strREBIRTH_RECOMMEND_ITEM, true );
        cFILE.getflag( "wAUTO_REBIRTH_TIMER", 1, 1, wAUTO_REBIRTH_TIMER, true );

        cFILE.getflag( "nidDIALOGUE_RECOMMEND_ITEM", 1, 2, nidDIALOGUE_RECOMMEND_ITEM.wMainID, true );
        cFILE.getflag( "nidDIALOGUE_RECOMMEND_ITEM", 2, 2, nidDIALOGUE_RECOMMEND_ITEM.wSubID, true );
        cFILE.getflag( "strDIALOGUE_RECOMMEND_ITEM", 1, 1, strDIALOGUE_RECOMMEND_ITEM, true );

        cFILE.getflag( "fBECOME_AWAY_TIME", 1, 1, fBECOME_AWAY_TIME, true );
        cFILE.getflag( "nAWAY_GESTURE_NUMBER", 1, 1, nAWAY_GESTURE_NUMBER, true );

        for (int i=0; i<QUESTION_SIZE; ++i )
            wNON_APPLY_QBOX_ON_VEHICLE[i] = QUESTION_SIZE;
        if ( cFILE.findflag( "wNON_APPLY_QBOX_ON_VEHICLE" ) )
        {
            wNON_APPLY_QBOX_ON_VEHICLE_NUM = static_cast<int>( cFILE.findflag( "wNON_APPLY_QBOX_ON_VEHICLE" )->size() - 1 );
            if ( wNON_APPLY_QBOX_ON_VEHICLE_NUM > QUESTION_SIZE )
                wNON_APPLY_QBOX_ON_VEHICLE_NUM = QUESTION_SIZE;
            for ( int i=0; i < wNON_APPLY_QBOX_ON_VEHICLE_NUM; ++i )
                cFILE.getflag( "wNON_APPLY_QBOX_ON_VEHICLE", 1+i, wNON_APPLY_QBOX_ON_VEHICLE_NUM, wNON_APPLY_QBOX_ON_VEHICLE[i], true );
        }

        for (int i=0; i<SKILL::FOR_TYPE_SIZE; ++i )
            wNON_APPLY_SKILL_TYPE_ON_VEHICLE[i] = SKILL::FOR_TYPE_SIZE;
        if ( cFILE.findflag( "wNON_APPLY_SKILL_TYPE_ON_VEHICLE" ) )
        {
            wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM = static_cast<int>( cFILE.findflag( "wNON_APPLY_SKILL_TYPE_ON_VEHICLE" )->size() - 1 );
            if ( wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM > SKILL::FOR_TYPE_SIZE )
                wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM = SKILL::FOR_TYPE_SIZE;
            for ( int i=0; i < wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM; ++i )
                cFILE.getflag( "wNON_APPLY_SKILL_TYPE_ON_VEHICLE", 1+i, wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM, wNON_APPLY_SKILL_TYPE_ON_VEHICLE[i], true );
        }

        for (int i=0; i<EIMPACTA_SIZE; ++i )
            wNON_APPLY_SKILL_IMPACT_ON_VEHICLE[i] = EIMPACTA_SIZE;
        if ( cFILE.findflag( "wNON_APPLY_SKILL_IMPACT_ON_VEHICLE" ) )
        {
            wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM = static_cast<int>( cFILE.findflag( "wNON_APPLY_SKILL_IMPACT_ON_VEHICLE" )->size() - 1 );
            if ( wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM > EIMPACTA_SIZE )
                wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM = EIMPACTA_SIZE;
            for ( int i=0; i < wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM; ++i )
                cFILE.getflag( "wNON_APPLY_SKILL_IMPACT_ON_VEHICLE", 1+i, wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM, wNON_APPLY_SKILL_IMPACT_ON_VEHICLE[i], true );
        }

        for (int i=0; i<EMSPECA_NSIZE; ++i )
            wNON_APPLY_SKILL_SPEC_ON_VEHICLE[i] = EMSPECA_NSIZE;
        if ( cFILE.findflag( "wNON_APPLY_SKILL_SPEC_ON_VEHICLE" ) )
        {
            wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM = static_cast<int>( cFILE.findflag( "wNON_APPLY_SKILL_SPEC_ON_VEHICLE" )->size() - 1 );
            if ( wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM > EMSPECA_NSIZE )
                wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM = EMSPECA_NSIZE;
            for ( int i=0; i < wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM; ++i )
                cFILE.getflag( "wNON_APPLY_SKILL_SPEC_ON_VEHICLE", 1+i, wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM, wNON_APPLY_SKILL_SPEC_ON_VEHICLE[i], true );
        }

        cFILE.getflag( "wMaxRebuyList", 1, 1, wMaxRebuyList, true );
        cFILE.getflag( "fRebuyMultiplePrice", 1, 1, fRebuyMultiplePrice, true );
        cFILE.getflag( "wMinRebuyPrice", 1, 1, wMinRebuyPrice, true );

		cFILE.getflag( "wMAX_LEVEL", 1, 1, wMAX_LEVEL, true );
		if( cFILE.getflag( "wMAX_EXTREME_LEVEL", 1, 1, wMAX_EXTREME_LEVEL, true ) == FALSE )
			wMAX_EXTREME_LEVEL = wMAX_LEVEL;
		cFILE.getflag( "wLVL_STATS_P", 1, 1, wLVL_STATS_P, true );
		cFILE.getflag( "wLVL_2ndSTATS_P", 1, 1, wLVL_2ndSTATS_P, true );
		cFILE.getflag( "fLVL_EXP_G", 1, 1, fLVL_EXP_G, true );
		cFILE.getflag( "fLVL_EXP_S", 1, 1, fLVL_EXP_S, true );	
		cFILE.getflag( "fREC_EXP_RATE", 1, 1, fREC_EXP_RATE, true );
		cFILE.getflag( "fKILL_EXP_RATE", 1, 1, fKILL_EXP_RATE, true );

		for (int i=0; i<DIE_DECEXP_NUM; ++i )
			cFILE.getflag( "fDIE_DECEXP", 1+i, DIE_DECEXP_NUM, fDIE_DECEXP[i], true );

		for (int i=0; i<DIE_DECEXP_NUM; ++i )
			cFILE.getflag( "fDIE_RECOVERYEXP", 1+i, DIE_DECEXP_NUM, fDIE_RECOVERYEXP[i], true );

		for (int i=0; i<DIE_DECEXP_NUM; ++i )
			cFILE.getflag( "fEXP_RATE_MONEY", 1+i, DIE_DECEXP_NUM, fEXP_RATE_MONEY[i], true );

		for (int i=0; i<EXPTABLE_RANGE; ++i )
			cFILE.getflag ( "fEXP_RATE_TABLE", 1+i, EXPTABLE_RANGE, fEXP_RATE_TABLE[i], true );


		/*cFILE.getflag( "wGRADE_MAX", 1, 1, wGRADE_MAX, true );
		if ( wGRADE_MAX > GRADE_LIMIT_MAX ) wGRADE_MAX = GRADE_LIMIT_MAX;
		if ( wGRADE_MAX < GRADE_HIGH ) wGRADE_MAX = GRADE_HIGH;*/

		//cFILE.getflag( "wGRADE_MAX_REGI", 1, 1, wGRADE_MAX_REGI, true ); // Note : 설정하지 않는 flag.
		//if ( wGRADE_MAX_REGI > GRADE_LIMIT_MAX ) wGRADE_MAX_REGI = GRADE_LIMIT_MAX;
		//if ( wGRADE_MAX_REGI < GRADE_HIGH ) wGRADE_MAX_REGI = GRADE_HIGH;

		//cFILE.getflag( "wDAMAGE_GRADE", 1, 1, wDAMAGE_GRADE, true );
		//cFILE.getflag( "wDEFENSE_GRADE", 1, 1, wDEFENSE_GRADE, true );

		//cFILE.getflag( "fDAMAGE_GRADE", 1, 1, fDAMAGE_GRADE, true );
		//cFILE.getflag( "fDEFENSE_GRADE", 1, 1, fDEFENSE_GRADE, true );


		//for (int i=0; i<GRADE_LIMIT_MAX-GRADE_HIGH; ++i )
		//	cFILE.getflag( "fDAMAGE_GRADE_TOP", 1+i, GRADE_LIMIT_MAX-GRADE_HIGH, fDAMAGE_GRADE_TOP[i], true );

		//for (int i=0; i<GRADE_LIMIT_MAX-GRADE_HIGH; ++i )
		//	cFILE.getflag( "fDEFENSE_GRADE_TOP", 1+i, GRADE_LIMIT_MAX-GRADE_HIGH, fDEFENSE_GRADE_TOP[i], true );

		//for (int i=0; i<GRADE_LIMIT_MAX-GRADE_HIGH; ++i )
		//	cFILE.getflag( "wUSE_GRADE_NUM", 1+i, GRADE_LIMIT_MAX-GRADE_HIGH, wUSE_GRADE_NUM[i], true );

		//cFILE.getflag( "wRESIST_FIRE_GRADE", 1, 1, wRESIST_FIRE_GRADE, true );
		//cFILE.getflag( "wRESIST_ICE_GRADE", 1, 1, wRESIST_ICE_GRADE, true );
		//cFILE.getflag( "wRESIST_ELEC_GRADE", 1, 1, wRESIST_ELEC_GRADE, true );
		//cFILE.getflag( "wRESIST_POISON_GRADE", 1, 1, wRESIST_POISON_GRADE, true );
		//cFILE.getflag( "wRESIST_SPIRIT_GRADE", 1, 1, wRESIST_SPIRIT_GRADE, true );

		cFILE.getflag( "wLVL_SKILL_P", 1, 1, wLVL_SKILL_P, true );
		cFILE.getflag( "wLVL_2ndSKILL_P", 1, 1, wLVL_2ndSKILL_P, true );

		cFILE.getflag( "wMAXATRANGE_SHORT", 1, 1, wMAXATRANGE_SHORT, true );
		cFILE.getflag( "fPUSHPULL_VELO", 1, 1, fPUSHPULL_VELO, true );
        cFILE.getflag( "fMOB_TRACING", 1, 1, fMOB_TRACING, true );
        cFILE.getflag( "fNPC_INTERACTION_RANGE", 1, 1, fNPC_INTERACTION_RANGE, true );
        cFILE.getflag( "fNPC_INTERACTION_ADJUST", 1, 1, fNPC_INTERACTION_ADJUST, true );

		cFILE.getflag( "nHitRateMinimum", 1, 1, nHitRateMinimum, true );
		cFILE.getflag( "nHitRateMaximum", 1, 1, nHitRateMaximum, true );	
		cFILE.getflag( "fHitRatePercentUnit", 1, 1, fHitRatePercentUnit, true);
		cFILE.getflag( "fAvoidRatePercentUnit",	1, 1, fAvoidRatePercentUnit, true);
		cFILE.getflag( "fAttackSuccesRate_LvScale",	1, 1, fAttackSuccesRate_LvScale, true);

		cFILE.getflag( "fLOW_SEED_DAMAGE", 1, 1, fLOW_SEED_DAMAGE, true );
		cFILE.getflag( "fCONFT_SPTY_EXP", 1, 1, fCONFT_SPTY_EXP, true );	

		cFILE.getflag( "wBODYRADIUS", 1, 1, wBODYRADIUS, true );	

		cFILE.getflag( "fREACT_VALID_SCALE", 1, 1, fREACT_VALID_SCALE, true );	
		cFILE.getflag( "fATTACK_RANGE_ADJUST_RATE", 1, 1, fATTACK_RANGE_ADJUST_RATE, true );	

		cFILE.getflag( "fUNIT_TIME",  1, 1, fUNIT_TIME, true );	

		cFILE.getflag( "fHP_INC_PER", 1, 1, fHP_INC_PER, true );
		cFILE.getflag( "fMP_INC_PER", 1, 1, fMP_INC_PER, true );
		cFILE.getflag( "fSP_INC_PER", 1, 1, fSP_INC_PER, true );		

		cFILE.getflag( "fHP_INC", 1, 1, fHP_INC, true );
		cFILE.getflag( "fMP_INC", 1, 1, fMP_INC, true );
		cFILE.getflag( "fSP_INC", 1, 1, fSP_INC, true );

		cFILE.getflag( "fRESIST_PHYSIC_G", 1, 1, fRESIST_PHYSIC_G, true );
		cFILE.getflag( "fRESIST_G", 1, 1, fRESIST_G, true );
		cFILE.getflag( "fREGIST_MAX_G", 1, 1, fREGIST_MAX_G, true );	
		fREGIST_MAX_G = fREGIST_MAX_G > 0.99f ? 0.99f : fREGIST_MAX_G;
		fREGIST_MAX_G = fREGIST_MAX_G < 0.01f ? 0.01f : fREGIST_MAX_G;

		cFILE.getflag( "fRESIST_MAX_CROW", 1, 1, fRESIST_MAX_CROW, true );	
		fRESIST_MAX_CROW = fRESIST_MAX_CROW > 0.99f ? 0.99f : fRESIST_MAX_CROW;
		fRESIST_MAX_CROW = fRESIST_MAX_CROW < 0.01f ? 0.01f : fRESIST_MAX_CROW;

		cFILE.getflag( "wBASIC_DIS_SP", 1, 1, wBASIC_DIS_SP, true );

		cFILE.getflag( "fLOWSP_MOTION",  1, 1, fLOWSP_MOTION, true );

		cFILE.getflag( "fLOWSP_DAMAGE", 1, 1, fLOWSP_DAMAGE, true );
		cFILE.getflag( "fLOWSP_HIT_DROP", 1, 1, fLOWSP_HIT_DROP, true );		
		cFILE.getflag( "fLOWSP_AVOID_DROP", 1, 1, fLOWSP_AVOID_DROP, true );	
		cFILE.getflag( "dwACCEPT_LOWERSTATS", 1, 1, dwACCEPT_LOWERSTATS, true );

		cFILE.getflag( "fDAMAGE_DEC_RATE", 1, 1, fDAMAGE_DEC_RATE, true );	
		cFILE.getflag( "fDAMAGE_GRADE_K", 1, 1, fDAMAGE_GRADE_K, true );
		cFILE.getflag( "fDAMAGE_DEC_MAX_G", 1, 1, fDAMAGE_DEC_MAX_G, true );
		if ( fDAMAGE_DEC_MAX_G > 1.0f )
			fDAMAGE_DEC_MAX_G = 1.0f;

		cFILE.getflag( "dwCRITICAL_DAMAGE", 1, 1, dwCRITICAL_DAMAGE, true );
		cFILE.getflag( "dwCRITICAL_MAX", 1, 1, dwCRITICAL_MAX, true );

		cFILE.getflag( "dwCRUSHING_BLOW_DAMAGE", 1, 1, dwCRUSHING_BLOW_DAMAGE, true );
		cFILE.getflag( "dwCRUSHING_BLOW_MAX", 1, 1, dwCRUSHING_BLOW_MAX, true );
		cFILE.getflag( "fCRUSH_BLOW_RANGE", 1, 1, fCRUSH_BLOW_RANGE, true );

        cFILE.getflag( "dwReleaseStigmaDistance", 1, 1, dwReleaseStigmaDistance, true);

		cFILE.getflag( "sProductMotionID", 1, 2, sProductMotionID.wMainID, true );
		cFILE.getflag( "sProductMotionID", 2, 2, sProductMotionID.wSubID, true );

		//hsshin 제작 시스템 숙련도시스템 추가로인한 수정
		{
			for ( int i = 0; i < Product::ETypeTotal; ++i )
			{
				cFILE.getflag( "bUnableProductType", 1+i, Product::ETypeTotal, bUnableProductType[i], true );
				cFILE.getflag( "bPublicStudyProductType", 1+i, Product::ETypeTotal, bPublicStudyProductType[i], true );
			}
			cFILE.getflag( "dwStudyPointDefault", 1, 1, dwStudyPointDefault, true );
		}

		cFILE.getflag( "fReferCharDelay", 1, 1, fReferCharDelay, true );

		cFILE.getflag( "sInvenExItemSearch", 1, 1, sInvenExItemSearch, true );
		cFILE.getflag( "wInvenDefaultLine", 1, 1, wInvenDefaultLine );
		wInvenExtendLine = EInvenTotalLine - wInvenDefaultLine - EInvenPremiumLine;
		cFILE.getflag( "wInvenExtendEventLine", 1, 1, wInvenExtendEventLine );

		cFILE.getflag( "fTrash_SellCostPer", 1, 1, fTrash_SellCostPer );
		cFILE.getflag( "nBrightMAX", 1, 1, nBrightMAX );
		cFILE.getflag( "nBrightMIN", 1, 1, nBrightMIN );

		cFILE.getflag( "wTrash_Cost", 1, 1, wTrash_Cost );
		cFILE.getflag( "wTrash_Point", 1, 1, wTrash_Point );

		cFILE.getflag( "fCONFRONT_TIME", 1, 1, fCONFRONT_TIME, true );
		cFILE.getflag( "fCONFRONT_ELAP", 1, 1, fCONFRONT_ELAP, true );		
		cFILE.getflag( "dwCONFRONT_MAX", 1, 1, dwCONFRONT_MAX, true );		
		cFILE.getflag( "fCONFRONT_RESET", 1, 1, fCONFRONT_RESET, true );		
		cFILE.getflag( "fCONFRONT_ONE_DIST", 1, 1, fCONFRONT_ONE_DIST, true );	
		cFILE.getflag( "fCONFRONT_PY_DIST", 1, 1, fCONFRONT_PY_DIST, true );	
		cFILE.getflag( "fCONFRONT_CLB_DIST", 1, 1, fCONFRONT_CLB_DIST, true );	

		cFILE.getflag( "nCONFRONT_WIN_LP", 1, 1, nCONFRONT_WIN_LP, true );	
		cFILE.getflag( "nCONFRONT_LOSS_LP", 1, 1, nCONFRONT_LOSS_LP, true );	

		cFILE.getflag( "nCONFRONT_CLB_WIN_LP", 1, 1, nCONFRONT_CLB_WIN_LP, true );	
		cFILE.getflag( "nCONFRONT_CLB_LOSS_LP", 1, 1, nCONFRONT_CLB_LOSS_LP, true ); // Note : 설정하지 않는 flag.	

		cFILE.getflag( "wCONFRONT_SCHOOL_LIMIT_NUM", 1, 1, wCONFRONT_SCHOOL_LIMIT_NUM, true );
		cFILE.getflag( "wCONFRONT_SCHOOL_LIMIT_LEVEL", 1, 1, wCONFRONT_SCHOOL_LIMIT_LEVEL, true );
		cFILE.getflag( "fCONFRONT_SCHOOL_EXP_SCALE", 1, 1, fCONFRONT_SCHOOL_EXP_SCALE, true );

		cFILE.getflag( "wCONFRONT_SCHOOL_WIN_NUM", 1, 1, wCONFRONT_SCHOOL_WIN_NUM, true );
		cFILE.getflag( "wCONFRONT_SCHOOL_WIN_RATE", 1, 1, wCONFRONT_SCHOOL_WIN_RATE, true );	
		cFILE.getflag( "fCONFRONT_SCHOOL_GENITEM", 1, 1, fCONFRONT_SCHOOL_GENITEM, true );

		for (int i=0; i<EMSTATEBLOW_LEVEL_SIZE; ++i )
			cFILE.getflag( "nSTATEBLOW_LEVEL", 1+i, EMSTATEBLOW_LEVEL_SIZE, nSTATEBLOW_LEVEL[i], true );

		for (int i=0; i<EMCONFT_RC_TYPENUM; ++i )
			cFILE.getflag( "wCONFT_RC_TYPE", 1+i, EMCONFT_RC_TYPENUM, wCONFT_RC_TYPE[i], true );

		for (int i=0; i<EMCONFT_STATE_TYPENUM; ++i )
			cFILE.getflag( "wCONFT_STATE_TYPE", 1+i, EMCONFT_STATE_TYPENUM, fCONFT_STATE_TYPE[i], true );

		cFILE.getflag( "fDPS_TurnToPeaceModeInterval", 1, 1, fDPS_TurnToPeaceModeInterval, true );

		cFILE.getflag( "fPARTYEXPRANGE", 1, 1, fPARTYEXPRANGE, true );
		cFILE.getflag( "fPARTYEXP_S", 1, 1, fPARTYEXP_S, true );

		cFILE.getflag( "fPartyTenderTime", 1, 1, fPartyTenderTime, true );
		cFILE.getflag( "fExpeditionTenderTime", 1, 1, fExpeditionTenderTime, true );
		cFILE.getflag( "fPartyTenderRange", 1, 1, fPartyTenderRange, true );
		cFILE.getflag( "fExpeditionTenderRange", 1, 1, fExpeditionTenderRange, true );
		cFILE.getflag( "fExpeditionDecreaseExpRate", 1, 1, fExpeditionDecreaseExpRate, true );
		cFILE.getflag( "fSwapSlotBlankInterval", 1, 1, fSwapSlotBlankInterval, true );
		cFILE.getflag( "dwExpeditionCheckReadyTime", 1, 1, dwExpeditionCheckReadyTime, true );
		
		fUPartyTenderTime = 1.0f / fPartyTenderTime;
		fUExpeditionTenderTime = 1.0f / fExpeditionTenderTime;

		for (int i=0; i<MAXPARTY; ++i )
			cFILE.getflag( "aPARTYEXP_ONE", 1+i, MAXPARTY, aPARTYEXP_ONE[i], true );

		for (int i=0; i<MAXPARTY; ++i )
			cFILE.getflag( "aPARTYKILLEXP_ONE", 1+i, MAXPARTY, aPARTYKILLEXP_ONE[i], true );

		cFILE.getflag( "dwCLUB_PARTYNUM", 1, 1, dwCLUB_PARTYNUM, true );
		cFILE.getflag( "dwCLUB_DISSOLUTION_DAY", 1, 1, dwCLUB_DISSOLUTION_DAY, true );
		cFILE.getflag( "dwCLUB_JOINBLOCK_DAY", 1, 1, dwCLUB_JOINBLOCK_DAY, true );
		cFILE.getflag( "dwCLUB_AUTHORITY", 1, 1, dwCLUB_AUTHORITY, true );		
		cFILE.getflag( "dwCLUB_ALLIANCE_SEC", 1, 1, dwCLUB_ALLIANCE_SEC, true );
		cFILE.getflag( "dwCLUB_ALLIANCE_DIS", 1, 1, dwCLUB_ALLIANCE_DIS, true );
		cFILE.getflag( "dwCLUB_ALLIANCE_NUM", 1, 1, dwCLUB_ALLIANCE_NUM, true );


		cFILE.getflag( "bCLUB_BATTLE", 1, 1, bCLUB_BATTLE, true );
		cFILE.getflag( "dwCLUB_BATTLE_MAX", 1, 1, dwCLUB_BATTLE_MAX, true );
		cFILE.getflag( "dwCLUB_BATTLE_RANK", 1, 1, dwCLUB_BATTLE_RANK, true );
		cFILE.getflag( "dwCLUB_BATTLE_TIME", 1, 1, dwCLUB_BATTLE_TIME, true ); // Note : 설정하지 않는 flag.
		cFILE.getflag( "dwCLUB_BATTLE_TIMEMIN", 1, 1, dwCLUB_BATTLE_TIMEMIN, true );
		cFILE.getflag( "dwCLUB_BATTLE_TIMEMAX", 1, 1, dwCLUB_BATTLE_TIMEMAX, true );
		cFILE.getflag( "dwCLUB_BATTLE_GUID_TIME", 1, 1, dwCLUB_BATTLE_GUID_TIME, true );
		cFILE.getflag( "dwCLUB_BATTLE_DIS_TIME", 1, 1, dwCLUB_BATTLE_DIS_TIME, true );
		cFILE.getflag( "bCLUB_BATTLE_ALLIANCE", 1, 1, bCLUB_BATTLE_ALLIANCE, true ); // Note : 설정하지 않는 flag.

		cFILE.getflag( "bCLUB_DEATHMATCH", 1, 1, bCLUB_DEATHMATCH, true );
		cFILE.getflag( "dwCLUB_DEATHMATCH_MEM", 1, 1, dwCLUB_DEATHMATCH_MEM, true );		
		cFILE.getflag( "dwCLUB_DEATHMATCH_RANK", 1, 1, dwCLUB_DEATHMATCH_RANK, true );

        cFILE.getflag( "dwCLUB_CHANGE_CLUB_MARK_LIMIT_TIME", 1, 1, dwCLUB_CHANGE_CLUB_MARK_LIMIT_TIME, true );

		// 클럽 마크 RGB;
		for ( int i = 0; i < 4; ++i )
		{
			DWORD color;
			cFILE.getflag( "dwClubMarkARGB", i + 1, 4, color, true );
			vecClubMarkARGB.push_back( color );
		}

		if( vecClubMarkARGB.size() > 3 )
		{
			dwClubMarkARGB = D3DCOLOR_ARGB(
				vecClubMarkARGB.at( 0 ),
				vecClubMarkARGB.at( 1 ),
				vecClubMarkARGB.at( 2 ),
				vecClubMarkARGB.at( 3 ) );
		}

		cFILE.getflag( "dwMAX_CLUBRANK", 1, 1, dwMAX_CLUBRANK, true );

		if (dwMAX_CLUBRANK < MAX_CLUBSTORAGE)
            dwMAX_CLUBRANK = MAX_CLUBSTORAGE;
		if (dwMAX_CLUBRANK > MAX_CLUBRANK)
            dwMAX_CLUBRANK = MAX_CLUBRANK;

		CString strTemp;
		for ( DWORD i = 0; i < dwMAX_CLUBRANK; ++i )
		{
            if (i >= MAX_CLUBRANK)
            {                
                std::string ErrMsg(
                    sc::string::format(
                        "GLCONST_CHAR::LOADFILE dwMAX_CLUBRANK overflow. %1%", i));
                sc::ErrorMessageBox(ErrMsg, false);
            }
            else
            {
			    strTemp.Format("GLCLUBRANK_%d", i+1 );

			    cFILE.getflag( strTemp.GetString(), 1, 4, sCLUBRANK[i].m_dwMasterLvl, true );
			    cFILE.getflag( strTemp.GetString(), 2, 4, sCLUBRANK[i].m_dwLivingPoint, true );
			    cFILE.getflag( strTemp.GetString(), 3, 4, sCLUBRANK[i].m_dwPay, true );
			    cFILE.getflag( strTemp.GetString(), 4, 4, sCLUBRANK[i].m_dwMember, true );
            }
		}

		cFILE.getflag( "fMAX_COMMISSION",	1, 1, fMAX_COMMISSION, true );
		cFILE.getflag( "fDEFAULT_COMMISSION",	1, 1, fDEFAULT_COMMISSION, true );
		cFILE.getflag( "fEARNING_RATE",	1, 1, fEARNING_RATE, true );
		cFILE.getflag( "fEARNING_RATE_NPC",	1, 1, fEARNING_RATE_NPC, true );

		cFILE.getflag( "fCDCERTIFY_DIST",	1, 1, fCDCERTIFY_DIST, true );
		cFILE.getflag( "fCDCERTIFY_TIME",	1, 1, fCDCERTIFY_TIME, true );

		cFILE.getflag( "fEXP_SCALE",	1, 1, fEXP_SCALE, true );
		cFILE.getflag( "fITEM_DROP_SCALE",	1, 1, fITEM_DROP_SCALE, true );
		cFILE.getflag( "fMONEY_DROP_SCALE",	1, 1, fMONEY_DROP_SCALE, true );
		cFILE.getflag( "fITEM_DROP_LOWER_SCALE", 1, 1, fITEM_DROP_LOWER_SCALE, true );

		cFILE.getflag( "fITEM_DROP_SPECIFIC_GRADE_SCALE", 1, 1, fITEM_DROP_SPECIFIC_GRADE_SCALE, true );
		cFILE.getflag( "wITEM_DROP_SPECIFIC_GRADE", 1, 1, wITEM_DROP_SPECIFIC_GRADE, true );

		cFILE.getflag( "bPK_MODE",	1, 1, bPK_MODE, true );
		cFILE.getflag( "nPK_LIMIT_LEVEL",	1, 1, nPK_LIMIT_LEVEL, true );
		cFILE.getflag( "nPK_LIMIT_DX_LEVEL",	1, 1, nPK_LIMIT_DX_LEVEL, true );

		cFILE.getflag( "nPK_TRY_BRIGHT_POINT",	1, 1, nPK_TRY_BRIGHT_POINT, true );
		cFILE.getflag( "nPK_KILL_BRIGHT_POINT",	1, 1, nPK_KILL_BRIGHT_POINT, true );
		cFILE.getflag( "nPK_TRY_LIVING_POINT",	1, 1, nPK_TRY_LIVING_POINT, true );
		cFILE.getflag( "nPK_KILL_LIVING_POINT",	1, 1, nPK_KILL_LIVING_POINT, true );

		cFILE.getflag( "fNONPK_ITEM_DROP",	1, 1, fNONPK_ITEM_DROP, true );
		cFILE.getflag( "fPK_ITEM_DROP",	1, 1, fPK_ITEM_DROP, true );

		cFILE.getflag( "dwPK_RECALL_ENABLE_LEVEL",	1, 1, dwPK_RECALL_ENABLE_LEVEL, true );
		cFILE.getflag( "dwPK_DRUG_ENABLE_LEVEL",	1, 1, dwPK_DRUG_ENABLE_LEVEL, true );

		cFILE.getflag( "fPK_JUSTNESS_TIME",	1, 1, fPK_JUSTNESS_TIME, true );
		cFILE.getflag( "fPK_SAFE_TIME",	1, 1, fPK_SAFE_TIME, true ); // Note : 설정하지 않는 flag.

		for (int i=0; i<CDM_ACCRUE_COUNT; ++i )
			cFILE.getflag( "fCDM_SAFE_TIME_ACCRUE", 1+i, CDM_ACCRUE_COUNT, fCDM_SAFE_TIME_ACCRUE[i], true );


		cFILE.getflag( "fCDM_SAFE_TIME_MOVE_SPEED",	1, 1, fCDM_SAFE_TIME_MOVE_SPEED, true );		

		BYTE cA, cR, cG, cB;

		cFILE.getflag( "PK_NORMAL_NAME_COLOR", 1, 4, cA, true );
		cFILE.getflag( "PK_NORMAL_NAME_COLOR", 2, 4, cR, true );
		cFILE.getflag( "PK_NORMAL_NAME_COLOR", 3, 4, cG, true );
		cFILE.getflag( "PK_NORMAL_NAME_COLOR", 4, 4, cB, true );
		dwPK_NORMAL_NAME_COLOR = D3DCOLOR_RGBA(cR,cG,cB,cA);

		cFILE.getflag( "fPK_POINT_DEC_RATE",	1, 1, fPK_POINT_DEC_RATE, true );
		cFILE.getflag( "fPK_POINT_DEC_PHY",	1, 1, fPK_POINT_DEC_PHY, true );
		cFILE.getflag( "fPK_POINT_DEC_MAGIC",	1, 1, fPK_POINT_DEC_MAGIC, true ); // Note : 설정하지 않는 flag.

		cFILE.getflag( "fReductionRate_PVP_PHY_P_Damage", 1, 1, fReductionRate_PVP_PHY_P_Damage, true );
		cFILE.getflag( "fReductionRate_PVP_PHY_S_Damage", 1, 1, fReductionRate_PVP_PHY_S_Damage, true );
		cFILE.getflag( "fReductionRate_PVP_MAGIC_Damage", 1, 1, fReductionRate_PVP_MAGIC_Damage, true );
		cFILE.getflag( "fReductionRate_PVP_PHY_Defense", 1, 1, fReductionRate_PVP_PHY_Defense, true );
		cFILE.getflag( "fReductionRate_PVP_MAGIC_Resistance", 1, 1, fReductionRate_PVP_MAGIC_Resistance, true );

		for (int i=0; i<EMPK_STATE_LEVEL; ++i )
		{
			SPLAYERKILL sPK;
			cFILE.getflag ( i, "SPLAYERKILL", 1, 12, sPK.dwLEVEL, true );

			cFILE.getflag ( i, "SPLAYERKILL", 2, 12, cA, true );
			cFILE.getflag ( i, "SPLAYERKILL", 3, 12, cR, true );
			cFILE.getflag ( i, "SPLAYERKILL", 4, 12, cG, true );
			cFILE.getflag ( i, "SPLAYERKILL", 5, 12, cB, true );
			sPK.dwNAME_COLOR = D3DCOLOR_RGBA(cR,cG,cB,cA);

			cFILE.getflag ( i, "SPLAYERKILL", 6, 12, sPK.nPKPOINT, true );

			cFILE.getflag ( i, "SPLAYERKILL", 7, 12, sPK.strNAME, true );
			cFILE.getflag ( i, "SPLAYERKILL", 8, 12, sPK.fPK_EXP_RATE, true );
			cFILE.getflag ( i, "SPLAYERKILL", 9, 12, sPK.dwITEM_DROP_NUM, true );
			cFILE.getflag ( i, "SPLAYERKILL", 10, 12, sPK.fITEM_DROP_RATE, true );

			cFILE.getflag ( i, "SPLAYERKILL", 11, 12, sPK.fSHOP_2BUY_RATE, true );
			cFILE.getflag ( i, "SPLAYERKILL", 12, 12, sPK.fSHOP_2SALE_RATE, true );

			sPK_STATE[i] = sPK;
		}

        // non pk 채널 접속가능 최하 성향점수
        cFILE.getflag("NON_PK_CONNECT_BRIGHT_POINT", 1, 1, nNON_PK_CONNECT_BRIGHT_POINT, true);

		//	Note : ----------------------------------------------------------------[school pk event use]
		cFILE.getflag( "bSCHOOL_FREE_PK_ALWAYS", 1, 1, bSCHOOL_FREE_PK_ALWAYS, true );
		cFILE.getflag( "bSCHOOL_FREE_PK", 1, 1, bSCHOOL_FREE_PK, true );
		cFILE.getflag( "bSCHOOL_FREE_PK_TIME_REAL", 1, 1, bSCHOOL_FREE_PK_TIME_REAL, true );
		cFILE.getflag( "bSCHOOL_FREE_PK_Z_FORCED", 1, 1, bSCHOOL_FREE_PK_Z_FORCED, true );
		cFILE.getflag( "dwSCHOOL_FREE_PK_TIME_START", 1, 1, dwSCHOOL_FREE_PK_TIME_START, true );
		cFILE.getflag( "dwSCHOOL_FREE_PK_TIME_TERM", 1, 1, dwSCHOOL_FREE_PK_TIME_TERM, true );
		cFILE.getflag( "fSCHOOL_FREE_PK_ITEM_DROP", 1, 1, fSCHOOL_FREE_PK_ITEM_DROP, true );

		//	Note : ---------------------------------------------------------------- 채팅 관련
		cFILE.getflag( "bCHAT_EXTEND", 1, 1, bCHAT_EXTEND, true );
		cFILE.getflag( "dwCHAT_AREA_CHARGE", 1, 1, dwCHAT_AREA_CHARGE, true );
		cFILE.getflag( "dwCHAT_PARTY_RECRUIT_CHARGE", 1, 1, dwCHAT_PARTY_RECRUIT_CHARGE, true );
		cFILE.getflag( "fCHAT_PARTY_RECRUIT_TIME", 1, 1, fCHAT_PARTY_RECRUIT_TIME, true );

		//	Note : ----------------------------------------------------------------
		//cFILE.getflag( "bENCHANT_TERMINATE_ITEM", 1, 1, bENCHANT_TERMINATE_ITEM, true );


		//	Note : ----------------------------------------------------------------기능 설정.
		cFILE.getflag( "bMONEY_DROP2FIELD", 1, 1, bMONEY_DROP2FIELD, true );

		//	Note : ----------------------------------------------------------------연마 확율.
		//for ( int i=0; i<GRADE_LIMIT_MAX; ++i )
		//{
		//	cFILE.getflag( "fGRADE_RATE",						 1+i, GRADE_LIMIT_MAX, fGRADE_RATE[i], true );
		//	cFILE.getflag( "fGRADE_RESET_RATE",					 1+i, GRADE_LIMIT_MAX, fGRADE_RESET_RATE[i], true );
		//	cFILE.getflag( "fGRADE_TERMINATE_RATE",				 1+i, GRADE_LIMIT_MAX, fGRADE_TERMINATE_RATE[i], true );
		//	cFILE.getflag( "fGRADE_ITEMOPTION_INC_GRINDINGRATE", 1+i, GRADE_LIMIT_MAX, fGRADE_ITEMOPTION_INC_GRINDINGRATE[i], true );
		//	cFILE.getflag( "nGRADE_ITEMOPTION_ANTIRESET",		 1+i, GRADE_LIMIT_MAX, nGRADE_ITEMOPTION_ANTIRESET[i], true );
		//}

		//	Note : ----------------------------------------------------------------사운드
		cFILE.getflag( "strGRINDING_SUCCEED", 1, 1, strGRINDING_SUCCEED, true );
		cFILE.getflag( "strGRINDING_FAIL", 1, 1, strGRINDING_FAIL, true );
		cFILE.getflag( "strGRINDING_RESET", 1, 1, strGRINDING_RESET, true );
		cFILE.getflag( "strGRINDING_BROKEN", 1, 1, strGRINDING_BROKEN, true );

		cFILE.getflag( "strGAMBLING_SHUFFLE", 1, 1, strGAMBLING_SHUFFLE );  // Monster7j
		cFILE.getflag( "strGAMBLING_WIN", 1, 1, strGAMBLING_WIN );
		cFILE.getflag( "strGAMBLING_LOSE", 1, 1, strGAMBLING_LOSE );
		cFILE.getflag( "strDICE_ROLL", 1, 1, strDICE_ROLL );		

		cFILE.getflag( "strITEMDROP_SUIT", 1, 1, strITEMDROP_SUIT, true );
		cFILE.getflag( "strITEMDROP_WAPON", 1, 1, strITEMDROP_WAPON, true );
		cFILE.getflag( "strITEMDROP_SHOES", 1, 1, strITEMDROP_SHOES, true );
		cFILE.getflag( "strITEMDROP_RING", 1, 1, strITEMDROP_RING, true );
		cFILE.getflag( "strITEMDROP_QBOX", 1, 1, strITEMDROP_QBOX, true );

		cFILE.getflag( "strITEMDROP_SCROLL", 1, 1, strITEMDROP_SCROLL, true );
		cFILE.getflag( "strITEMDROP_COIN", 1, 1, strITEMDROP_COIN, true );
		cFILE.getflag( "strITEMDROP_DRUGS", 1, 1, strITEMDROP_DRUGS, true );

		cFILE.getflag( "strPICKUP_ITEM", 1, 1, strPICKUP_ITEM, true );
		cFILE.getflag( "strQITEM_FACT", 1, 1, strQITEM_FACT, true );

		for ( int i=0; i<QUESTION_SIZE; ++i )
		{
			cFILE.getflag( "strQITEM_BGM", 1+i, QUESTION_SIZE, strQITEM_BGM[i], true );
		}

		cFILE.getflag( "strQITEM_FACT", 1, 1, strQITEM_FACT, true );

		for ( int i = 0; i < EMITEM_LEVEL_NSIZE; i++ )
		{
			std::string strItemRank = sc::string::format("strITEMDROP_RANK%d", i);
			cFILE.getflag( strItemRank, 1, 1, strITEMDROP_RANK[i], true );
		}


		//	Note : ----------------------------------------------------------------효과
		cFILE.getflag( "strSELECT_CHAR", 1, 1, strSELECT_CHAR, true );

		cFILE.getflag( "strREBIRTH_EFFECT", 1, 1, strREBIRTH_EFFECT, true );
		cFILE.getflag( "strLEVELUP_EFFECT", 1, 1, strLEVELUP_EFFECT, true );

		for ( int i = 0; i < EMELEMENT_MAXNUM; ++i )
			cFILE.getflag( "strSRIKE_ELMT_EFFECT", i+1 , EMELEMENT_MAXNUM , strSRIKE_ELMT_EFFECT[i], true );

		cFILE.getflag( "strSTRIKE_ARROW_EFFECT", 1, 1, strSTRIKE_ARROW_EFFECT, true );
		cFILE.getflag( "strSTRIKE_BULLET_EFFECT", 1, 1, strSTRIKE_BULLET_EFFECT, true );

		cFILE.getflag( "strAMBIENT_EFFECT",			1, 1, strAMBIENT_EFFECT, true );
		cFILE.getflag( "strERASE_EFFECT",			1, 1, strERASE_EFFECT, true );

		cFILE.getflag( "strERASE_SUMMON_EFFECT",	1, 1, strERASE_SUMMON_EFFECT, true );
		cFILE.getflag( "fERASE_SUMMON_BEGIN_TIME",	1, 1, fERASE_SUMMON_BEGIN_TIME, true );

		cFILE.getflag( "strSKILL_LEARN_EFFECT",	1, 1, strSKILL_LEARN_EFFECT, true );		
		cFILE.getflag( "strSKILL_UP_EFFECT",	1, 1, strSKILL_UP_EFFECT, true );			

		cFILE.getflag( "strQUEST_START", 1, 1, strQUEST_START, true );				
		cFILE.getflag( "strQUEST_END_SUCCEED", 1, 1, strQUEST_END_SUCCEED, true );		
		cFILE.getflag( "strQUEST_END_FAIL", 1, 1, strQUEST_END_FAIL, true );			

		cFILE.getflag( "strMOB_GEM_EFFECT",	1, 1, strMOB_GEM_EFFECT, true );
		cFILE.getflag( "strMOB_DIE_BLOOD",	1, 1, strMOB_DIE_BLOOD, true );

		cFILE.getflag( "strPET_GEN_EFFECT",	1, 1, strPET_GEN_EFFECT, true );

		cFILE.getflag( "strVEHICLE_GEN_EFFECT",	1, 1, strVEHICLE_GEN_EFFECT, true );

		cFILE.getflag( "strCONFRONT_BOUND",	1, 1, strCONFRONT_BOUND, true );			
		cFILE.getflag( "strHALFALPHA_EFFECT",	1, 1, strHALFALPHA_EFFECT, true );			

		cFILE.getflag( "strBLOW_NUMB_EFFECT",	1, 1, strBLOW_NUMB_EFFECT, true );			
		cFILE.getflag( "strBLOW_STUN_EFFECT",	1, 1, strBLOW_STUN_EFFECT, true );			
		cFILE.getflag( "strBLOW_STONE_EFFECT",	1, 1, strBLOW_STONE_EFFECT, true );		
		cFILE.getflag( "strBLOW_BURN_EFFECT",	1, 1, strBLOW_BURN_EFFECT, true );			
		cFILE.getflag( "strBLOW_FROZEN_EFFECT",	1, 1, strBLOW_FROZEN_EFFECT, true );		

		cFILE.getflag( "strBLOW_MAD_EFFECT",	1, 1, strBLOW_MAD_EFFECT, true );			
		cFILE.getflag( "strBLOW_POISON_EFFECT",	1, 1, strBLOW_POISON_EFFECT, true );		
		cFILE.getflag( "strBLOW_CURSE_EFFECT",	1, 1, strBLOW_CURSE_EFFECT, true );		

		cFILE.getflag( "strBLOW_BODY_NUMB_EFFECT",	1, 1, strBLOW_BODY_NUMB_EFFECT, true );	
		cFILE.getflag( "strBLOW_BODY_STUN_EFFECT",	1, 1, strBLOW_BODY_STUN_EFFECT, true );	
		cFILE.getflag( "strBLOW_BODY_STONE_EFFECT",	1, 1, strBLOW_BODY_STONE_EFFECT, true );	
		cFILE.getflag( "strBLOW_BODY_BURN_EFFECT",	1, 1, strBLOW_BODY_BURN_EFFECT, true );	
		cFILE.getflag( "strBLOW_BODY_FROZEN_EFFECT",	1, 1, strBLOW_BODY_FROZEN_EFFECT, true );	

		cFILE.getflag( "strBLOW_BODY_MAD_EFFECT",	1, 1, strBLOW_BODY_MAD_EFFECT, true );		
		cFILE.getflag( "strBLOW_BODY_POISON_EFFECT",	1, 1, strBLOW_BODY_POISON_EFFECT, true );	
		cFILE.getflag( "strBLOW_BODY_CURSE_EFFECT",	1, 1, strBLOW_BODY_CURSE_EFFECT, true );	

		cFILE.getflag( "strCRUSHING_BLOW_EFFECT",	1, 1, strCRUSHING_BLOW_EFFECT, true );

		cFILE.getflag( "strCOMPLETE_NPC_EFFECT", 1, 1, strCOMPLETE_NPC_EFFECT, true );

        cFILE.getflag( "strCRUSHING_BLOW_EFFECT", 1, 1, strCRUSHING_BLOW_EFFECT, true );

        cFILE.getflag( "strAUTH_EFFECT", 1, 1, strAUTH_EFFECT, true );
        cFILE.getflag( "strAUTH_SM_EFFECT", 1, 1, strAUTH_SM_EFFECT, true );
        cFILE.getflag( "strAUTH_HA_EFFECT", 1, 1, strAUTH_HA_EFFECT, true );
        cFILE.getflag( "strAUTH_BH_EFFECT", 1, 1, strAUTH_BH_EFFECT, true );

		cFILE.getflag( "strCLICK_EFFECT", 1, 1, strCLICK_EFFECT, true );
		cFILE.getflag( "strMINE_EFFECT", 1, 1, strMINE_EFFECT, true );
		cFILE.getflag( "strCURSOR_TARGET_EFFECT", 1, 1, strCURSOR_TARGET_EFFECT, true );
		cFILE.getflag( "strTARGET_EFFECT", 1, 1, strTARGET_EFFECT, true );
		cFILE.getflag( "strTARGET_EFFECT_ENEMY", 1, 1, strTARGET_EFFECT_ENEMY, true );
		cFILE.getflag( "strTARGET_EFFECT_NPC", 1, 1, strTARGET_EFFECT_NPC, true );
		cFILE.getflag( "strTARGET_EFFECT_ALLIANCE", 1, 1, strTARGET_EFFECT_ALLIANCE, true );
		cFILE.getflag( "strARROW_EFFECT_ENEMY", 1, 1, strARROW_EFFECT_ENEMY, true );
		cFILE.getflag( "strARROW_EFFECT_NPC", 1, 1, strARROW_EFFECT_NPC, true );
		cFILE.getflag( "strARROW_EFFECT_ALLIANCE", 1, 1, strARROW_EFFECT_ALLIANCE, true );
		cFILE.getflag( "strSCOPE_TEXTURE_CIRCLE", 1, 1, strSCOPE_TEXTURE_CIRCLE, true );
		cFILE.getflag( "strSCOPE_TEXTURE_FANWISE", 1, 1, strSCOPE_TEXTURE_FANWISE, true );

        cFILE.getflag( "strCOMPLATE_ACTIVITY_EFFECT", 1, 1, strCOMPLATE_ACTIVITY_EFFECT, true );

		cFILE.getflag( "strCA_EFFECT",	1, 1, strCA_EFFECT, true );	

		cFILE.getflag( "nPK_KILL_Combat_Point", 1, 1, wPK_KILL_Combat_Point, true );
		cFILE.getflag( "bCOMBAT_POINT", 1, 1, bCOMBAT_POINT, true );
		cFILE.getflag( "fIllusionInterval", 1, 1, fIllusionInterval, true );
    
        // 내구도
        for ( int i = 0; i < SUIT_NSIZE; ++i )
            cFILE.getflag( "bDurabilityEmsuit", i+1, SUIT_NSIZE, bDurabilityEmsuit[i], true );
        cFILE.getflag( "wDurabilityDropDeathPvE", 1, 1, wDurabilityDropDeathPvE, true );
        cFILE.getflag( "wDurabilityDropDeathPvP", 1, 1, wDurabilityDropDeathPvP, true );

        cFILE.getflag( "dwDurabilityCountLimit", 1, 1, dwDurabilityCountLimit, true );
        cFILE.getflag( "dwDurabilityDropSkillUse", 1, 1, dwDurabilityDropSkillUse, true );
        cFILE.getflag( "dwDurabilityDropHitPvE", 1, 1, dwDurabilityDropHitPvE, true );
        cFILE.getflag( "dwDurabilityDropHitPvP", 1, 1, dwDurabilityDropHitPvP, true );
        
        
        for ( int i = 0; i < DURABILITY_RATE_COUNT; ++i )
            cFILE.getflag( "nDurabilityRateTable", i+1, DURABILITY_RATE_COUNT, nDurabilityRateTable[i], true );
        for ( int i = 0; i < DURABILITY_RATE_COUNT; ++i )
            cFILE.getflag( "nDurabilityRatePerformance", i+1, DURABILITY_RATE_COUNT, nDurabilityRatePerformance[i], true );

		for ( int i = 0; i < DURABILITY_RATE_COUNT; ++i )
		{
			cFILE.getflag( "nWPowDurRatePerformance", i+1, DURABILITY_RATE_COUNT, nWPowDurRatePerformance[i], true );
			cFILE.getflag( "nWDexDurRatePerformance", i+1, DURABILITY_RATE_COUNT, nWDexDurRatePerformance[i], true );
			cFILE.getflag( "nWSpiDurRatePerformance", i+1, DURABILITY_RATE_COUNT, nWSpiDurRatePerformance[i], true );
			cFILE.getflag( "nWMixDurRatePerformance", i+1, DURABILITY_RATE_COUNT, nWMixDurRatePerformance[i], true );
		}

//#ifdef _RELEASED
		for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
			cFILE.getflag( "nDurabilityApply", i+1, DURABILITY_APPLY_COUNT, nDurabilityApply[i], true );

		// 일정 비율 이하시 퀵메뉴에 알리기위한 부스트비율 설정, 퀵메뉴에서 알린다;
		cFILE.getflag( "fDurabilityAlarmRemainPer", 1, 1, fDurabilityAlarmRemainPer, true );
//#endif

		// 점령전중 AFK 시 알림메시지 시간
		DWORD nCTFNotifyCount =0;
		cFILE.getflag("nCTFNotifyCount", 1, 1, nCTFNotifyCount, true);
		for ( DWORD i = 0; i < nCTFNotifyCount; ++i )
		{
			DWORD nCTFNotifyvalue = 0;
			cFILE.getflag("nCTFNotify", i+1, nCTFNotifyCount, nCTFNotifyvalue, true);
			vecCTFNotify.push_back(nCTFNotifyvalue);
		}

		std::sort(vecCTFNotify.begin(),vecCTFNotify.end());

		// 점령전 AFK 하는 사람 팅구는 시간
		cFILE.getflag("fCTF_BECOME_AWAY_TIME",1,1,fCTF_BECOME_AWAY_TIME,true);

		// 프리미엄 맵 강제퇴장 체크 시간 (초)
		cFILE.getflag( "fPremiumMapExitCheckTime", 1, 1, fPremiumMapExitCheckTime, true );

		// 프리미엄 맵 강제퇴장 남은 시간 알림		
		size_t nPremiumMapForcedExitCnt = 0;
		cFILE.getflag( "nPremiumMapForcedExitNotifyCount", 1, 1, nPremiumMapForcedExitCnt, true );
		for( size_t i = 0; i < nPremiumMapForcedExitCnt; ++i )
		{
			DWORD dwTime = 0;
			cFILE.getflag( "nPremiumMapForcedExitNotify", i + 1, nPremiumMapForcedExitCnt, dwTime, true );
			vecPremiumMapForcedExitNotify.push_back( dwTime );
		}

		cFILE.getflag( "bAdditional_ability", 1, 1, bAdditional_ability, true );

		strBLOW_EFFECTS[EMBLOW_NUMB] = GLCONST_CHAR::strBLOW_NUMB_EFFECT;
		strBLOW_EFFECTS[EMBLOW_STUN] = GLCONST_CHAR::strBLOW_STUN_EFFECT;
		strBLOW_EFFECTS[EMBLOW_STONE] = GLCONST_CHAR::strBLOW_STONE_EFFECT;
		strBLOW_EFFECTS[EMBLOW_BURN] = GLCONST_CHAR::strBLOW_BURN_EFFECT;
		strBLOW_EFFECTS[EMBLOW_FROZEN] = GLCONST_CHAR::strBLOW_FROZEN_EFFECT;

		strBLOW_EFFECTS[EMBLOW_MAD] = GLCONST_CHAR::strBLOW_MAD_EFFECT;
		strBLOW_EFFECTS[EMBLOW_POISON] = GLCONST_CHAR::strBLOW_POISON_EFFECT;
		strBLOW_EFFECTS[EMBLOW_CURSE] = GLCONST_CHAR::strBLOW_CURSE_EFFECT;

		strBLOW_BODY_EFFECTS[EMBLOW_NUMB] = GLCONST_CHAR::strBLOW_BODY_NUMB_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_STUN] = GLCONST_CHAR::strBLOW_BODY_STUN_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_STONE] = GLCONST_CHAR::strBLOW_BODY_STONE_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_BURN] = GLCONST_CHAR::strBLOW_BODY_BURN_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_FROZEN] = GLCONST_CHAR::strBLOW_BODY_FROZEN_EFFECT;

		strBLOW_BODY_EFFECTS[EMBLOW_MAD] = GLCONST_CHAR::strBLOW_BODY_MAD_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_POISON] = GLCONST_CHAR::strBLOW_BODY_POISON_EFFECT;
		strBLOW_BODY_EFFECTS[EMBLOW_CURSE] = GLCONST_CHAR::strBLOW_BODY_CURSE_EFFECT;

		std::string strGUIDANCE_FILE;
		int nNUM = (int) cFILE.getflagnum ( "GUIDANCE_FILE" );
		for ( int i=0; i<nNUM; ++i )
		{
			cFILE.getflag ( i, "GUIDANCE_FILE",	1, 1, strGUIDANCE_FILE, true );
			vecGUIDANCE_FILE.push_back ( strGUIDANCE_FILE );
		}

		//
		//mjeon
		//default.charclass에 CLUB_DM_FILE이란 필드명으로 ClubDeathMatch에 대한 정보가 기록된
		//  파일명이 저장되어 있다. -> 여러개의 파일이 등록될 수 있다.
		//
		std::string strClubDM_FILE;
		nNUM = (int) cFILE.getflagnum ( "CLUB_DM_FILE" );	//사용중인 파일명: CDM_01.ini
		for ( int i=0; i<nNUM; ++i )
		{
			cFILE.getflag ( i, "CLUB_DM_FILE",	1, 1, strClubDM_FILE, true );
			vecClubDM_FILE.push_back ( strClubDM_FILE );
		}

		const char* szSETFILE[GLCI_NUM_ACTOR] = 
		{
			"FIGHTER_M.SETFILE",
			"ARMS_M.SETFILE",
			"ARCHER_W.SETFILE",
			"SPIRIT_W.SETFILE",
			"EXTREME_M.SETFILE",
			"EXTREME_W.SETFILE",
			"FIGHTER_W.SETFILE",
			"ARMS_W.SETFILE",
			"ARCHER_M.SETFILE",
			"SPIRIT_M.SETFILE",
			"SCIENTIST_M.SETFILE",
			"SCIENTIST_W.SETFILE",
            "ASSASSIN_M.SETFILE",
            "ASSASSIN_W.SETFILE",
			"TRICKER_M.SETFILE",
			"TRICKER_W.SETFILE",
            "ETC_M.SETFILE",
            "ETC_W.SETFILE",
			"ACTOR_M.SETFILE",
			"ACTOR_W.SETFILE",
		};

		for ( int i=0; i<wSCHOOLNUM; ++i )
		{
			for ( WORD j=0; j<GLCI_NUM_ACTOR; ++j )
			{
				CString strKEY;
				strKEY.Format ( "strCLASS_INIT_%d", i );
				cFILE.getflag( strKEY.GetString(), 1+j, GLCI_NUM_ACTOR, strCLASS_INIT[i][j], true );	

			}
		}

		for ( int i=0; i<wSCHOOLNUM; ++i )
		{
			for ( WORD j=0; j<GLCI_NUM_ACTOR; ++j )
			{
				CString strKEY;
				strKEY.Format ( "strJUMPING_CLASS_INIT_%d", i );
				cFILE.getflag( strKEY.GetString(), 1+j, GLCI_NUM_ACTOR, strJUMPING_CLASS_INIT[i][j], true );	

			}
		}


		for ( int i=0; i < GLCI_NUM_ACTOR; i++ )
		{
			if ( cFILE.findflag(szSETFILE[i]) )
			{
				CString strArg;
				cFILE.getflag( szSETFILE[i], 1, 1, strArg, true );

				BOOL bOk = cCONSTCLASS[i].LOADFILE ( strArg.GetString() );
				if ( !bOk )
				{
					MessageBox ( NULL, strArg.GetString(), "File Load Fail", MB_OK );					
				}
			}
		}

		//	Note : 파일 이름 기록
		//
		StringCchCopy ( szErrorFileName, MAX_PATH, strPath.c_str() );


		const char* szSKILFILE[GLCI_NUM_ACTOR] = 
		{
			"FIGHTER_M.SKINFILE",
			"ARMS_M.SKINFILE",
			"ARCHER_W.SKINFILE",
			"SPIRIT_W.SKINFILE",
			"EXTREME_M.SKINFILE",
			"EXTREME_W.SKINFILE",
			"FIGHTER_W.SKINFILE",
			"ARMS_W.SKINFILE",
			"ARCHER_M.SKINFILE",
			"SPIRIT_M.SKINFILE",
			"SCIENTIST_M.SKINFILE",
			"SCIENTIST_W.SKINFILE",
            "ASSASSIN_M.SKINFILE",
            "ASSASSIN_W.SKINFILE",
			"TRICKER_M.SKINFILE",
			"TRICKER_W.SKINFILE",
            "ETC_M.SKINFILE",
            "ETC_W.SKINFILE",
			"ACTOR_M.SKINFILE",
			"ACTOR_W.SKINFILE"
		};

		for ( int i = 0; i < GLCI_NUM_ACTOR; i++ )
		{
			if( cFILE.findflag( szSKILFILE[i] ) )
			{
				std::string strArg;
				cFILE.getflag( szSKILFILE[i], 1, 1, strArg, true );
				StringCchCopy ( szCharSkin[i], MAX_PATH, strArg.c_str() );
			}
		}

		const char* szSimpleManSkinFILE[EM_SIMPLESKIN_NUM] = 
		{
			"M_SIMPLE_CAP.CPS",
			"M_SIMPLE_BODY.CPS",
			"M_SIMPLE_LEG.CPS",
			"M_SIMPLE_FOOT.CPS",			
		};

		const char* szSimpleWomenSkinFILE[EM_SIMPLESKIN_NUM] = 
		{
			"W_SIMPLE_CAP.CPS",
			"W_SIMPLE_BODY.CPS",
			"W_SIMPLE_LEG.CPS",
			"W_SIMPLE_FOOT.CPS",			
		};

		for ( int i = 0; i < EM_SIMPLESKIN_NUM; i++ )
		{
			if( cFILE.findflag( szSimpleManSkinFILE[i] ) )
			{
				std::string strArg;
				cFILE.getflag( szSimpleManSkinFILE[i], 1, 1, strArg, true );
				StringCchCopy ( szManSimpleSkin[i], MAX_PATH, strArg.c_str() );
			}

			if( cFILE.findflag( szSimpleWomenSkinFILE[i] ) )
			{
				std::string strArg;
				cFILE.getflag( szSimpleWomenSkinFILE[i], 1, 1, strArg, true );
				StringCchCopy ( szWomenSimpleSkin[i], MAX_PATH, strArg.c_str() );
			}
		}

		const char* szBillboardSkin[GLCI_NUM_ACTOR] =
		{
			"M_FIGHT_BOARD",
			"M_ARMS_BOARD",
			"W_ARCHER_BOARD",
			"W_SPIRIT_BOARD",

			"M_EXTREME_BOARD",
			"W_EXTREME_BOARD",

			"W_FIGHT_BOARD",
			"W_ARMS_BOARD",
			"M_ARCHER_BOARD",
			"M_SPIRIT_BOARD",
			"M_SCIENTIST_BOARD",
			"W_SCIENTIST_BOARD",
            "M_ASSASSIN_BOARD",
            "W_ASSASSIN_BOARD",
			"M_TRICKER_BOARD",
			"W_TRICKER_BOARD",
            "M_ETC_BOARD",
            "W_ETC_BOARD",
			"M_ACTOR_BOARD",
			"W_ACTOR_BOARD",
		};

		for ( int i = 0; i < GLCI_NUM_ACTOR; i++ )
		{
			if( cFILE.findflag( szBillboardSkin[i] ) )
			{
				std::string strArg;
				cFILE.getflag( szBillboardSkin[i], 1, 1, strArg, true );
				strCharBoard[i] = strArg;
			}
		}

		const char* szItemLevelIcon[EMITEMLEVEL_NUM] =
		{
			"ITEM_LEVEL_NORMAL",
			"ITEM_LEVEL_HIGH",
			"ITEM_LEVEL_RARE",
			"ITEM_LEVEL_UNIQUE",
			"ITEM_LEVEL_EPIC",
			"ITEM_LEVEL_LEGENDARY",
		};

		for ( int i = 0; i < EMITEMLEVEL_NUM; i++ )
		{
			if( cFILE.findflag( szItemLevelIcon[i] ) )
			{
				std::string strArg;
				cFILE.getflag( szItemLevelIcon[i], 1, 1, strArg, true );
				strItemLevelIcon[i] = strArg;
			}
		}


		// 미니게임 - 홀짝
		for ( int i=0; i<MINIGAME_ODDEVEN::MAX_ROUND; ++i )
		{
			cFILE.getflag( "RETURN_RATE", 1+i, MINIGAME_ODDEVEN::MAX_ROUND, MINIGAME_ODDEVEN::fReturnRate[i], true );
			cFILE.getflag( "SUCCESS_RATE", 1+i, MINIGAME_ODDEVEN::MAX_ROUND, MINIGAME_ODDEVEN::fSuccessRate[i], true );
		}

		cFILE.getflag( "TIME_LIMIT", 1, 1, MINIGAME_ODDEVEN::fTimeLimit, true );
		cFILE.getflag( "SHUFFLE_TIME", 1, 1, MINIGAME_ODDEVEN::fShuffleTime, true );
		cFILE.getflag( "MAX_BATTING_MONEY", 1, 1, MINIGAME_ODDEVEN::uiMaxBattingMoney, true );

		EXP_MAX_LOADFILE( "exptable_max.bin" );
		EXP_MAX_2nd_LOADFILE( "exptable_max_2nd.bin" );

		if ( bServer )
		{
			EXP_LOADFILE ( "exptable.charclass" );

			EXP_PC_LOADFILE ( "exptable_pc.bin" );
		}

		// 금액 색상 관련
		
		// Note : 색상
		//

        // 금액 색상 적용되는 시작 값
        cFILE.getflag( "dwTEXT_MONEY_COLOR_BEGIN",  1, 1, MONEYCOLOR::dwTEXT_MONEY_COLOR_BEGIN, true );

        // 기본 색상 값
        for ( int j=0; j<3; ++j )
        {
            int nColor = 0;
            
            if ( cFILE.getflag( "TEXT_MONEY_DEFAULT", j+1, 3, nColor, true ) )
            {
                MONEYCOLOR::dwMONEY_COLOR_DEFAULT |= ((nColor) << (8*(3-j-1)));
            }
        }

		std::string strMoneyColor;

		for ( int i=0; i<MONEYCOLOR::MAX_COLOR; ++i ) 
		{ 
			strMoneyColor = "TEXT_MONEY_COLOR";
			strMoneyColor += ('0' + i);

            DWORD dwColor = 0xFF000000;
            bool  bError  = false;

			for ( int j=0; j<3; ++j )
			{
				int nColor = 0;
				
                if ( cFILE.getflag( strMoneyColor.c_str(), j+1, 3, nColor, true ) )
                {
                     dwColor |= ((nColor) << (8*(3-j-1)));

                }
                else
                {
                    bError = true;
                    break;
                }
			}

            if ( !bError )
            {
                MONEYCOLOR::dwMONEY_COLOR[i] = dwColor;
            }
		}

        // Item Information color.
        {
            const int nRGB = 3;
            int tempDefault[nRGB];
            int tempItemRank[ EMITEM_LEVEL_NSIZE ][nRGB];
            int tempReqWrong[nRGB];
            int tempInchant1[nRGB];
            int tempInchant2[nRGB];
            int tempEquipCostum[nRGB];
            int tempRankUp1[nRGB];
            int tempRankUp2[nRGB];
            int tempSPReduce[nRGB];
            int tempAddAbility[nRGB];
            int tempRandomAbility[nRGB];
            int tempElementalAbility[nRGB];
            int tempTrade[nRGB];
            int tempLimitTime[nRGB];
			int tempAddDisguise[ nRGB ];
			int tempMoney[ nRGB ];
			int tempSetItem[ nRGB ];

            for ( int i = 0; i < nRGB; i++ )
            {
                cFILE.getflag( "dwDefault", 1+i, 3, tempDefault[i], true );
                for ( int j = 0; j < EMITEM_LEVEL_NSIZE; j++ )
                {
                    std::string strItemRank = sc::string::format("dwItemRank%d", j);
                    cFILE.getflag( strItemRank, 1+i, 3, tempItemRank[j][i], true );
                }
                cFILE.getflag( "dwReqWrong", 1+i, 3, tempReqWrong[i], true );
                cFILE.getflag( "dwInchant1", 1+i, 3, tempInchant1[i], true );
                cFILE.getflag( "dwInchant2", 1+i, 3, tempInchant2[i], true );
                cFILE.getflag( "dwEquipCostum", 1+i, 3, tempEquipCostum[i], true );
                cFILE.getflag( "dwRankUp1", 1+i, 3, tempRankUp1[i], true );
                cFILE.getflag( "dwRankUp2", 1+i, 3, tempRankUp2[i], true );
                cFILE.getflag( "dwSPReduce", 1+i, 3, tempSPReduce[i], true );
                cFILE.getflag( "dwAddAbility", 1+i, 3, tempAddAbility[i], true );
                cFILE.getflag( "dwRandomAbility", 1+i, 3, tempRandomAbility[i], true );
                cFILE.getflag( "dwElementalAbility", 1+i, 3, tempElementalAbility[i], true );
                cFILE.getflag( "dwTrade", 1+i, 3, tempTrade[i], true );
                cFILE.getflag( "dwLimitTime", 1+i, 3, tempLimitTime[i], true );
				cFILE.getflag( "dwAddDisguise", 1 + i, 3, tempAddDisguise[ i ], true );
				cFILE.getflag( "dwMoney", 1 + i, 3, tempMoney[ i ], true );
				cFILE.getflag( "dwSetItem", 1 + i, 3, tempSetItem[ i ], true );
            }

            ITEM_INFOR_COLOR::dwDefault = D3DCOLOR_ARGB( 0xFF, tempDefault[0], tempDefault[1], tempDefault[2] );
            for ( int i = 0; i < EMITEM_LEVEL_NSIZE; i++ )
                ITEM_INFOR_COLOR::dwItemRank[ i ] = D3DCOLOR_ARGB( 0xFF, tempItemRank[i][0], tempItemRank[i][1], tempItemRank[i][2] );
            ITEM_INFOR_COLOR::dwReqWrong = D3DCOLOR_ARGB( 0xFF, tempReqWrong[0], tempReqWrong[1], tempReqWrong[2] );
            ITEM_INFOR_COLOR::dwInchant1 = D3DCOLOR_ARGB( 0xFF, tempInchant1[0], tempInchant1[1], tempInchant1[2] );
            ITEM_INFOR_COLOR::dwInchant2 = D3DCOLOR_ARGB( 0xFF, tempInchant2[0], tempInchant2[1], tempInchant2[2] );
            ITEM_INFOR_COLOR::dwEquipCostum = D3DCOLOR_ARGB( 0xFF, tempEquipCostum[0], tempEquipCostum[1], tempEquipCostum[2] );
            ITEM_INFOR_COLOR::dwRankUp1 = D3DCOLOR_ARGB( 0xFF, tempRankUp1[0], tempRankUp1[1], tempRankUp1[2] );
            ITEM_INFOR_COLOR::dwRankUp2 = D3DCOLOR_ARGB( 0xFF, tempRankUp2[0], tempRankUp2[1], tempRankUp2[2] );
            ITEM_INFOR_COLOR::dwSPReduce = D3DCOLOR_ARGB( 0xFF, tempSPReduce[0], tempSPReduce[1], tempSPReduce[2] );
            ITEM_INFOR_COLOR::dwAddAbility = D3DCOLOR_ARGB( 0xFF, tempAddAbility[0], tempAddAbility[1], tempAddAbility[2] );
            ITEM_INFOR_COLOR::dwRandomAbility = D3DCOLOR_ARGB( 0xFF, tempRandomAbility[0], tempRandomAbility[1], tempRandomAbility[2] );
            ITEM_INFOR_COLOR::dwElementalAbility = D3DCOLOR_ARGB( 0xFF, tempElementalAbility[0], tempElementalAbility[1], tempElementalAbility[2] );
            ITEM_INFOR_COLOR::dwTrade = D3DCOLOR_ARGB( 0xFF, tempTrade[0], tempTrade[1], tempTrade[2] );
            ITEM_INFOR_COLOR::dwLimitTime = D3DCOLOR_ARGB( 0xFF, tempLimitTime[0], tempLimitTime[1], tempLimitTime[2] );
			ITEM_INFOR_COLOR::dwAddDisguise = D3DCOLOR_ARGB( 0xFF, tempAddDisguise[ 0 ], tempAddDisguise[ 1 ], tempAddDisguise[ 2 ] );
			ITEM_INFOR_COLOR::dwMoney = D3DCOLOR_ARGB( 0xFF, tempMoney[ 0 ], tempMoney[ 1 ], tempMoney[ 2 ] );
			ITEM_INFOR_COLOR::dwSetItem = D3DCOLOR_ARGB( 0xFF, tempSetItem[ 0 ], tempSetItem[ 1 ], tempSetItem[ 2 ] );

        }

		// Chatting Information Color.
		{
			const int nRGB = 3;
			int nTempCHAT_NORMAL[ nRGB ];
			int nTempCHAT_PRIVATE[ nRGB ];
			int nTempCHAT_PARTY[ nRGB ];
			int nTempCHAT_MASTER[ nRGB ];
			int nTempCHAT_GUILD[ nRGB ];
			int nTempCHAT_ALLIANCE[ nRGB ];
			int nTempCHAT_AREA[ nRGB ];
			int nTempCHAT_PARTY_RECRUIT[ nRGB ];
			int nTempCHAT_TOALL[ nRGB ];
			int nTempCHAT_SYSTEM[ nRGB ];
			int nTempCHAT_FACTION[ nRGB ];
			int nTempCHAT_CHAR_NAME[ nRGB ];
			int nTempCHAT_GM[ nRGB ];

			for ( int i = 0; i < nRGB; ++i )
			{
				cFILE.getflag( "CHAT_NORMAL", 1+i, 3, nTempCHAT_NORMAL[ i ], true );
				cFILE.getflag( "CHAT_PRIVATE", 1+i, 3, nTempCHAT_PRIVATE[ i ], true );
				cFILE.getflag( "CHAT_PARTY", 1+i, 3, nTempCHAT_PARTY[ i ], true );
				cFILE.getflag( "CHAT_MASTER", 1+i, 3, nTempCHAT_MASTER[ i ], true );
				cFILE.getflag( "CHAT_GUILD", 1+i, 3, nTempCHAT_GUILD[ i ], true );
				cFILE.getflag( "CHAT_ALLIANCE", 1+i, 3, nTempCHAT_ALLIANCE[ i ], true );
				cFILE.getflag( "CHAT_AREA", 1+i, 3, nTempCHAT_AREA[ i ], true );
				cFILE.getflag( "CHAT_PARTY_RECRUIT", 1+i, 3, nTempCHAT_PARTY_RECRUIT[ i ], true );
				cFILE.getflag( "CHAT_TOALL", 1+i, 3, nTempCHAT_TOALL[ i ], true );
				cFILE.getflag( "CHAT_SYSTEM", 1+i, 3, nTempCHAT_SYSTEM[ i ], true );
				cFILE.getflag( "CHAT_FACTION", 1+i, 3, nTempCHAT_FACTION[ i ], true );
				cFILE.getflag( "CHAT_CHAR_NAME", 1+i, 3, nTempCHAT_CHAR_NAME[ i ], true );
				cFILE.getflag( "CHAT_GM", 1 + i, 3, nTempCHAT_GM[ i ], true );
			}

			CHAT_INFOR_COLOR::CHAT_NORMAL			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_NORMAL[0], nTempCHAT_NORMAL[1], nTempCHAT_NORMAL[2] );
			CHAT_INFOR_COLOR::CHAT_PRIVATE			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_PRIVATE[0], nTempCHAT_PRIVATE[1], nTempCHAT_PRIVATE[2] );
			CHAT_INFOR_COLOR::CHAT_PARTY			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_PARTY[0], nTempCHAT_PARTY[1], nTempCHAT_PARTY[2] );
			CHAT_INFOR_COLOR::CHAT_MASTER			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_MASTER[0], nTempCHAT_MASTER[1], nTempCHAT_MASTER[2] );
			CHAT_INFOR_COLOR::CHAT_GUILD			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_GUILD[0], nTempCHAT_GUILD[1], nTempCHAT_GUILD[2] );
			CHAT_INFOR_COLOR::CHAT_ALLIANCE			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_ALLIANCE[0], nTempCHAT_ALLIANCE[1], nTempCHAT_ALLIANCE[2] );
			CHAT_INFOR_COLOR::CHAT_AREA				= D3DCOLOR_ARGB( 0xFF, nTempCHAT_AREA[0], nTempCHAT_AREA[1], nTempCHAT_AREA[2] );
			CHAT_INFOR_COLOR::CHAT_PARTY_RECRUIT	= D3DCOLOR_ARGB( 0xFF, nTempCHAT_PARTY_RECRUIT[0], nTempCHAT_PARTY_RECRUIT[1], nTempCHAT_PARTY_RECRUIT[2] );
			CHAT_INFOR_COLOR::CHAT_TOALL			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_TOALL[0], nTempCHAT_TOALL[1], nTempCHAT_TOALL[2] );
			CHAT_INFOR_COLOR::CHAT_SYSTEM			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_SYSTEM[0], nTempCHAT_SYSTEM[1], nTempCHAT_SYSTEM[2] );
			CHAT_INFOR_COLOR::CHAT_FACTION			= D3DCOLOR_ARGB( 0xFF, nTempCHAT_FACTION[0], nTempCHAT_FACTION[1], nTempCHAT_FACTION[2] );
			CHAT_INFOR_COLOR::CHAT_CHAR_NAME		= D3DCOLOR_ARGB( 0xFF, nTempCHAT_CHAR_NAME[0], nTempCHAT_CHAR_NAME[1], nTempCHAT_CHAR_NAME[2] );
			CHAT_INFOR_COLOR::CHAT_GM				= D3DCOLOR_ARGB( 0xFF, nTempCHAT_GM[0], nTempCHAT_GM[1], nTempCHAT_GM[2] );
		}

		for (int i=0; i < ITEM_INFOR_COLOR::ITEM_GRIND::TOTAL_COUNT; ++i)
		{			
			std::string strItemGrindRange = sc::string::format("dwITEM_GRIND_RANGE_%d", i);
			int RangeStart,RangeEnd;
			cFILE.getflag( strItemGrindRange, 1, 2, RangeStart, true );
			cFILE.getflag( strItemGrindRange, 2, 2, RangeEnd, true );

			std::string strItemGrindColor = sc::string::format("dwITEM_GRIND_FONT_COLOR_%d", i);
			int colorR,colorG,colorB;
			cFILE.getflag( strItemGrindColor, 1, 3, colorR, true );
			cFILE.getflag( strItemGrindColor, 2, 3, colorG, true);
			cFILE.getflag( strItemGrindColor, 3, 3, colorB, true );


			ITEM_INFOR_COLOR::ITEM_GRIND itemGrind;
			itemGrind.RangeStart = RangeStart;
			itemGrind.RangeEnd = RangeEnd;
			itemGrind.GrindColor = D3DCOLOR_XRGB(colorR,colorG,colorB);
			ITEM_INFOR_COLOR::dwItemGrind[i] = itemGrind;
		}



        // Skill Information color.
        {
            const int nRGB = 3;
            int tempDefault[nRGB];
            int tempSkillName[nRGB];
            int tempSkillGrade[nRGB];
            int tempSkillType[nRGB];
            int tempSkillElemental[nRGB];
            int tempReqWrong[nRGB];
            int tempSkillRange[nRGB];
            int tempAddAbility[nRGB];
            int tempElementalAbility[nRGB];

            for ( int i = 0; i < nRGB; i++ )
            {
                cFILE.getflag( "dwDefault", 1+i, 3, tempDefault[i], true );
                cFILE.getflag( "dwSkillName", 1+i, 3, tempSkillName[i], true );
                cFILE.getflag( "dwSkillGrade", 1+i, 3, tempSkillGrade[i], true );
                cFILE.getflag( "dwSkillType", 1+i, 3, tempSkillType[i], true );
                cFILE.getflag( "dwSkillElemental", 1+i, 3, tempSkillElemental[i], true );
                cFILE.getflag( "dwReqWrong", 1+i, 3, tempReqWrong[i], true );
                cFILE.getflag( "dwSkillRange", 1+i, 3, tempSkillRange[i], true );
                cFILE.getflag( "dwAddAbility", 1+i, 3, tempAddAbility[i], true );
                cFILE.getflag( "dwElementalAbility", 1+i, 3, tempElementalAbility[i], true );
            }

            SKILL_INFOR_COLOR::dwDefault = D3DCOLOR_ARGB( 0xFF, tempDefault[0], tempDefault[1], tempDefault[2] );
            SKILL_INFOR_COLOR::dwSkillName = D3DCOLOR_ARGB( 0xFF, tempSkillName[0], tempSkillName[1], tempSkillName[2] );
            SKILL_INFOR_COLOR::dwSkillGrade = D3DCOLOR_ARGB( 0xFF, tempSkillGrade[0], tempSkillGrade[1], tempSkillGrade[2] );
            SKILL_INFOR_COLOR::dwSkillType = D3DCOLOR_ARGB( 0xFF, tempSkillType[0], tempSkillType[1], tempSkillType[2] );
            SKILL_INFOR_COLOR::dwSkillElemental = D3DCOLOR_ARGB( 0xFF, tempSkillElemental[0], tempSkillElemental[1], tempSkillElemental[2] );
            SKILL_INFOR_COLOR::dwReqWrong = D3DCOLOR_ARGB( 0xFF, tempReqWrong[0], tempReqWrong[1], tempReqWrong[2] );
            SKILL_INFOR_COLOR::dwSkillRange = D3DCOLOR_ARGB( 0xFF, tempSkillRange[0], tempSkillRange[1], tempSkillRange[2] );
            SKILL_INFOR_COLOR::dwAddAbility = D3DCOLOR_ARGB( 0xFF, tempAddAbility[0], tempAddAbility[1], tempAddAbility[2] );
            SKILL_INFOR_COLOR::dwElementalAbility = D3DCOLOR_ARGB( 0xFF, tempElementalAbility[0], tempElementalAbility[1], tempElementalAbility[2] );
        }

		// Icons
		cFILE.getflag( "strBattleIcon", 1, 3, strBattleIcon, true );
		cFILE.getflag( "strBattleIcon", 2, 3, sBattleIconIndex.wMainID, true );
		cFILE.getflag( "strBattleIcon", 3, 3, sBattleIconIndex.wSubID, true );
		
		cFILE.getflag( "NotifyMoney", 1, 1, NotifyMoney, true );
		cFILE.getflag( "nDecomposeCost", 1, 1, nDecomposeCost, true );
		cFILE.getflag( "fQBoxPickableTime", 1, 1, fQBoxPickableTime, true );
		cFILE.getflag( "fMacro_TermsofClose_LimitTime", 1, 1, fMacro_TermsofClose_LimitTime, true); // 남은시간에 따른 매크로 종료조건;
		cFILE.getflag( "nMacro_TermsofClose_Potion", 1, 1, nMacro_TermsofClose_Potion, true); // 남은 물약에 따른 매크로 종료조건;
		cFILE.getflag( "fMacro_MaxFillTime", 1, 1, fMacro_MaxFillTime, true); // 물약으로 채울수있는 매크로 시간(단위-초);

		int iNum = 0;

		for( int i = 0; i < SCREEN_EFFECT_NUM; ++i )
		{
			cFILE.getflag ( i, "ScreenThrow", 1, 2, iNum, true );
			cFILE.getflag ( i, "ScreenThrow", 2, 2, strScreenEffect[i], true );
		}

		iNum = 0;

		for( int i = 0; i < CAMERA_EFFECT_NUM; ++i )
		{
			cFILE.getflag ( i, "CameraHit", 1, 2, iNum, true );
			cFILE.getflag ( i, "CameraHit", 2, 2, strCameraEffect[i], true );
		}

		cFILE.getflag("strOPItemIcon", 1, 1, strOPItemIcon, true);

		// 일반 캐릭터의 레벨 별 최대스탯포인트 정보;
		nNUM = (int) cFILE.getflagnum( "nMaxStatPoint" );
		for ( int i=0; i<nNUM; ++i )
		{
			SSTATPOINT_BYLEVEL sStatPointByLevel;

			cFILE.getflag( i, "nMaxStatPoint", 1, 2, sStatPointByLevel.dwLevel, true );
			cFILE.getflag( i, "nMaxStatPoint", 2, 2, sStatPointByLevel.dwStatPoint, true );

			vecMaxStatPoint.push_back( sStatPointByLevel );
		}

		// 극강부 캐릭터의 레벨 별 최대스탯포인트 정보;
		nNUM = (int) cFILE.getflagnum( "nExtremeMaxStatPoint" );
		for ( int i=0; i<nNUM; ++i )
		{
			SSTATPOINT_BYLEVEL sStatPointByLevel;

			cFILE.getflag( i, "nExtremeMaxStatPoint", 1, 2, sStatPointByLevel.dwLevel, true );
			cFILE.getflag( i, "nExtremeMaxStatPoint", 2, 2, sStatPointByLevel.dwStatPoint, true );

			vecExtremeMaxStatPoint.push_back( sStatPointByLevel );
		}

		for ( int i = 0; i < 5; ++i )
		{
			INT nTime(-1);
			cFILE.getflag( "nMacroRemainTimeAlarm", i, 5, nTime, true );
			vecMacroRemainTimeAlarm.push_back(nTime);
		}

		cFILE.getflag( "fIgnore_BossDamageMax", 1, 1, fIgnore_BossDamageMax, true );
		cFILE.getflag( "nBossFixedDamageMax", 1, 1, nBossFixedDamageMax, true );
		cFILE.getflag( "fIncRCriticalMax_EmergencyMob", 1, 1, fIncRCriticalMax_EmergencyMob, true );
		cFILE.getflag( "fIncRCriticalMax_EmergencyBoss", 1, 1, fIncRCriticalMax_EmergencyBoss, true );
		cFILE.getflag( "fDecRDamageMax_EmergencyState", 1, 1, fDecRDamageMax_EmergencyState, true );

		cFILE.getflag( "fEmergency_CharHPR", 1, 1, fEmergency_CharHPR, true );
		cFILE.getflag( "fEmergency_MobHPR", 1, 1, fEmergency_MobHPR, true );
		cFILE.getflag( "fEmergency_BossHPR", 1, 1, fEmergency_BossHPR, true );
		cFILE.getflag( "nEmergency_BlinkCount", 1, 1, nEmergency_BlinkCount, true );

		cFILE.getflag( "fIncRExpAddEffectMax", 1, 1, fIncRExpAddEffectMax, true );
		cFILE.getflag( "fIncRDropGenMoneyMax", 1, 1, fIncRDropGenMoneyMax, true );
		cFILE.getflag( "fDecRDamageMax_MeleeSkill", 1, 1, fDecRDamageMax_MeleeSkill, true );
		cFILE.getflag( "fDecRDamageMax_RangeSkill", 1, 1, fDecRDamageMax_RangeSkill, true );
		cFILE.getflag( "fDecRDamageMax_MagicSkill", 1, 1, fDecRDamageMax_MagicSkill, true );
		cFILE.getflag( "nInc_HPMax_SoloPlay", 1, 1, nInc_HPMax_SoloPlay, true );
		cFILE.getflag( "nInc_HPMax_PartyPlay", 1, 1, nInc_HPMax_PartyPlay, true );
		cFILE.getflag( "fIncR_AtkSpeedMax_SoloPlay", 1, 1, fIncR_AtkSpeedMax_SoloPlay, true );
		cFILE.getflag( "fIncR_AtkSpeedMax_PartyPlay", 1, 1, fIncR_AtkSpeedMax_PartyPlay, true );
		cFILE.getflag( "fInc_RandomOp_SkillDurationAddTimeMax", 1, 1, fInc_RandomOp_SkillDurationAddTimeMax, true );
		cFILE.getflag( "fIncR_RandomOp_SkillDamageAddMax", 1, 1, fIncR_RandomOp_SkillDamageAddMax, true );
		cFILE.getflag( "fDecR_SkillDelayTMax", 1, 1, fDecR_SkillDelayTMax, true );
		cFILE.getflag( "fIncR_CriticalDamageMax", 1, 1, fIncR_CriticalDamageMax, true );
		cFILE.getflag( "fIncR_CrushingBlowMax", 1, 1, fIncR_CrushingBlowMax, true );	
		cFILE.getflag( "fIncR_LifePerKillMax", 1, 1, fIncR_LifePerKillMax, true );	
		cFILE.getflag( "fIncR_MPPerKillMax", 1, 1, fIncR_MPPerKillMax, true );	
		cFILE.getflag( "fIncR_SPPerKillMax", 1, 1, fIncR_SPPerKillMax, true );
		cFILE.getflag( "fIncR_HPMPSPPerKillMax", 1, 1, fIncR_HPMPSPPerKillMax, true );
		cFILE.getflag( "fIncR_AllMPRatioToAttackMax", 1, 1, fIncR_AllMPRatioToAttackMax, true );
		cFILE.getflag( "fIncR_AllSPRatioToHpMax", 1, 1, fIncR_AllSPRatioToHpMax, true );
		cFILE.getflag( "fIncR_AllHitRatioToMeleeMax", 1, 1, fIncR_AllHitRatioToMeleeMax, true );
		cFILE.getflag( "fIncR_AllHitRatioToRangeMax", 1, 1, fIncR_AllHitRatioToRangeMax, true );
		cFILE.getflag( "fIncR_AllHitRatioToMagicMax", 1, 1, fIncR_AllHitRatioToMagicMax, true );
		return TRUE;
	}

	BOOL EXP_LOADFILE(char* szFileName)
	{
		if (!szFileName || (strlen(szFileName) == 0))
            return FALSE;

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szFileName;

		gltexfile cFILE;
		cFILE.reg_sep("\t ,{}[]()|\"");

		if (GLOGIC::UseLogicZipFile()) // by 경대
			cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE ); // by 경대, zip파일 사용

		if( !cFILE.open( strPath, true ) )
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_CHAR::EXP_LOADFILE() %1%", szFileName));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}

		WORD dwVERSION = 0;
		cFILE.getflag ( "VERSION", 1, 1, dwVERSION, true );

		TCHAR szTemp[10] = {0};

		for ( WORD i=1; i<EXPTABLE_SIZE+1; ++i )
		{
			//std::strstream	strBuffer;
			//strBuffer << i << std::ends;

			_snprintf_s( szTemp, 10, _TRUNCATE, "%u", i);

			for ( WORD j=0; j<EXPTABLE_LVL; ++j )
			{
				cFILE.getflag( szTemp, 2+j, EXPTABLE_LVL+1, wEXPTABLE[i-1][j], true );	
			}
			//strBuffer.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		}

		return TRUE;
	}

	BOOL EXP_PC_LOADFILE(char* szFileName)
	{
		if( !szFileName || (strlen(szFileName) == 0) )
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_CHAR::EXP_PC_LOADFILE %1%", szFileName));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szFileName;

		std::auto_ptr<sc::CBaseString> pBString(
            GLOGIC::openfile_basestring(
                GLOGIC::UseLogicZipFile(), 
			    GLOGIC::strGLOGIC_SERVER_ZIPFILE, 
			    strPath.c_str(), 
			    szFileName, 
			    true));
		if ( !pBString.get() )
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_CHAR::EXP_PC_LOADFILE(), %1%", szFileName));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}
		sc::CBaseString &cBString = *pBString;

		int i=0;
		CString strLine;
		while ( cBString.GetNextLine(strLine) )
		{
			int nEXP = atoi(strLine.GetString());

			if ( i>=MAX_LEVEL )
			{
                std::string ErrorMsg(
                    sc::string::format(
                        "GLCONST_CHAR::EXP_PC_LOADFILE too long array %1%", i));
				sc::writeLogError(ErrorMsg);
				break;
			}

			dwEXP_PC_TABLE[i++] = (DWORD) nEXP;
		}

		return TRUE;
	}

	BOOL	EXP_MAX_LOADFILE ( char* szFileName )
	{
		if( !szFileName || (strlen(szFileName) == 0) )
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_CHAR::EXP_MAX_LOADFILE() %1%", szFileName));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}

		int i = 0;
		for (i=0; i<MAX_LEVEL; ++i)
            lnEXP_MAX_TABLE[i] = LLONG_MAX;

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szFileName;

		std::auto_ptr<sc::CBaseString> pBString(
            GLOGIC::openfile_basestring(
                GLOGIC::UseLogicZipFile(), 
			    GLogicData::GetInstance().GetGlogicZipFile(), 
			    strPath.c_str(), 
			    szFileName, 
			    true,
			    GLOGIC::UseLogicPackFile()));

		if ( !pBString.get() )
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_CHAR::EXP_MAX_LOADFILE %1%", szFileName));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}

		sc::CBaseString &cBString = *pBString;

		i=0;
		CString strLine;
		while ( cBString.GetNextLine(strLine) )
		{
			LONGLONG lnEXP = _atoi64(strLine.GetString());

			if (i >= MAX_LEVEL)
			{
                std::string ErrorMsg(
                    sc::string::format(
                        "GLCONST_CHAR::EXP_MAX_LOADFILE, too long array %1%", i));
				sc::writeLogError(ErrorMsg);
				break;
			}

			lnEXP_MAX_TABLE[i++] = lnEXP;
		}

		return TRUE;
	}

	BOOL EXP_MAX_2nd_LOADFILE(char* szFileName)
	{
		if (!szFileName || (strlen(szFileName) == 0))
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_CHAR::EXP_MAX_2nd_LOADFILE() %1%", szFileName));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}

		int i = 0;
		for ( i=0; i<MAX_LEVEL; ++i )	lnEXP_MAX_TABLE_2nd[i] = LLONG_MAX;

		std::string strPath;
		strPath = GLOGIC::GetServerPath();
		strPath += szFileName;

		std::auto_ptr<sc::CBaseString> pBString(
            GLOGIC::openfile_basestring(
                GLOGIC::UseLogicZipFile(), 
			    GLogicData::GetInstance().GetGlogicZipFile(),
			    strPath.c_str(), 
			    szFileName, 
			    true,
			    GLOGIC::UseLogicPackFile()));

		if ( !pBString.get() )
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_CHAR::EXP_MAX_2nd_LOADFILE %1%", szFileName));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}

		sc::CBaseString &cBString = *pBString;

		i=0;
		CString strLine;
		while ( cBString.GetNextLine(strLine) )
		{
			LONGLONG lnEXP = _atoi64(strLine.GetString());

			if (i >= MAX_LEVEL)
			{
                std::string ErrorMsg(
                    sc::string::format(
                        "GLCONST_CHAR::EXP_MAX_2nd_LOADFILE(), too long array %1%", i));
				sc::writeLogError(ErrorMsg);
				break;
			}

			lnEXP_MAX_TABLE_2nd[i++] = lnEXP;
		}

		return TRUE;
	}
};


BOOL GLCONST_CHARCLASS::LOADFILE ( const char* szFileName )
{
	if( !szFileName )
		return FALSE;
	if( strlen(szFileName) == 0 )
		return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;

	//	Note : 파일 이름 기록
	//
	StringCchCopy ( szErrorFileName, MAX_PATH, strPath.c_str() );

	gltexfile cFILE;
	cFILE.reg_sep("\t ,{}[]()|\"");

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if (!cFILE.open( strPath, true, GLOGIC::UseLogicPackFile()))
	{
        std::string ErrorMsg(
            sc::string::format(
                "GLCONST_CHAR::EXP_LOADFILE %1%", szFileName));
		sc::writeLogError(ErrorMsg);
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag ( "VERSION", 1, 1, dwVERSION, true );

	cFILE.getflag( "fWALKVELO", 1, 1, fWALKVELO, true );
	cFILE.getflag( "fRUNVELO",	 1, 1, fRUNVELO, true );	

	cFILE.getflag( "fHP_STR", 1, 1, fHP_STR, true );		
	cFILE.getflag( "fMP_SPI", 1, 1, fMP_SPI, true );		
	cFILE.getflag( "fSP_STA", 1, 1, fSP_STA, true );		
	cFILE.getflag( "fHIT_DEX", 1, 1, fHIT_DEX, true );	
	cFILE.getflag( "fAVOID_DEX", 1, 1, fAVOID_DEX, true );	
	cFILE.getflag( "fDEFENSE_DEX", 1, 1, fDEFENSE_DEX, true );

	cFILE.getflag( "fPA_POW",	1, 1, fPA_POW, true );
	cFILE.getflag( "fPA_DEX",	1, 1, fPA_DEX, true );

	cFILE.getflag( "fSA_POW",	1, 1, fSA_POW, true );
	cFILE.getflag( "fSA_DEX",	1, 1, fSA_DEX, true );

	cFILE.getflag( "fMA_DEX",	1, 1, fMA_DEX, true );		
	cFILE.getflag( "fMA_SPI",	1, 1, fMA_SPI, true );		
	cFILE.getflag( "fMA_INT",	1, 1, fMA_INT, true );		

	cFILE.getflag( "fCONV_AP", 1, 1, fCONV_AP, true );	
	cFILE.getflag( "fCONV_DP", 1, 1, fCONV_DP, true );	
	cFILE.getflag( "fCONV_PA", 1, 1, fCONV_PA, true );	
	cFILE.getflag( "fCONV_SA", 1, 1, fCONV_SA, true );	

	cFILE.getflag( "wBEGIN_AP", 1, 1, wBEGIN_AP, true );	
	cFILE.getflag( "wBEGIN_DP", 1, 1, wBEGIN_DP, true );	
	cFILE.getflag( "wBEGIN_PA", 1, 1, wBEGIN_PA, true );	
	cFILE.getflag( "wBEGIN_SA", 1, 1, wBEGIN_SA, true );	

	cFILE.getflag( "fLVLUP_AP", 1, 1, fLVLUP_AP, true );	
	cFILE.getflag( "fLVLUP_DP", 1, 1, fLVLUP_DP, true );	
	cFILE.getflag( "fLVLUP_PA", 1, 1, fLVLUP_PA, true );	
	cFILE.getflag( "fLVLUP_SA", 1, 1, fLVLUP_SA, true );	

	cFILE.getflag( "sBEGIN_STATS", 1, 6, sBEGIN_STATS.wPow, true );
	cFILE.getflag( "sBEGIN_STATS", 2, 6, sBEGIN_STATS.wStr, true );
	cFILE.getflag( "sBEGIN_STATS", 3, 6, sBEGIN_STATS.wSpi, true );
	cFILE.getflag( "sBEGIN_STATS", 4, 6, sBEGIN_STATS.wDex, true );
	cFILE.getflag( "sBEGIN_STATS", 5, 6, sBEGIN_STATS.wInt, true );
	cFILE.getflag( "sBEGIN_STATS", 6, 6, sBEGIN_STATS.wSta, true );

	cFILE.getflag( "sLVLUP_STATS", 1, 6, sLVLUP_STATS.fPow, true );
	cFILE.getflag( "sLVLUP_STATS", 2, 6, sLVLUP_STATS.fStr, true );
	cFILE.getflag( "sLVLUP_STATS", 3, 6, sLVLUP_STATS.fSpi, true );
	cFILE.getflag( "sLVLUP_STATS", 4, 6, sLVLUP_STATS.fDex, true );
	cFILE.getflag( "sLVLUP_STATS", 5, 6, sLVLUP_STATS.fInt, true );
	cFILE.getflag( "sLVLUP_STATS", 6, 6, sLVLUP_STATS.fSta, true );

	cFILE.getflag( "dwHEADNUM", 1, 1, dwHEADNUM, true );	
	if ( dwHEADNUM >= MAX_HEAD )
		dwHEADNUM = MAX_HEAD;

	cFILE.getflag( "dwHEADNUM_SELECT", 1, 1, dwHEADNUM_SELECT, true );	
	if ( dwHEADNUM_SELECT >= MAX_HEAD )		
		dwHEADNUM_SELECT = MAX_HEAD;

	for ( DWORD i=0; (i<dwHEADNUM); ++i )
		cFILE.getflag( "strHEAD_CPS", 1+i, dwHEADNUM, strHEAD_CPS[i], true );	

	cFILE.getflag( "dwHAIRNUM", 1, 1, dwHAIRNUM, true );	
	if ( dwHAIRNUM >= MAX_HAIR )		
		dwHAIRNUM = MAX_HAIR;

	cFILE.getflag( "dwHAIRNUM_SELECT", 1, 1, dwHAIRNUM_SELECT, true );	
	if ( dwHAIRNUM_SELECT >= MAX_HEAD )		
		dwHAIRNUM_SELECT = MAX_HEAD;

	for ( DWORD i=0; (i<dwHAIRNUM); ++i )
		cFILE.getflag( "strHAIR_CPS", 1+i, dwHAIRNUM, strHAIR_CPS[i], true );	

	cFILE.getflag( "strCLASS_EFFECT", 1, 1, strCLASS_EFFECT, true );

	return TRUE;
}

namespace GLCONST_ATTENDANCE
{
	BOOL LoadFile(const std::string& strFileName, BOOL bServer)
	{
		if (!bServer)
            return FALSE;

		std::string strFullPath(GLOGIC::GetServerPath());
        strFullPath.append(strFileName);

		gltexfile cFILE;
		cFILE.reg_sep("\t ,{}[]()|\"");

		if (GLOGIC::UseLogicZipFile())
			cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

		if (!cFILE.open(strFullPath, true, GLOGIC::UseLogicPackFile()))
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_ATTENDANCE::LoadFile %1%",
                    strFileName));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}

		cFILE.getflag( "bUse", 1, 1, bUse, true );

		cFILE.getflag( "dwATTENDTIME", 1, 1, dwAttendTime, true );		

		cFILE.getflag( "dwMAXREWARD", 1, 1, dwMAXREWARD, true );

		for ( DWORD i=0; i<dwMAXREWARD; ++i )
		{
			CString strFLAG;

			ATTEND_REWARD sAttendReward;

			strFLAG.Format ( "REWARD_%02d", i+1 );
			cFILE.getflag( strFLAG.GetString(), 1, 3, sAttendReward.nComboDay, true );
			cFILE.getflag( strFLAG.GetString(), 2, 3, sAttendReward.idReward.wMainID, true );
			cFILE.getflag( strFLAG.GetString(), 3, 3, sAttendReward.idReward.wSubID, true );

			vecAttendReward.push_back( sAttendReward );
		}

		return TRUE;
	}
}
