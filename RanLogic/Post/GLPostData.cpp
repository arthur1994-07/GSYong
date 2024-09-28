#include "pch.h"
#include "../../SigmaCore/String/basestring.h"
#include "../../SigmaCore/Container/MList.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/Common/GLTexFile.h"
#include "../../EngineLib/G-Logic/Glogic.h"

#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "../s_NetGlobal.h"
#include "./GLPostData.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//----------------------------------------------------------------------------//

LONGLONG GLPost::llPOST_EXPIRE_DURATION_SEC = 0; 

//----------------------------------------------------------------------------//

GLPost::GLPost()
{
	//Blank
}

GLPost::~GLPost()
{
	//Blank
}

const LONGLONG GLPost::GetCommision ( const BOOL IsAttachDesign, const BOOL IsAttachItem, const LONGLONG llAttachMoney )
{
	LONGLONG llPostFee = (LONGLONG)GLCONST_POST::dwPOST_BASE_FEE;

	if ( IsAttachItem )
	{
		llPostFee += (LONGLONG)GLCONST_POST::dwPOST_COMMISSION_ITEM;
	}

	if ( IsAttachDesign )
	{
		llPostFee += (LONGLONG)GLCONST_POST::dwPOST_COMMISSION_DESIGN;
	}

	// Note : 에러검사
	{
		if ( llAttachMoney < 0 )
		{
			return 0;
		}

		if ( llPostFee < 0 )
		{
			return 0;
		}
	}

	llPostFee += GLogicData::GetInstance().MultiplyBigValueByPoint( llAttachMoney, GLCONST_POST::fPOST_COMMISSION );

	return llPostFee;
}

namespace COMMENT
{
	std::string POSTBOX_DESIGN_TYPE[ EMPOSTBOX_DESIGN_TYPE_SIZE ] =
	{
		"없음",
		"디자인카드 1",
		"디자인카드 2",
		"디자인카드 3",
		"디자인카드 4",
		"디자인카드 5",
		"디자인카드 6",
		"디자인카드 7",
		"디자인카드 8",
		"디자인카드 9",
		"디자인카드 10",
		"디자인카드 11",
		"디자인카드 12",
		"디자인카드 13",
		"디자인카드 14",
		"디자인카드 15",
		"디자인카드 16",
		"디자인카드 17",
		"디자인카드 18",
		"디자인카드 19",
	};
};

// Note : 파일을 읽어들인다. (bjju.post)
namespace GLCONST_POST
{
	LONGLONG	llPOST_MAX_ATTACH_MONEY		= 200000000;

	FLOAT		fPOSTBOX_DISTANCE			= 30.0f;			// 우체통 이용가능한 최대 거리

	FLOAT		fPOST_COMMISSION			= 0.002f;			// 우편 첨부 금액에 대한 수수료
	DWORD		dwPOST_BASE_FEE				= 1000;				// Post Fee for Text mail
	DWORD		dwPOST_COMMISSION_ITEM		= 1000;				// Post Commission for Item attachment
	DWORD		dwPOST_COMMISSION_DESIGN	= 1000;				// Post Commission for Design card

	DWORD		dwPOST_HOW_MANY_GET			= 30;				// how many posts will be get at once
    DWORD       dwPOST_STATE_HOW_MANY_GET   = 30;               // how many post state will be get at once (bjju.post)
    DWORD		dwPOST_SEND_STATE_HOW_MANY_GET	= 15;			// how many post state which be sent mail will be get at once
    DWORD		dwPOST_RECV_STATE_HOW_MANY_GET	= 15;			// how many post state which be received mail will be get at once
	DWORD		dwPOST_EXPIRE_DURATION_DAY	= 7;
	DWORD		dwPOST_SEND_COOL_TIME_MS	= 6000;				// 1min (ms)
	DWORD		dwPOST_RECV_COOL_TIME_MS	= 10000;			// 1min (ms)
	DWORD		dwPOST_SEND_REQUIRE_LEVEL	= 10;				// Require cha's Level for post sending

    BOOL LOADFILE(const std::string& FileName, BOOL bServer)
	{
		if (FileName.empty())
            return FALSE;

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
                    "GLCONST_POST::LOADFILE(), %1%", FileName.c_str()));
			return FALSE;
		}

		// Note : 우체통 이용가능한 최대 거리
		cFILE.getflag( "fPOSTBOX_DISTANCE",				1, 1, fPOSTBOX_DISTANCE, true );
		
		// Note : 첨부할수 있는 금액의 최대치
		cFILE.getflag( "llPOST_MAX_ATTACH_MONEY",		1, 1, llPOST_MAX_ATTACH_MONEY, true );

		// Note : 첨부 금액에대한 수수료 비율
		cFILE.getflag( "fPOST_COMMISSION",				1, 1, fPOST_COMMISSION, true );
		// Note : 기본 수수료
		cFILE.getflag( "dwPOST_BASE_FEE",				1, 1, dwPOST_BASE_FEE, true );
		// Note : 첨부 아이템에대한 수수료
		cFILE.getflag( "dwPOST_COMMISSION_ITEM",		1, 1, dwPOST_COMMISSION_ITEM, true );
		// Note : 디자인 카드에 대한 수수료
		cFILE.getflag( "dwPOST_COMMISSION_DESIGN",		1, 1, dwPOST_COMMISSION_DESIGN, true );


		// Note : 한번에 받을 수 있는 최대 우편 갯수
		cFILE.getflag( "dwPOST_HOW_MANY_GET",			1, 1, dwPOST_HOW_MANY_GET, true );
        // Note : 한번에 받을 수 있는 편지의 최대 현황 갯수
        cFILE.getflag( "dwPOST_STATE_HOW_MANY_GET",     1, 1, dwPOST_STATE_HOW_MANY_GET, true );
        // Note : 한번에 받을 수 있는 보낸 편지의 최대 현황 갯수
        cFILE.getflag( "dwPOST_SEND_STATE_HOW_MANY_GET",1, 1, dwPOST_SEND_STATE_HOW_MANY_GET, true );
        // Note : 한번에 받을 수 있는 받은 편지의 최대 현황 갯수
        cFILE.getflag( "dwPOST_RECV_STATE_HOW_MANY_GET",1, 1, dwPOST_RECV_STATE_HOW_MANY_GET, true );

		// Note : 자동 소멸되는 날짜 (일)
		cFILE.getflag( "dwPOST_EXPIRE_DURATION_DAY",	1, 1, dwPOST_EXPIRE_DURATION_DAY, true );

		// Note : 우편 보내기 쿨타임 (ms)
		cFILE.getflag( "dwPOST_SEND_COOL_TIME_MS",		1, 1, dwPOST_SEND_COOL_TIME_MS, true );

		// Note : 우편 받기 쿨타임 (ms)
		cFILE.getflag( "dwPOST_RECV_COOL_TIME_MS",		1, 1, dwPOST_RECV_COOL_TIME_MS, true );
		
		// Note : 우편을 보내기위한 최소 레벨
		cFILE.getflag( "dwPOST_SEND_REQUIRE_LEVEL",		1, 1, dwPOST_SEND_REQUIRE_LEVEL, true );
		
		//-------------------------------------------------------------------------------------------------//

		// Note : 자동 소멸 날짜를 초로 계산
		GLPost::llPOST_EXPIRE_DURATION_SEC = ((LONGLONG)dwPOST_EXPIRE_DURATION_DAY * 24 * 60 * 60); 

		return TRUE;
	}
};
