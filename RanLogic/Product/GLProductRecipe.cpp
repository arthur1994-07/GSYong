#include "pch.h"

#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtils.h"

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../enginelib/DxMeshs/DxAniKeys.h"
#include "../../enginelib/DxTools/DxFontMan.h"
#include "../../enginelib/DxTools/RENDERPARAM.h"

#include "../GLogicDataMan.h"
#include "./GLProductRecipe.h"
#include "../Item/GLItem.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace COMMENT
{
    std::string PRODUCT_TYPE_NAME[Product::ETypeTotal] = 
    {
        "머리 제작분야",
        "상의 제작분야",
        "하의 제작분야",
        "장갑 제작분야",
        "신발 제작분야",
        "무기 제작분야",
        "목걸이 & 귀걸이 제작분야",
        "팔찌 & 벨트 제작분야",
        "반지 & 장신구 제작분야",
        "기타(ETC) 제작분야",
        "코스튬 제작분야",
    };
}


SProductItem::SProductItem()
{
	RESET();
}

void SProductItem::RESET()
{
	sNID = false;
	nNum = 0;
}

SProductRecipe::SProductRecipe()
: dwKey( UINT_MAX )
, dwRate( 0 )
, dwPrice( 0 )
, fDelaySec( 0.0f )
, bNeedLearn( FALSE )
, bRandomOption( TRUE )
, nGradeAttack( 0 )
, nGradeDefense( 0 )
, wLevel( Product::PRODUCT_NOT_USE_REQ )			//필요레벨
, wWs( Product::PRODUCT_NOT_USE_REQ )				//필요숙련도
, wWsp( Product::PRODUCT_NOT_USE_REQ )				//획득숙련도
, wWspPer( Product::PRODUCT_NOT_USE_REQ )			//숙련도획득확률
, eProductType( Product::PRODUCT_NOT_USE_REQ )		//조합법 제작분야
{
}

BOOL SProductRecipe::SaveFile( sc::SerialFile &SFile )
{
	SFile << DWORD( VERSION );

	SFile << dwKey;

	for( int i=0; i< Product::ITEM_PRODUCT_MAX_MATERIAL; ++i )
	{
		SFile << sMeterialItem[ i ].sNID.dwID;
		SFile << sMeterialItem[ i ].nNum;
	}

	SFile << sResultItem.sNID.dwID;
	SFile << sResultItem.nNum;

	SFile << dwRate;
	SFile << dwPrice;
	SFile << fDelaySec;

	SFile << bNeedLearn;
	SFile << bRandomOption;

	SFile << nGradeAttack;
	SFile << nGradeDefense;

	SFile << wLevel;
	SFile << eProductType;
	SFile << wWs;
	SFile << wWsp;
	SFile << wWspPer;

	return true;
}

BOOL SProductRecipe::LoadFile( sc::BaseStream& SFile )
{
	DWORD dwVersion;
	SFile >> dwVersion;

	switch( dwVersion )
	{
	case 0x0100:
		return LoadFile100( SFile );
	case 0x0101:
		return LoadFile101( SFile );
	case VERSION:
		return LoadFile102( SFile );
	}

	sc::writeLogError(
		sc::string::format(
		"ITEM_MIX %1% != %2%", dwVersion, VERSION ) );

	return false;
}

BOOL SProductRecipe::LoadFile102( sc::BaseStream& SFile)
{
	SFile >> dwKey;

	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i )
	{
		SFile >> sMeterialItem[ i ].sNID.dwID;
		SFile >> sMeterialItem[ i ].nNum;
	}

	SFile >> sResultItem.sNID.dwID;
	SFile >> sResultItem.nNum;

	SFile >> dwRate;
	SFile >> dwPrice;
	SFile >> fDelaySec;

	SFile >> bNeedLearn;
	SFile >> bRandomOption;

	SFile >> nGradeAttack;
	SFile >> nGradeDefense;

	//hsshin 숙련도 기능 추가로인한 조합법 정보 추가
	SFile >> wLevel;			//필요 레벨
	SFile >> eProductType;		//제작 분야
	SFile >> wWs;				//필요 숙련도
	SFile >> wWsp;				//획득 숙련도
	SFile >> wWspPer;			//숙련도 획득 확률

	return true;
}

BOOL SProductRecipe::LoadFile101( sc::BaseStream& SFile )
{
	SFile >> dwKey;

	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i )
	{
		SFile >> sMeterialItem[ i ].sNID.dwID;
		SFile >> sMeterialItem[ i ].nNum;
	}

	SFile >> sResultItem.sNID.dwID;
	SFile >> sResultItem.nNum;

	SFile >> dwRate;
	SFile >> dwPrice;
	SFile >> fDelaySec;

	SFile >> bNeedLearn;
	SFile >> bRandomOption;

	SFile >> nGradeAttack;
	SFile >> nGradeDefense;

	return true;
}

BOOL SProductRecipe::LoadFile100( sc::BaseStream& SFile )
{
	SFile >> dwKey;

	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i )
	{
		SFile >> sMeterialItem[ i ].sNID.dwID;
		SFile >> sMeterialItem[ i ].nNum;
	}

	SFile >> sResultItem.sNID.dwID;
	SFile >> sResultItem.nNum;

	SFile >> dwRate;
	SFile >> dwPrice;

	return true;
}

