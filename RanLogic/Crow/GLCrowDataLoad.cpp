#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../EngineLib/Common/GLTexFile.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"

#include "../Item/GLItemMan.h"
#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "./CrowAttack.h"
#include "./GLCrowData.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

VOID ChangeGradeValueFromItemEditValue( SITEMCUSTOM &sItem )
{
	// 기본 연마 수치를 아이템 에디트에서 셋팅한 값으로 변경 (crowsale의 값은 무시)
	// NPC가 +아이템을 파는 경우는 일단 없다고해서 바로 리턴시킴
	return;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( sItem.GetNativeID() );
	if( !pItem )
		return;

	sItem.cDAMAGE = (BYTE)pItem->sBasicOp.wGradeAttack;
	sItem.cDEFENSE = (BYTE)pItem->sBasicOp.wGradeDefense;
}

BOOL SaleInvenLoadFile ( const char* szFileName, GLInventory &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice )
{
	if (!szFileName)
        return FALSE;
	if (strlen(szFileName) == 0)
        return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;	

	gltexfile glTextFile;
	glTextFile.reg_sep("\t ,[]");

	if (GLOGIC::UseLogicZipFile()) // by 경대
        glTextFile.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if (!glTextFile.open(strPath, true, GLOGIC::UseLogicPackFile()))
	{
		sc::writeLogError(
            sc::string::format(
                "SaleInvenLoadFile %1%", szFileName));
		return FALSE;
	}
	
	WORD dwVERSION = 0;

	glTextFile.getflag ( "VERSION", 1, 1, dwVERSION, true );

	//	Note : 인벤 아이템이 있는지 검사.
	//
	DWORD dwNUM = glTextFile.getflagnum ( "cInventory" );
	if ( dwNUM==0 )			return FALSE;

	//	Note : 인벤 아이템의 파라메타 갯수 판단.
	//
	DWORD dwPARAM_NUM = glTextFile.getparamnum ( "cInventory" );
	if ( dwPARAM_NUM < 11 || dwPARAM_NUM > 14  )
	{
		sc::writeLogError(
            sc::string::format(
                "SaleInvenLoadFile %1% 파라메타 갯수 오류 %2%", szFileName, dwPARAM_NUM));
		return FALSE;
	}

	glTextFile.getflag ( "szSaleType", 1, 1, _strSaleType, true );

	if ( dwPARAM_NUM==11 || dwPARAM_NUM==12 )
	{
		for ( DWORD i=0; i<dwNUM; ++i )
		{
			DWORD dwPART_PARAM_NUM = glTextFile.getparamnum( i, "cInventory" );

            WORD wMID = 0, wSID = 0;
			glTextFile.getflag ( i, "cInventory", 1,  dwPART_PARAM_NUM -1, wMID, true );
			glTextFile.getflag ( i, "cInventory", 2,  dwPART_PARAM_NUM -1, wSID, true );

            SITEMCUSTOM sItemCustom( SNATIVEID(wMID, wSID) );
			glTextFile.getflag ( i, "cInventory", 3,  dwPART_PARAM_NUM -1, sItemCustom.wTurnNum, true );
			glTextFile.getflag ( i, "cInventory", 4,  dwPART_PARAM_NUM -1, sItemCustom.cDAMAGE, true );
			glTextFile.getflag ( i, "cInventory", 5,  dwPART_PARAM_NUM -1, sItemCustom.cDEFENSE, true );
			glTextFile.getflag ( i, "cInventory", 6,  dwPART_PARAM_NUM -1, sItemCustom.cRESIST_FIRE, true );
			glTextFile.getflag ( i, "cInventory", 7,  dwPART_PARAM_NUM -1, sItemCustom.cRESIST_ICE, true );
			glTextFile.getflag ( i, "cInventory", 8,  dwPART_PARAM_NUM -1, sItemCustom.cRESIST_ELEC, true );
			glTextFile.getflag ( i, "cInventory", 9,  dwPART_PARAM_NUM -1, sItemCustom.cRESIST_POISON, true );
			glTextFile.getflag ( i, "cInventory", 10, dwPART_PARAM_NUM -1, sItemCustom.cRESIST_SPIRIT, true );

			ChangeGradeValueFromItemEditValue( sItemCustom );
			_sINVENTORY.InsertItem ( sItemCustom );

			if( dwPART_PARAM_NUM  == 12 )
			{
				LONGLONG lNpcSellValue = 0;
				glTextFile.getflag ( i, "cInventory", 11, dwPART_PARAM_NUM -1, lNpcSellValue, true );
				_mapNpcSellPrice[sItemCustom.Id()] = lNpcSellValue;
			}
		}
	}
	else if ( dwPARAM_NUM==13 || dwPARAM_NUM==14 )
	{
		for ( DWORD i=0; i<dwNUM; ++i )
		{
			WORD wPosX(0), wPosY(0);
            WORD wMID = 0, wSID = 0;
			DWORD dwPART_PARAM_NUM = glTextFile.getparamnum( i, "cInventory" );

			glTextFile.getflag ( i, "cInventory", 1,  dwPART_PARAM_NUM-1, wPosX, true );
			glTextFile.getflag ( i, "cInventory", 2,  dwPART_PARAM_NUM-1, wPosY, true );
			glTextFile.getflag ( i, "cInventory", 3,  dwPART_PARAM_NUM-1, wMID, true );
			glTextFile.getflag ( i, "cInventory", 4,  dwPART_PARAM_NUM-1, wSID, true );

            SITEMCUSTOM sItemCustom( SNATIVEID(wMID, wSID) );
			glTextFile.getflag ( i, "cInventory", 5,  dwPART_PARAM_NUM-1, sItemCustom.wTurnNum, true );
			glTextFile.getflag ( i, "cInventory", 6,  dwPART_PARAM_NUM-1, sItemCustom.cDAMAGE, true );
			glTextFile.getflag ( i, "cInventory", 7,  dwPART_PARAM_NUM-1, sItemCustom.cDEFENSE, true );
			glTextFile.getflag ( i, "cInventory", 8,  dwPART_PARAM_NUM-1, sItemCustom.cRESIST_FIRE, true );
			glTextFile.getflag ( i, "cInventory", 9,  dwPART_PARAM_NUM-1, sItemCustom.cRESIST_ICE, true );
			glTextFile.getflag ( i, "cInventory", 10, dwPART_PARAM_NUM-1, sItemCustom.cRESIST_ELEC, true );
			glTextFile.getflag ( i, "cInventory", 11, dwPART_PARAM_NUM-1, sItemCustom.cRESIST_POISON, true );
			glTextFile.getflag ( i, "cInventory", 12, dwPART_PARAM_NUM-1, sItemCustom.cRESIST_SPIRIT, true );

			ChangeGradeValueFromItemEditValue( sItemCustom );

			const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
			if( !pItem )
			{
				sc::writeLogError(
                    sc::string::format(
                        "SaleInvenLoadFile() %1%, [%2%,%3%] 아이템 파일 로드 실패", szFileName, wPosX, wPosY));
			}

			BOOL bOK = _sINVENTORY.InsertItem ( sItemCustom, wPosX, wPosY, true );
			if ( !bOK )
			{
				sc::writeLogError(
                    sc::string::format(
                        "SaleInvenLoadFile %1% [%2%,%3%] 위치 중복", szFileName, wPosX, wPosY));
			}

			if( dwPART_PARAM_NUM == 14 )
			{
				LONGLONG lNpcSellValue = 0;
				glTextFile.getflag ( i, "cInventory", 13, dwPART_PARAM_NUM-1, lNpcSellValue, true );
				_mapNpcSellPrice[sItemCustom.Id()] = lNpcSellValue;
			}
		}
	}
	else
	{
		sc::writeLogError(
            sc::string::format(
                "SaleInvenLoadFile() %1% cInventory의 파라메타의 갯수가 잘못되었습니다.", szFileName));
	}

	return TRUE;
}

