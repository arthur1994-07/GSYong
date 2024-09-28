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
	// �⺻ ���� ��ġ�� ������ ����Ʈ���� ������ ������ ���� (crowsale�� ���� ����)
	// NPC�� +�������� �Ĵ� ���� �ϴ� ���ٰ��ؼ� �ٷ� ���Ͻ�Ŵ
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

	if (GLOGIC::UseLogicZipFile()) // by ���
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

	//	Note : �κ� �������� �ִ��� �˻�.
	//
	DWORD dwNUM = glTextFile.getflagnum ( "cInventory" );
	if ( dwNUM==0 )			return FALSE;

	//	Note : �κ� �������� �Ķ��Ÿ ���� �Ǵ�.
	//
	DWORD dwPARAM_NUM = glTextFile.getparamnum ( "cInventory" );
	if ( dwPARAM_NUM < 11 || dwPARAM_NUM > 14  )
	{
		sc::writeLogError(
            sc::string::format(
                "SaleInvenLoadFile %1% �Ķ��Ÿ ���� ���� %2%", szFileName, dwPARAM_NUM));
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
                        "SaleInvenLoadFile() %1%, [%2%,%3%] ������ ���� �ε� ����", szFileName, wPosX, wPosY));
			}

			BOOL bOK = _sINVENTORY.InsertItem ( sItemCustom, wPosX, wPosY, true );
			if ( !bOK )
			{
				sc::writeLogError(
                    sc::string::format(
                        "SaleInvenLoadFile %1% [%2%,%3%] ��ġ �ߺ�", szFileName, wPosX, wPosY));
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
                "SaleInvenLoadFile() %1% cInventory�� �Ķ��Ÿ�� ������ �߸��Ǿ����ϴ�.", szFileName));
	}

	return TRUE;
}

