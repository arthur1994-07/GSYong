#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/String/StringUtils.h"

#include "../GLStringTable.h"
#include "./GLItemBasic.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
	static std::string MakeNewIconFileName( const CString& strInventoryFile, const SNATIVEID& sIconID )
	{
		static const char* szExt = ".png";

		int nExtPos = strInventoryFile.ReverseFind( '.' );

		CString strFileName = strInventoryFile.Left( nExtPos );

		CString strNewFileName;
		strNewFileName.Format( "%s_%d_%d%s", strFileName.GetBuffer(), sIconID.Sid(), sIconID.Mid(), szExt );

		return strNewFileName.GetBuffer();
	}

    void SBASIC::Assign ( SBASIC_100 &sOld )
	{
		sNativeID		= sOld.sNativeID;

		strName			= sOld.szName;
		emLevel			= sOld.emLevel;

		dwFlags			= sOld.dwFlags;
		dwBuyPrice		= sOld.dwPrice;
		dwSellPrice		= dwBuyPrice/3;

		emItemType		= sOld.emItemType;

		//	���� ����.
		emReqBright		= sOld.emReqBright;
		dwReqCharClass	= sOld.dwReqCharClass;
		wReqLevelDW		= sOld.wReqLevel;
		wReqPA			= sOld.wReqPA;
		wReqSA			= sOld.wReqSA;
		sReqStats		= sOld.sReqStats;

		strFieldFile	= sOld.szFieldFile;
		strInventoryFile = sOld.szInventoryFile;
		for ( int i=0; i<GLCI_NUM; i++ )
			strWearingFile[i] = sOld.szWearingFIle[i];
		strComment = sOld.szComment;
	}

	void SBASIC::Assign ( SBASIC_101 &sOld )
	{
		sNativeID		= sOld.sNativeID;

		strName			= sOld.szName;
		emLevel			= sOld.emLevel;

		dwFlags			= sOld.dwFlags;
		dwBuyPrice		= sOld.dwPrice;
		dwSellPrice		= dwBuyPrice/3;

		emItemType		= sOld.emItemType;

		//	���� ����.
		emReqBright		= sOld.emReqBright;
		dwReqCharClass	= sOld.dwReqCharClass;
		wReqLevelDW		= sOld.wReqLevel;
		wReqPA			= 0;
		wReqSA			= 0;
		sReqStats		= sOld.sReqStats;

		strSelfBodyEffect = sOld.szSelfBodyEffect;
		strTargBodyEffect = sOld.szTargBodyEffect;
		strTargetEffect = sOld.szTargetEffect;

		strFieldFile	= sOld.szFieldFile;
		strInventoryFile = sOld.szInventoryFile;
		for ( int i=0; i<GLCI_NUM; ++i )
			strWearingFile[i] = sOld.szWearingFIle[i];

		strComment = sOld.szComment;
	}

	void SBASIC::Assign ( SBASIC_102 &sOld )
	{
		sNativeID = sOld.sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

		strName = sOld.szName;						//	�������� �̸�.	
		emLevel = sOld.emLevel;						//	������ ��ġ ���.

		dwFlags		= sOld.dwFlags;						//	Trade �Ӽ�.
		dwBuyPrice	= sOld.dwPrice;						//	������ ���԰���.
		dwSellPrice	= dwBuyPrice/3;				//  ������ �ǸŰ���

		emItemType = sOld.emItemType;				//	������ ����.

		//	���� ����.
		emReqBright = sOld.emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		dwReqCharClass = sOld.dwReqCharClass;		//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		wReqLevelDW = sOld.wReqLevel;					//	�䱸 Level.
		wReqPA = sOld.wReqPA;						//	�䱸 ����ġ.
		wReqSA = sOld.wReqSA;						//	�䱸 ���ġ.
		sReqStats = sOld.sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		sICONID = sOld.sICONID;						//	������ �ε���.

		strSelfBodyEffect = sOld.szSelfBodyEffect;	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		strTargBodyEffect = sOld.szTargBodyEffect;	//	��ǥ ������ ���� �״� ����Ʈ.
		strTargetEffect = sOld.szTargetEffect;		//	��ǥ ���� ����Ʈ.

		strFieldFile = sOld.szFieldFile;			//	�ٴ� ���� ����.
		strInventoryFile = sOld.szInventoryFile;	//	�κ��丮 ���� ����.

		for ( int i=0; i<GLCI_NUM; ++i )	strWearingFile[i] = sOld.szWearingFIle[i];	//	���� ����.

		strComment = sOld.szComment;			//	�����ۿ� ���� ������ ����.

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );
	}

	bool SBASIC::LOAD_103 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_104 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_105 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_106 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_107 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_108 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_EX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_109 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_EX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_110 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;

		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_111 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_112 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		dwSellPrice		= dwBuyPrice/3;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_113 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_114 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> strName;
		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> fExpMultiple;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		//	���� ����.
		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;
		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strSelfBodyEffect;
		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strComment;

		// PET
		SFile >> strPetWearingFile;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;	

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_115 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;

		for ( int i=0; i<GLCI_NUM_NEWSEX; ++i )
			SFile >> strWearingFile[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_116( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;

		for ( int i=0; i<GLCI_NUM_SCIENTIST; ++i )
			SFile >> strWearingFile[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_117 ( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;

		for ( int i=0; i<GLCI_NUM_SCIENTIST; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_SCIENTIST; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

    bool SBASIC::LOAD_118( sc::BaseStream &SFile )
    {
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;

		for ( int i=0; i<GLCI_NUM_SCIENTIST; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_SCIENTIST; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
    }

    bool SBASIC::LOAD_119( sc::BaseStream &SFile )
    {
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

        return true;
    };

	bool SBASIC::LOAD_120( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

        SFile >> emReqActivityType;
        SFile >> wReqActPointDW;
        SFile >> wReqActPointUP;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_121( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		WORD wInvenSizeX, wInvenSizeY;
		SFile >> wInvenSizeX;
		SFile >> wInvenSizeY;

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

    bool SBASIC::LOAD_122( sc::BaseStream &SFile )
    {
        DWORD dwData;

        SFile >> sNativeID.dwID;
        SFile >> sGroupID.dwID;

        SFile >> dwData;
        emLevel = (EMITEMLEVEL)dwData;

        SFile >> dwData;
        emItemType = (EMITEM_TYPE) dwData;

        SFile >> strName;

        SFile >> fExpMultiple;

        SFile >> wGradeAttack;
        SFile >> wGradeDefense;

        SFile >> dwFlags;
		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

        SFile >> wReserved1;
        SFile >> wReserved2;
        SFile >> wReserved3;
        SFile >> wReserved4;
        SFile >> wReserved5;

        //	���� ����.
        SFile >> dwReqCharClass;
        SFile >> dwReqSchool;

        SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

        SFile >> wReqLevelDW;
        SFile >> wReqLevelUP;
        SFile >> wReqPA;
        SFile >> wReqSA;

        SFile >> dwData;
        emReqBright = (EMBRIGHT) dwData;

        //	��Ÿ��
        SFile >> dwCoolTime;

        SFile >> dwData;
        emCoolType = (EMCOOL_TYPE) dwData;		

        SFile >> sICONID.dwID;

        SFile >> strFieldFile;
        SFile >> strInventoryFile;

        SFile >> strTargBodyEffect;
        SFile >> strTargetEffect;
        SFile >> strSelfBodyEffect;

        for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
            SFile >> strWearingFile[i];

        for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
            SFile >> strWearingFileEx[i];

        SFile >> strPetWearingFile;

        SFile >> strComment;

        SFile >> sSubID.dwID;

        SFile >> wPosX;
        SFile >> wPosY;

        SFile >> bSearch; // by luxes.

        SFile >> emReqActivityType;
        SFile >> wReqActPointDW;
        SFile >> wReqActPointUP;

        SFile >> dwReqContribution;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

        return true;
    }

	bool SBASIC::LOAD_123( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
        SFile >> nRPTrade;

		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_124( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> nRPTrade;

		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;
		SFile >> strGeneralEffect;

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_TRICKER; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_125( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> nRPTrade;

		SFile >> dwData;
		dwBuyPrice = dwData;
		SFile >> dwData;
		dwSellPrice = dwData;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;
		SFile >> strGeneralEffect;

		for ( int i=0; i<GLCI_NUM_ETC; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_ETC; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_126( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> nRPTrade;

		SFile >> dwBuyPrice;
		SFile >> dwSellPrice;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;
		SFile >> strGeneralEffect;

		for ( int i=0; i<GLCI_NUM_ETC; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_ETC; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;

		DWORD dwOldM = GLCC_TOOL_MAN   ^ GLCC_ACTOR_M;
		DWORD dwOldW = GLCC_TOOL_WOMAN ^ GLCC_ACTOR_W;
		
		// ���� Ŭ���� ��ü üũ 
		if( dwReqCharClass == GLCC_ALL_TRICKER )
		{
			dwReqCharClass = GLCC_ALL_ACTOR;
		}
		else if( dwReqCharClass == dwOldM )
		{
			dwReqCharClass = GLCC_TOOL_MAN;
		}
		else if( dwReqCharClass == dwOldW )
		{
			dwReqCharClass = GLCC_TOOL_WOMAN;
		}

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;

	}

	bool SBASIC::LOAD_127( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> nRPTrade;

		SFile >> dwBuyPrice;
		SFile >> dwSellPrice;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;
		SFile >> strGeneralEffect;

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );

		return true;
	}

	bool SBASIC::LOAD_128( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> nRPTrade;

		SFile >> dwBuyPrice;
		SFile >> dwSellPrice;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;
		SFile >> strGeneralEffect;

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;
		SFile >> strItemPeriodExtension;

		strInventoryFile = MakeNewIconFileName( strInventoryFile.c_str(), sICONID );
		
		return true;
	}

	bool SBASIC::LOAD_129 ( sc::BaseStream& SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> nRPTrade;

		SFile >> dwBuyPrice;
		SFile >> dwSellPrice;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;
		SFile >> strGeneralEffect;

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;
		SFile >> strItemPeriodExtension;

		return true;
	}

	bool SBASIC::LOAD( sc::BaseStream &SFile )
	{
		DWORD dwData;

		SFile >> sNativeID.dwID;
		SFile >> sGroupID.dwID;

		SFile >> dwData;
		emLevel = (EMITEMLEVEL)dwData;

		SFile >> dwData;
		emItemType = (EMITEM_TYPE) dwData;

		SFile >> strName;

		SFile >> fExpMultiple;

		SFile >> wGradeAttack;
		SFile >> wGradeDefense;

		SFile >> dwFlags;
		SFile >> nRPTrade;

		SFile >> dwBuyPrice;
		SFile >> dwSellPrice;

		SFile >> wReserved1;
		SFile >> wReserved2;
		SFile >> wReserved3;
		SFile >> wReserved4;
		SFile >> wReserved5;

		//	���� ����.
		SFile >> dwReqCharClass;
		SFile >> dwReqSchool;

		SFile.ReadBuffer ( &sReqStats, sizeof(sReqStats) );

		SFile >> wReqLevelDW;
		SFile >> wReqLevelUP;
		SFile >> wReqPA;
		SFile >> wReqSA;

		SFile >> dwData;
		emReqBright = (EMBRIGHT) dwData;

		//	��Ÿ��
		SFile >> dwCoolTime;

		SFile >> dwData;
		emCoolType = (EMCOOL_TYPE) dwData;		

		SFile >> sICONID.dwID;

		SFile >> strFieldFile;
		SFile >> strInventoryFile;

		SFile >> strTargBodyEffect;
		SFile >> strTargetEffect;
		SFile >> strSelfBodyEffect;
		SFile >> strGeneralEffect;

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strArrInventoryFile[i];

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile >> strWearingFileEx[i];

		SFile >> strPetWearingFile;

		SFile >> strComment;

		SFile >> sSubID.dwID;

		SFile >> wPosX;
		SFile >> wPosY;

		SFile >> bSearch; // by luxes.

		SFile >> emReqActivityType;
		SFile >> wReqActPointDW;
		SFile >> wReqActPointUP;

		SFile >> dwReqContribution;
		SFile >> strItemPeriodExtension;

		return true;
	}

	bool SBASIC::SAVE ( sc::SerialFile &SFile )
	{
		SFile << sNativeID.dwID;
		SFile << sGroupID.dwID;

		SFile << (DWORD)emLevel;
		SFile << (DWORD)emItemType;

		CString cstrName;
		cstrName.Format( _T("IN_%03d_%03d"), sNativeID.wMainID , sNativeID.wSubID ); // by ���
		strName = cstrName.GetString();
		SFile << strName;

		SFile << fExpMultiple;

		SFile << wGradeAttack;
		SFile << wGradeDefense;

		SFile << dwFlags;
        SFile << nRPTrade;

		SFile << dwBuyPrice;
		SFile << dwSellPrice;		

		SFile << wReserved1;
		SFile << wReserved2;
		SFile << wReserved3;
		SFile << wReserved4;
		SFile << wReserved5;

		//	���� ����.
		SFile << dwReqCharClass;
		SFile << dwReqSchool;

		SFile.WriteBuffer ( &sReqStats, sizeof(sReqStats) );
		
		SFile << wReqLevelDW;
		SFile << wReqLevelUP;
		SFile << wReqPA;
		SFile << wReqSA;		

		SFile << (DWORD)emReqBright;

		SFile << dwCoolTime;
		SFile << (DWORD)emCoolType;

		SFile << sICONID.dwID;

		SFile << strFieldFile;
		SFile << strInventoryFile;

		SFile << strTargBodyEffect;
		SFile << strTargetEffect;
		SFile << strSelfBodyEffect;
		SFile << strGeneralEffect;

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile << strArrInventoryFile[i];

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile << strWearingFile[i];

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile << strWearingFileEx[i];

		// PET
		SFile << strPetWearingFile;

		cstrName.Format( _T("ID_%03d_%03d"), sNativeID.wMainID , sNativeID.wSubID ); // by ���
		strComment = cstrName.GetString();
		SFile << strComment;

		SFile << sSubID.dwID;

		SFile << wPosX;
		SFile << wPosY;

		SFile << bSearch; // by luxes.

        SFile << emReqActivityType;
        SFile << wReqActPointDW;
        SFile << wReqActPointUP;

		SFile << dwReqContribution;

		SFile << strItemPeriodExtension;

		return true;
	}

    bool SBASIC::SaveLua(sc::TxtFile& LuaFile)
    {
        return true;
    }

	VOID SBASIC::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "sNativeID wMainID" << ",";
		SFile << "sNativeID wSubID" << ",";
		SFile << "sGroupID wMainID" << ",";
		SFile << "sGroupID wSubID" << ",";

		SFile << "strName" << ",";
		SFile << "strName" << ",";

		SFile << "emLevel" << ",";

		SFile << "wGradeAttack" << ",";
		SFile << "wGradeDefense" << ",";

		SFile << "fExpMultiple" << ",";

		SFile << "wReserved1" << ",";
		SFile << "wReserved2" << ",";
		SFile << "wReserved3" << ",";
		SFile << "wReserved4" << ",";
		SFile << "wReserved5" << ",";

		SFile << "dwFlags" << ",";
		SFile << "unRPTrade" << ",";
		SFile << "dwBuyPrice" << ",";
		SFile << "dwSellPrices" << ",";

		SFile << "emItemType" << ",";

		SFile << "emReqBright" << ",";
		SFile << "dwReqCharClass" << ",";
		SFile << "dwReqSchool" << ",";
		SFile << "wReqLevelDW" << ",";
		SFile << "wReqLevelUP" << ",";
		SFile << "wReqPA" << ",";
		SFile << "wReqSA" << ",";
		SFile << "sReqStats wPow" << ",";
		SFile << "sReqStats wStr" << ",";
		SFile << "sReqStats wSpi" << ",";
		SFile << "sReqStats wDex" << ",";
		SFile << "sReqStats wInt" << ",";
		SFile << "sReqStats wSta" << ",";

		//SFile << "sICONID wMainID" << ",";
		//SFile << "sICONID wSubID" << ",";

		SFile << "strFieldFile" << ",";
		SFile << "strInventoryFile" << ",";

		SFile << "strSelfBodyEffect" << ",";
		SFile << "strTargBodyEffect" << ",";
		SFile << "strTargetEffect" << ",";
		SFile << "strGeneralEffect" << ",";

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile << "strArrInventoryFile" << i << ",";

		for( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile << "strWearingFile " << i << ",";

		for( int i=0; i<GLCI_NUM_ACTOR; ++i )
			SFile << "strWearingFileEx " << i << ",";

		SFile << "strComment" << ",";

		// PET
		SFile << "strPetWearingFile" << ",";

		SFile << "sMapID wMainID" << ",";
		SFile << "sMapID wSubID" << ",";

		SFile << "wPosX" << ",";
		SFile << "wPosY" << ",";

		SFile << "dwCoolTime" << ",";
		SFile << "emCoolType" << ",";

		SFile << "bSearch" << ","; // by luxes.
    
        SFile << "emReqActType" << ",";
        SFile << "wReqActPointDW" << ",";
        SFile << "wReqActPointUP" << ",";   

		SFile << "dwReqContribution" << ",";
		SFile << "strItemPeriodExtension" << ",";
	}

	VOID SBASIC::SaveCsv ( std::fstream &SFile )
	{
		SFile << sNativeID.wMainID << ",";
		SFile << sNativeID.wSubID << ",";
		SFile << sGroupID.wMainID << ",";
		SFile << sGroupID.wSubID << ",";

		STRUTIL::OutputStrCsv( SFile, strName );

		std::string str = strName;
		const char* szpName = str.c_str();
		if( szpName )
		{
			const char* szpLongName = GLStringTable::GetInstance().GetString( szpName, GLStringTable::ITEM );
			if( szpLongName )
				str = szpLongName;
		}
		STRUTIL::OutputStrCsv( SFile, str );

		SFile << emLevel << ",";

		SFile << wGradeAttack << ",";
		SFile << wGradeDefense << ",";

		SFile << fExpMultiple << ",";

		SFile << wReserved1 << ",";
		SFile << wReserved2 << ",";
		SFile << wReserved3 << ",";
		SFile << wReserved4 << ",";
		SFile << wReserved5 << ",";

		SFile << dwFlags << ",";
        SFile << nRPTrade << ",";

		SFile << dwBuyPrice << ",";
		SFile << dwSellPrice << ",";

		SFile << emItemType << ",";

		SFile << emReqBright << ",";
		SFile << dwReqCharClass << ",";
		SFile << dwReqSchool << ",";
		SFile << wReqLevelDW << ",";
		SFile << wReqLevelUP << ",";
		SFile << wReqPA << ",";
		SFile << wReqSA << ",";
		SFile << sReqStats.wPow << ",";
		SFile << sReqStats.wStr << ",";
		SFile << sReqStats.wSpi << ",";
		SFile << sReqStats.wDex << ",";
		SFile << sReqStats.wInt << ",";
		SFile << sReqStats.wSta << ",";

		//SFile << sICONID.wMainID << ",";
		//SFile << sICONID.wSubID << ",";

		STRUTIL::OutputStrCsv( SFile, strFieldFile );
		STRUTIL::OutputStrCsv( SFile, strInventoryFile );

		STRUTIL::OutputStrCsv( SFile, strSelfBodyEffect );
		STRUTIL::OutputStrCsv( SFile, strTargBodyEffect );
		STRUTIL::OutputStrCsv( SFile, strTargetEffect );
		STRUTIL::OutputStrCsv( SFile, strGeneralEffect );

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			STRUTIL::OutputStrCsv( SFile, strArrInventoryFile[i] );

		for( int i=0; i<GLCI_NUM_ACTOR; ++i )
			STRUTIL::OutputStrCsv( SFile, strWearingFile[i] );

		for( int i=0; i<GLCI_NUM_ACTOR; ++i )
			STRUTIL::OutputStrCsv( SFile, strWearingFileEx[i] );

		STRUTIL::OutputStrCsv( SFile, strComment );

		// PET
		STRUTIL::OutputStrCsv ( SFile, strPetWearingFile );

		SFile << sSubID.wMainID << ",";
		SFile << sSubID.wSubID << ",";
		
		SFile << wPosX << ",";
		SFile << wPosY << ",";

		SFile << dwCoolTime << ",";
		SFile << emCoolType << ",";

		SFile << bSearch << ","; // by luxes.

        SFile << emReqActivityType << ",";
        SFile << wReqActPointDW << ",";
		SFile << wReqActPointUP << ",";

		SFile << dwReqContribution << ",";

		STRUTIL::OutputStrCsv( SFile, strItemPeriodExtension );
	}

	VOID SBASIC::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
	{
		iCsvCur = 0;

		sNativeID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sNativeID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sGroupID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sGroupID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strName );

		//��ȹ�� ��û���� ������
		++iCsvCur;	// Item Name

		emLevel = (EMITEMLEVEL)atoi( StrArray[ iCsvCur++ ] );

		wGradeAttack = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wGradeDefense = (WORD)atoi( StrArray[ iCsvCur++ ] );

		fExpMultiple = (float)atof( StrArray[ iCsvCur++ ] );

		wReserved1 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReserved2 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReserved3 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReserved4 = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReserved5 = (WORD)atoi( StrArray[ iCsvCur++ ] );

		dwFlags = (DWORD)atol( StrArray[ iCsvCur++ ] );
        nRPTrade = (UINT)atoi( StrArray[ iCsvCur++] );

		dwBuyPrice  = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwSellPrice = (DWORD)atol( StrArray[ iCsvCur++ ] );

		emItemType = (EMITEM_TYPE)atoi( StrArray[ iCsvCur++ ] );

		emReqBright = (EMBRIGHT)atoi( StrArray[ iCsvCur++ ] );
		dwReqCharClass = (DWORD)atol( StrArray[ iCsvCur++ ] );
		dwReqSchool = (DWORD)atol( StrArray[ iCsvCur++ ] );
		wReqLevelDW = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReqLevelUP = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReqPA = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wReqSA = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wPow = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wStr = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wSpi = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wDex = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wInt = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sReqStats.wSta = (WORD)atoi( StrArray[ iCsvCur++ ] );

		//sICONID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		//sICONID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strFieldFile );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strInventoryFile );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strSelfBodyEffect );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTargBodyEffect );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTargetEffect );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strGeneralEffect );

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strArrInventoryFile[i] );

		for( int i=0; i<GLCI_NUM_ACTOR; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strWearingFile[i] );

		for( int i=0; i<GLCI_NUM_ACTOR; ++i )
			STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strWearingFileEx[i] );

		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strComment );

		// PET
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strPetWearingFile );

		sSubID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sSubID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		
		wPosX = (WORD)atoi( StrArray[ iCsvCur++ ] );
		wPosY = (WORD)atoi( StrArray[ iCsvCur++ ] );

		dwCoolTime = (DWORD)atoi( StrArray[ iCsvCur++ ] );
		emCoolType = (EMCOOL_TYPE)atoi( StrArray[ iCsvCur++ ] );

		bSearch = (0!=atoi( StrArray[ iCsvCur++ ] ));

        emReqActivityType = atoi( StrArray[ iCsvCur++ ] );
        wReqActPointDW = (WORD)atoi( StrArray[ iCsvCur++ ] );
        wReqActPointUP = (WORD)atoi( StrArray[ iCsvCur++ ] );

		dwReqContribution  = (DWORD)atoi( StrArray[ iCsvCur++ ] );
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strItemPeriodExtension );
	}

	SBASIC& SBASIC::operator = ( const SBASIC& rvalue )
	{
		sNativeID = rvalue.sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
		sGroupID = rvalue.sGroupID;

		strName = rvalue.strName;						//	�������� �̸�.	
		emLevel = rvalue.emLevel;						//	������ ��ġ ���.

		wGradeAttack = rvalue.wGradeAttack;				// ���� ���� ���
		wGradeDefense = rvalue.wGradeDefense;			// ��� ���� ���

		fExpMultiple = rvalue.fExpMultiple;				// ����ġ ���� ���

		wReserved1 = rvalue.wReserved1;					// ��� ����
		wReserved2 = rvalue.wReserved2;					// ��� ����
		wReserved3 = rvalue.wReserved3;					// ��� ����
		wReserved4 = rvalue.wReserved4;					// ��� ����
		wReserved5 = rvalue.wReserved4;					// ��� ����

		dwFlags = rvalue.dwFlags;						//	Trade �Ӽ�.
        nRPTrade = rvalue.nRPTrade;                   //  �� ����Ʈ �ŷ� ���� Ƚ��

		dwBuyPrice	= rvalue.dwBuyPrice;				//	������ ���� ����.
		dwSellPrice = rvalue.dwSellPrice;				//	������ �Ǹ� ����.

		emItemType = rvalue.emItemType;					//	������ ����.

		//	���� ����.
		emReqBright = rvalue.emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
		dwReqCharClass = rvalue.dwReqCharClass;			//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
		dwReqSchool = rvalue.dwReqSchool;				//	������ �� �ִ� �п�.
		wReqLevelDW = rvalue.wReqLevelDW;				//	�䱸 Level.
		wReqLevelUP = rvalue.wReqLevelUP;				//	�䱸 Level.
		wReqPA = rvalue.wReqPA;							//	�䱸 ����ġ.
		wReqSA = rvalue.wReqSA;							//	�䱸 ���ġ.
		sReqStats = rvalue.sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

		sICONID = rvalue.sICONID;						//	������ �ε���.

		strSelfBodyEffect = rvalue.strSelfBodyEffect;	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		strTargBodyEffect = rvalue.strTargBodyEffect;	//	��ǥ ������ ���� �״� ����Ʈ.
		strTargetEffect = rvalue.strTargetEffect;		//	��ǥ ���� ����Ʈ.
		strGeneralEffect = rvalue.strGeneralEffect;		// ���� ����Ʈ;

		strFieldFile = rvalue.strFieldFile;				//	�ٴ� ���� ����.
		strInventoryFile = rvalue.strInventoryFile;		//	�κ��丮 ���� ����.

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
			strArrInventoryFile[i] = rvalue.strArrInventoryFile[i];	//	������ �κ��丮 ���� ����.

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )	
			strWearingFile[i] = rvalue.strWearingFile[i];	//	���� ����.

		for ( int i=0; i<GLCI_NUM_ACTOR; ++i )	
			strWearingFileEx[i] = rvalue.strWearingFileEx[i];	//	���� ����.

		strComment = rvalue.strComment;

		// PET
		strPetWearingFile = rvalue.strPetWearingFile;

		sSubID = rvalue.sSubID;
		wPosX = rvalue.wPosX;
		wPosY = rvalue.wPosY;

		dwCoolTime = rvalue.dwCoolTime;
		emCoolType = rvalue.emCoolType;

		bSearch = rvalue.bSearch; // by luxes.

        emReqActivityType = rvalue.emReqActivityType;
        wReqActPointDW = rvalue.wReqActPointDW;
        wReqActPointUP = rvalue.wReqActPointUP;            

		dwReqContribution  = rvalue.dwReqContribution;
		strItemPeriodExtension = rvalue.strItemPeriodExtension;

		return *this;
	}


} // namespace ITEM