int SaleInvenLoadVer1( gltexfile * const txtparsing, GLInventorySale &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice )
{
	if( txtparsing == NULL )
		return -1;
	//	Note : 인벤 아이템이 있는지 검사.
	//
	DWORD dwNUM = txtparsing->getflagnum ( "cInventory" );
	if ( dwNUM==0 )			return -1;

	//	Note : 인벤 아이템의 파라메타 갯수 판단.
	//
	DWORD dwPARAM_NUM = txtparsing->getparamnum ( "cInventory" );
	if ( dwPARAM_NUM < 11 || dwPARAM_NUM > 14  )
	{		
		return SCROWDATA::CROW_LOAD_ERROR1;
	}

	txtparsing->getflag ( "szSaleType", 1, 1, _strSaleType, true );

	if ( dwPARAM_NUM==11 || dwPARAM_NUM==12 )
	{
		for ( DWORD i=0; i<dwNUM; ++i )
		{
			DWORD dwPART_PARAM_NUM = txtparsing->getparamnum( i, "cInventory" );

			WORD wMID = 0, wSID = 0;
			txtparsing->getflag ( i, "cInventory", 1,  dwPART_PARAM_NUM -1, wMID, true );
			txtparsing->getflag ( i, "cInventory", 2,  dwPART_PARAM_NUM -1, wSID, true );

			SITEMCUSTOM sItemCustom( SNATIVEID(wMID, wSID) );
			txtparsing->getflag ( i, "cInventory", 3,  dwPART_PARAM_NUM -1, sItemCustom.wTurnNum, true );
			txtparsing->getflag ( i, "cInventory", 4,  dwPART_PARAM_NUM -1, sItemCustom.cDAMAGE, true );
			txtparsing->getflag ( i, "cInventory", 5,  dwPART_PARAM_NUM -1, sItemCustom.cDEFENSE, true );
			txtparsing->getflag ( i, "cInventory", 6,  dwPART_PARAM_NUM -1, sItemCustom.cRESIST_FIRE, true );
			txtparsing->getflag ( i, "cInventory", 7,  dwPART_PARAM_NUM -1, sItemCustom.cRESIST_ICE, true );
			txtparsing->getflag ( i, "cInventory", 8,  dwPART_PARAM_NUM -1, sItemCustom.cRESIST_ELEC, true );
			txtparsing->getflag ( i, "cInventory", 9,  dwPART_PARAM_NUM -1, sItemCustom.cRESIST_POISON, true );
			txtparsing->getflag ( i, "cInventory", 10, dwPART_PARAM_NUM -1, sItemCustom.cRESIST_SPIRIT, true );

			ChangeGradeValueFromItemEditValue( sItemCustom );
			_sINVENTORY.InsertItem ( sItemCustom );

			if( dwPART_PARAM_NUM  == 12 )
			{
				LONGLONG lNpcSellValue = 0;
				txtparsing->getflag ( i, "cInventory", 11, dwPART_PARAM_NUM -1, lNpcSellValue, true );
				_mapNpcSellPrice[sItemCustom.Id()] = lNpcSellValue;
			}
		}
	}
	else if ( dwPARAM_NUM==13 || dwPARAM_NUM==14 )
	{
		for ( DWORD i=0; i<dwNUM; ++i )
		{
			WORD wPosX(0), wPosY(0);
			WORD wMID = 0, wSID = 0;
			DWORD dwPART_PARAM_NUM = txtparsing->getparamnum( i, "cInventory" );

			txtparsing->getflag ( i, "cInventory", 1,  dwPART_PARAM_NUM-1, wPosX, true );
			txtparsing->getflag ( i, "cInventory", 2,  dwPART_PARAM_NUM-1, wPosY, true );
			txtparsing->getflag ( i, "cInventory", 3,  dwPART_PARAM_NUM-1, wMID, true );
			txtparsing->getflag ( i, "cInventory", 4,  dwPART_PARAM_NUM-1, wSID, true );

			SITEMCUSTOM sItemCustom( SNATIVEID(wMID, wSID) );
			txtparsing->getflag ( i, "cInventory", 5,  dwPART_PARAM_NUM-1, sItemCustom.wTurnNum, true );
			txtparsing->getflag ( i, "cInventory", 6,  dwPART_PARAM_NUM-1, sItemCustom.cDAMAGE, true );
			txtparsing->getflag ( i, "cInventory", 7,  dwPART_PARAM_NUM-1, sItemCustom.cDEFENSE, true );
			txtparsing->getflag ( i, "cInventory", 8,  dwPART_PARAM_NUM-1, sItemCustom.cRESIST_FIRE, true );
			txtparsing->getflag ( i, "cInventory", 9,  dwPART_PARAM_NUM-1, sItemCustom.cRESIST_ICE, true );
			txtparsing->getflag ( i, "cInventory", 10, dwPART_PARAM_NUM-1, sItemCustom.cRESIST_ELEC, true );
			txtparsing->getflag ( i, "cInventory", 11, dwPART_PARAM_NUM-1, sItemCustom.cRESIST_POISON, true );
			txtparsing->getflag ( i, "cInventory", 12, dwPART_PARAM_NUM-1, sItemCustom.cRESIST_SPIRIT, true );

			ChangeGradeValueFromItemEditValue( sItemCustom );

			const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
			if( !pItem )
			{
				return SCROWDATA::CROW_LOAD_ERROR2;
			}

			BOOL bOK = _sINVENTORY.InsertItem ( sItemCustom, wPosX, wPosY, true );
			if ( !bOK )
			{
				return SCROWDATA::CROW_LOAD_ERROR3;
			}

			if( dwPART_PARAM_NUM == 14 )
			{
				LONGLONG lNpcSellValue = 0;
				txtparsing->getflag ( i, "cInventory", 13, dwPART_PARAM_NUM-1, lNpcSellValue, true );
				_mapNpcSellPrice[sItemCustom.Id()] = lNpcSellValue;
			}
		}
	}
	else
	{
		return SCROWDATA::CROW_LOAD_ERROR4;
	}

	return SCROWDATA::CROW_LOAD_SALE_SUCCESS;
}

