#pragma once

#include <vector>
#include "../../EngineLib/G-Logic/TypeDefine.h"

//-----------------------------
//  [2/8/2013 hsshin]
// 제작 시스템 리뉴얼
//-----------------------------

namespace sc 
{
	class BaseStream;
	class SerialFile;
}

struct SProductItem
{
	SNATIVEID sNID;
	BYTE nNum;

	SProductItem();
	void RESET();
};

namespace Product
{
	enum
	{
 		ETypeHeadGear = 0,			    // 모자
 		ETypeUpper,					    // 상의
 		ETypeLower,					    // 바지
 		ETypeHand,					    // 장갑
 		ETypeFoot,					    // 신발
 		ETypeHandheld,				    // 무기
 		ETypeNeck_Earring,			    // 목걸이 + 귀걸이
 		ETypeWrist_WaistBand,		    // 팔찌 + 벨트
		ETypeFinger_Accessories,	    // 반지 + 장신구
		ETypeEtc,					    // 기타 + VEHICLE;
		ETypeRandomBox,				    // 랜덤박스
		ETypeCostume,				    // 코스튬 EFindEtc == 13

		ETypeTotal,					    // 모두 (맨마지막에 있어야함 최대 분야수를 산정하는데 쓰임)
	};
	enum
	{
		ITEM_PRODUCT_MAX_MATERIAL = 5,	//제작아이템의 최대 재료갯수
	};
    const static int PRODUCT_NOT_USE_REQ = 65535;    //필요 레벨,숙련도 획득경험치 비활성화 값
}

namespace COMMENT
{
    extern std::string PRODUCT_TYPE_NAME[Product::ETypeTotal];
}

struct SProductRecipe
{
	enum
	{
		VERSION = 0x0102,	            //  [2/8/2013 hsshin] 숙련도 시스템 추가로 버전 변경 1.01 -> 1.02
	};
	DWORD           dwKey;			    //조합법 고유키

	SProductItem    sMeterialItem[ Product::ITEM_PRODUCT_MAX_MATERIAL ];
	SProductItem    sResultItem;

	DWORD           dwRate;			    //제작 성공 확률 
	DWORD           dwPrice;			//제작 가격
	FLOAT           fDelaySec;		    //제작 시간

	BOOL            bNeedLearn;		            
	BOOL            bRandomOption;		//랜덤옵션

	BYTE            nGradeAttack;
	BYTE            nGradeDefense;

	//  [2/8/2013 hsshin] 숙련도 시스템추가
	WORD            wLevel;				//필요레벨
	WORD            wWs;				//필요숙련도
	WORD            wWsp;				//획득숙련도
	WORD            wWspPer;			//숙련도획득확률

	WORD            eProductType;		//조합법 제작분야

    SProductRecipe();

	BOOL            SaveFile( sc::SerialFile& SFile );
	BOOL            LoadFile( sc::BaseStream& SFile );
	BOOL            LoadFile102( sc::BaseStream& SFile );   //숙련도 시스템 추가 버젼파일 읽어오기
	BOOL            LoadFile101( sc::BaseStream& SFile );
	BOOL            LoadFile100( sc::BaseStream& SFile );

	static void     SaveCsvHead( std::fstream &SFile );
	void            SaveCsv( std::fstream &SFile );
	void            LoadCsv( CStringArray &StrArray );
};