void SProductRecipe::SaveCsvHead( std::fstream &SFile )
{	
	SFile << "Id" << ",";

	SFile << "Rate" << ",";
	SFile << "Price" << ",";
	SFile << "DelaySec" << ",";

	SFile << "NeedLearn" << ",";
	SFile << "RandomOption" << ",";

	SFile << "Attack" << ",";
	SFile << "Defense" << ",";

	SFile << "Level" << ",";
	SFile << "Type" << ",";
	SFile << "WS" << ",";
	SFile << "WSP" << ",";
	SFile << "WspPer" << ",";

	SFile << "Result Name" << ",";
	SFile << "Result MID" << ",";
	SFile << "Result SID" << ",";
	SFile << "Result Num" << ",";
	
	SFile << "M1 Name" << ",";
	SFile << "M1 MID" << ",";
	SFile << "M1 SID" << ",";
	SFile << "M1 Num" << ",";

	SFile << "M2 Name" << ",";
	SFile << "M2 MID" << ",";
	SFile << "M2 SID" << ",";
	SFile << "M2 Num" << ",";

	SFile << "M3 Name" << ",";
	SFile << "M3 MID" << ",";
	SFile << "M3 SID" << ",";
	SFile << "M3 Num" << ",";

	SFile << "M4 Name" << ",";
	SFile << "M4 MID" << ",";
	SFile << "M4 SID" << ",";
	SFile << "M4 Num" << ",";

	SFile << "M5 Name" << ",";
	SFile << "M5 MID" << ",";
	SFile << "M5 SID" << ",";
	SFile << "M5 Num" << ",";

	SFile << std::endl;
}

void SProductRecipe::SaveCsv( std::fstream &SFile )
{
	SFile << dwKey << ",";

	SFile << dwRate << ",";
	SFile << dwPrice << ",";
	SFile << fDelaySec << ",";

	SFile << bNeedLearn << ",";
	SFile << bRandomOption << ",";

	SFile << DWORD( nGradeAttack ) << ",";
	SFile << DWORD( nGradeDefense ) << ",";

	SFile << WORD( wLevel ) << ",";
	SFile << WORD( eProductType ) << ",";
	SFile << WORD( wWs ) << ",";
	SFile << WORD( wWsp ) << ",";
	SFile << WORD( wWspPer ) << ",";

	SNATIVEID sNativeID = sResultItem.sNID;
	const SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
	if( pItem )
	{
		STRUTIL::OutputStrCsv( SFile, std::string( pItem->GetName() ), true );

		SFile << sNativeID.wMainID << ",";
		SFile << sNativeID.wSubID << ",";
		SFile << DWORD( sResultItem.nNum )  << ",";
	}
	else
	{
		STRUTIL::OutputStrCsv( SFile, std::string() );

		SFile << SNATIVEID::ID_NULL << ",";
		SFile << SNATIVEID::ID_NULL << ",";
		SFile << 0 << ",";
	}

	for ( int i =0; i < Product::ITEM_PRODUCT_MAX_MATERIAL; i++ )
	{
		SNATIVEID sMNativeID = sMeterialItem[ i ].sNID;
		const SITEM* pMItem = GLogicData::GetInstance().GetItem( sMNativeID );
		if( pMItem )
		{
			STRUTIL::OutputStrCsv( SFile, std::string( pMItem->GetName() ), true );

			SFile << sMNativeID.wMainID << ",";
			SFile << sMNativeID.wSubID << ",";
			SFile << DWORD( sMeterialItem[ i ].nNum ) << ",";
		}
		else
		{
			STRUTIL::OutputStrCsv( SFile, std::string() );

			SFile << SNATIVEID::ID_NULL << ",";
			SFile << SNATIVEID::ID_NULL << ",";
			SFile << 0 << ",";
		}
	}

	SFile << std::endl;
}
void SProductRecipe::LoadCsv( CStringArray &StrArray )
{	
	std::string strTemp;
	int iCsvCur = 0;

	dwKey = (DWORD)atoi( StrArray[ iCsvCur++ ] );

	dwRate = (DWORD)atoi( StrArray[ iCsvCur++ ] );
	dwPrice = (DWORD)atoi( StrArray[ iCsvCur++ ] );
	fDelaySec = (FLOAT)atof( StrArray[ iCsvCur++ ] );

	bNeedLearn = atoi( StrArray[ iCsvCur++ ] ) ? TRUE : FALSE;
	bRandomOption = atoi( StrArray[ iCsvCur++ ] ) ? TRUE : FALSE;

	nGradeAttack = (BYTE)atoi( StrArray[ iCsvCur++ ] );
	nGradeDefense = (BYTE)atoi( StrArray[ iCsvCur++ ] );

    wLevel = (WORD)atoi( StrArray[ iCsvCur++ ] );
    eProductType = (WORD)atoi( StrArray[ iCsvCur++ ] );
    wWs = (WORD)atoi( StrArray[ iCsvCur++ ] );
    wWsp = (WORD)atoi( StrArray[ iCsvCur++ ] );
    wWspPer = (WORD)atoi( StrArray[ iCsvCur++ ] );

	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTemp );

	sResultItem.sNID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	sResultItem.sNID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

	if( sResultItem.sNID.wMainID == SNATIVEID::ID_NULL || sResultItem.sNID.wSubID == SNATIVEID::ID_NULL )
	{
		sResultItem.nNum = 0;
		iCsvCur++;
	}
	else
	{
		sResultItem.nNum = (BYTE)atoi( StrArray[ iCsvCur++ ] );
	}

	for ( int i =0; i < Product::ITEM_PRODUCT_MAX_MATERIAL; i++ )
	{
		STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTemp );

		sMeterialItem[i].sNID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sMeterialItem[i].sNID.wSubID  = (WORD)atoi( StrArray[ iCsvCur++ ] );

		if( sMeterialItem[i].sNID.wMainID == SNATIVEID::ID_NULL || sMeterialItem[i].sNID.wSubID == SNATIVEID::ID_NULL )
		{
			sMeterialItem[i].nNum = 0;
			iCsvCur++;
		}
		else
		{
			sMeterialItem[i].nNum = (BYTE)atoi( StrArray[ iCsvCur++ ] );
		}
	}
}