int SaleInvenLoadVer1( gltexfile * const txtparsing, GLInventorySale &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice )
{
	if( txtparsing == NULL )
		return -1;
	//	Note : �κ� �������� �ִ��� �˻�.
	//
	DWORD dwNUM = txtparsing->getflagnum ( "cInventory" );
	if ( dwNUM==0 )			return -1;

	//	Note : �κ� �������� �Ķ��Ÿ ���� �Ǵ�.
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
	
	// ������ <=> ������
	DWORD dwItemVsItemFlagNum  = txtparsing->getflagnum ( "ItemTrade" );
	DWORD dwItemVsItemParamNum = txtparsing->getparamnum ( "ItemTrade" );
	int   dwItemVsItemNeedNum  = ((dwItemVsItemParamNum - 1) - 5) / 3;
	
	// ������ <=> ������Ʈ
	DWORD dwItemVsPtFlagNum  = txtparsing->getflagnum ( "RanPTrade" );
	DWORD dwItemVsPtParamNum = txtparsing->getparamnum ( "RanPTrade" );

	// ������ <=> ��Ȱ����
	DWORD dwItemVsLvPtFlagNum  = txtparsing->getflagnum ( "LivingPTrade" );
	DWORD dwItemVsLvPtParamNum = txtparsing->getparamnum ( "LivingPTrade" );

	txtparsing->getflag ( "szTradeType", 1, 1, _strSaleType, true );

	// ������ <=> ������ ItemTrade
	{
		for ( DWORD i = 0; i < dwItemVsItemFlagNum; ++i )
		{
			sExList data;

			WORD wX = 0, wY = 0;
			WORD wMID   = 0, wSID  = 0;
			WORD wCount = 0;

			dwItemVsItemParamNum = txtparsing->getparamnum ( i, "ItemTrade" );
			dwItemVsItemNeedNum  = ((dwItemVsItemParamNum - 1) - 5) / 3;

			txtparsing->getflag ( i, "ItemTrade", 1,  dwItemVsItemParamNum - 1, wX	  , true );		// x ��ǥ
			txtparsing->getflag ( i, "ItemTrade", 2,  dwItemVsItemParamNum - 1, wY	  , true );		// y ��ǥ
			txtparsing->getflag ( i, "ItemTrade", 3,  dwItemVsItemParamNum - 1, wMID  , true );		// MainID
			txtparsing->getflag ( i, "ItemTrade", 4,  dwItemVsItemParamNum - 1, wSID  , true );		// SubID						
			txtparsing->getflag ( i, "ItemTrade", 5,  dwItemVsItemParamNum - 1, wCount, true );		// ����
			
			for( int j = 0; j < dwItemVsItemNeedNum; ++j )
			{				
				WORD wNeedMid   = 0, wNeedSid = 0, wNeedCount = 0;

				txtparsing->getflag ( i, "ItemTrade", 6 + (j*3),  dwItemVsItemParamNum - 1, wNeedMid  , true );		// �ʿ��� MainID
				txtparsing->getflag ( i, "ItemTrade", 7 + (j*3),  dwItemVsItemParamNum - 1, wNeedSid  , true );		// �ʿ��� SubID
				txtparsing->getflag ( i, "ItemTrade", 8 + (j*3),  dwItemVsItemParamNum - 1, wNeedCount, true );		// �ʿ��� ����
								
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
	
	// ������ <=> ������Ʈ
	{
		for ( DWORD i = 0; i < dwItemVsPtFlagNum; ++i )
		{
			sExList data;

			WORD wX = 0, wY = 0;
			WORD wMID   = 0, wSID  = 0;
			WORD wCount = 0;

			txtparsing->getflag ( i, "RanPTrade", 1,  dwItemVsPtParamNum - 1, wX	, true );		// x ��ǥ
			txtparsing->getflag ( i, "RanPTrade", 2,  dwItemVsPtParamNum - 1, wY	, true );		// y ��ǥ
			txtparsing->getflag ( i, "RanPTrade", 3,  dwItemVsPtParamNum - 1, wMID	, true );		// MainID
			txtparsing->getflag ( i, "RanPTrade", 4,  dwItemVsPtParamNum - 1, wSID	, true );		// SubID						
			txtparsing->getflag ( i, "RanPTrade", 5,  dwItemVsPtParamNum - 1, wCount, true );		// ����

			WORD wNeedPoint = 0;

			txtparsing->getflag ( i, "RanPTrade", 6,  dwItemVsPtParamNum - 1, wNeedPoint, true );		// �ʿ��� ����

			data.sNativeID = SNATIVEID(wMID, wSID);
			data.exType    = EEXTYPE::EX_POINT_TYPE;
			data.iCount    = static_cast<BYTE>(wCount);
			data.vNeedItem.push_back( sExNeed(SNATIVEID(0,0), static_cast<__int64>(wNeedPoint)));

			SITEMCUSTOM sItemCustom( SNATIVEID(wMID, wSID) );

			_sINVENTORY.InsertItem ( sItemCustom, wX, wY );
			_sINVENTORY.push_exchangeData( wX, wY, &data );
		}
	}

	// ������ <=> ��Ȱ����
	{
		for ( DWORD i = 0; i < dwItemVsLvPtFlagNum; ++i )
		{
			sExList data;

			WORD wX = 0, wY = 0;
			WORD wMID   = 0, wSID  = 0;
			WORD wCount = 0;

			txtparsing->getflag ( i, "LivingPTrade", 1,  dwItemVsLvPtParamNum - 1, wX	, true );	// x ��ǥ
			txtparsing->getflag ( i, "LivingPTrade", 2,  dwItemVsLvPtParamNum - 1, wY	, true );	// y ��ǥ
			txtparsing->getflag ( i, "LivingPTrade", 3,  dwItemVsLvPtParamNum - 1, wMID		, true );	// MainID
			txtparsing->getflag ( i, "LivingPTrade", 4,  dwItemVsLvPtParamNum - 1, wSID		, true );	// SubID						
			txtparsing->getflag ( i, "LivingPTrade", 5,  dwItemVsLvPtParamNum - 1, wCount	, true );	// ����

			WORD wNeedPoint = 0;

			txtparsing->getflag ( i, "LivingPTrade", 6,  dwItemVsLvPtParamNum - 1, wNeedPoint, true );	// �ʿ��� ����

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