int SaleInvenLoadVer2( gltexfile* const txtparsing, GLInventorySale &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice )
{
	if( txtparsing == NULL )
		return -1;
	
	// 아이템 <=> 아이템
	DWORD dwItemVsItemFlagNum  = txtparsing->getflagnum ( "ItemTrade" );
	DWORD dwItemVsItemParamNum = txtparsing->getparamnum ( "ItemTrade" );
	int   dwItemVsItemNeedNum  = ((dwItemVsItemParamNum - 1) - 5) / 3;
	
	// 아이템 <=> 란포인트
	DWORD dwItemVsPtFlagNum  = txtparsing->getflagnum ( "RanPTrade" );
	DWORD dwItemVsPtParamNum = txtparsing->getparamnum ( "RanPTrade" );

	// 아이템 <=> 생활점수
	DWORD dwItemVsLvPtFlagNum  = txtparsing->getflagnum ( "LivingPTrade" );
	DWORD dwItemVsLvPtParamNum = txtparsing->getparamnum ( "LivingPTrade" );

	txtparsing->getflag ( "szTradeType", 1, 1, _strSaleType, true );

	// 아이템 <=> 아이템 ItemTrade
	{
		for ( DWORD i = 0; i < dwItemVsItemFlagNum; ++i )
		{
			sExList data;

			WORD wX = 0, wY = 0;
			WORD wMID   = 0, wSID  = 0;
			WORD wCount = 0;

			dwItemVsItemParamNum = txtparsing->getparamnum ( i, "ItemTrade" );
			dwItemVsItemNeedNum  = ((dwItemVsItemParamNum - 1) - 5) / 3;

			txtparsing->getflag ( i, "ItemTrade", 1,  dwItemVsItemParamNum - 1, wX	  , true );		// x 좌표
			txtparsing->getflag ( i, "ItemTrade", 2,  dwItemVsItemParamNum - 1, wY	  , true );		// y 좌표
			txtparsing->getflag ( i, "ItemTrade", 3,  dwItemVsItemParamNum - 1, wMID  , true );		// MainID
			txtparsing->getflag ( i, "ItemTrade", 4,  dwItemVsItemParamNum - 1, wSID  , true );		// SubID						
			txtparsing->getflag ( i, "ItemTrade", 5,  dwItemVsItemParamNum - 1, wCount, true );		// 개수
			
			for( int j = 0; j < dwItemVsItemNeedNum; ++j )
			{				
				WORD wNeedMid   = 0, wNeedSid = 0, wNeedCount = 0;

				txtparsing->getflag ( i, "ItemTrade", 6 + (j*3),  dwItemVsItemParamNum - 1, wNeedMid  , true );		// 필요템 MainID
				txtparsing->getflag ( i, "ItemTrade", 7 + (j*3),  dwItemVsItemParamNum - 1, wNeedSid  , true );		// 필요템 SubID
				txtparsing->getflag ( i, "ItemTrade", 8 + (j*3),  dwItemVsItemParamNum - 1, wNeedCount, true );		// 필요템 개수
								
				data.vNeedItem.push_back( stExNeed( SNATIVEID(wNeedMid, wNeedSid), static_cast<__int64>(wNeedCount) ) );
			}
			
			data.sNativeID = SNATIVEID(wMID, wSID);
			data.exType    = EEXTYPE::EX_ITEM_TYPE;
			data.iCount    = static_cast<BYTE>(wCount);

			SITEMCUSTOM sItemCustom( SNATIVEID(wMID, wSID) );
			

			_sINVENTORY.InsertItem ( sItemCustom, wX, wY );
			_sINVENTORY.push_exchangeData( wX, wY, &data );
		}
	}
	
	// 아이템 <=> 란포인트
	{
		for ( DWORD i = 0; i < dwItemVsPtFlagNum; ++i )
		{
			sExList data;

			WORD wX = 0, wY = 0;
			WORD wMID   = 0, wSID  = 0;
			WORD wCount = 0;

			txtparsing->getflag ( i, "RanPTrade", 1,  dwItemVsPtParamNum - 1, wX	, true );		// x 좌표
			txtparsing->getflag ( i, "RanPTrade", 2,  dwItemVsPtParamNum - 1, wY	, true );		// y 좌표
			txtparsing->getflag ( i, "RanPTrade", 3,  dwItemVsPtParamNum - 1, wMID	, true );		// MainID
			txtparsing->getflag ( i, "RanPTrade", 4,  dwItemVsPtParamNum - 1, wSID	, true );		// SubID						
			txtparsing->getflag ( i, "RanPTrade", 5,  dwItemVsPtParamNum - 1, wCount, true );		// 개수

			WORD wNeedPoint = 0;

			txtparsing->getflag ( i, "RanPTrade", 6,  dwItemVsPtParamNum - 1, wNeedPoint, true );		// 필요템 개수

			data.sNativeID = SNATIVEID(wMID, wSID);
			data.exType    = EEXTYPE::EX_POINT_TYPE;
			data.iCount    = static_cast<BYTE>(wCount);
			data.vNeedItem.push_back( sExNeed(SNATIVEID(0,0), static_cast<__int64>(wNeedPoint)));

			SITEMCUSTOM sItemCustom( SNATIVEID(wMID, wSID) );

			_sINVENTORY.InsertItem ( sItemCustom, wX, wY );
			_sINVENTORY.push_exchangeData( wX, wY, &data );
		}
	}

	// 아이템 <=> 생활점수
	{
		for ( DWORD i = 0; i < dwItemVsLvPtFlagNum; ++i )
		{
			sExList data;

			WORD wX = 0, wY = 0;
			WORD wMID   = 0, wSID  = 0;
			WORD wCount = 0;

			txtparsing->getflag ( i, "LivingPTrade", 1,  dwItemVsLvPtParamNum - 1, wX	, true );	// x 좌표
			txtparsing->getflag ( i, "LivingPTrade", 2,  dwItemVsLvPtParamNum - 1, wY	, true );	// y 좌표
			txtparsing->getflag ( i, "LivingPTrade", 3,  dwItemVsLvPtParamNum - 1, wMID		, true );	// MainID
			txtparsing->getflag ( i, "LivingPTrade", 4,  dwItemVsLvPtParamNum - 1, wSID		, true );	// SubID						
			txtparsing->getflag ( i, "LivingPTrade", 5,  dwItemVsLvPtParamNum - 1, wCount	, true );	// 개수

			WORD wNeedPoint = 0;

			txtparsing->getflag ( i, "LivingPTrade", 6,  dwItemVsLvPtParamNum - 1, wNeedPoint, true );	// 필요템 개수

			data.sNativeID = SNATIVEID(wMID, wSID);
			data.exType    = EEXTYPE::EX_LVP_TYPE;
			data.vNeedItem.push_back( sExNeed(SNATIVEID(0,0), static_cast<__int64>(wNeedPoint)));
			data.iCount    = static_cast<BYTE>(wCount);

			SITEMCUSTOM sItemCustom( SNATIVEID(wMID, wSID) );

			_sINVENTORY.InsertItem ( sItemCustom, wX, wY );
			_sINVENTORY.push_exchangeData( wX, wY, &data );
		}
	}
	
	return SCROWDATA::CROW_LOAD_EXCHANGE_SUCCESS;
}