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

	// Note : �����˻�
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
		"����",
		"������ī�� 1",
		"������ī�� 2",
		"������ī�� 3",
		"������ī�� 4",
		"������ī�� 5",
		"������ī�� 6",
		"������ī�� 7",
		"������ī�� 8",
		"������ī�� 9",
		"������ī�� 10",
		"������ī�� 11",
		"������ī�� 12",
		"������ī�� 13",
		"������ī�� 14",
		"������ī�� 15",
		"������ī�� 16",
		"������ī�� 17",
		"������ī�� 18",
		"������ī�� 19",
	};
};

// Note : ������ �о���δ�. (bjju.post)
namespace GLCONST_POST
{
	LONGLONG	llPOST_MAX_ATTACH_MONEY		= 200000000;

	FLOAT		fPOSTBOX_DISTANCE			= 30.0f;			// ��ü�� �̿밡���� �ִ� �Ÿ�

	FLOAT		fPOST_COMMISSION			= 0.002f;			// ���� ÷�� �ݾ׿� ���� ������
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
		
		if (GLOGIC::UseLogicZipFile()) // by ���
			cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

		if (!cFILE.open(strPath, true, GLOGIC::UseLogicPackFile()))
		{
			sc::writeLogError(
                sc::string::format(
                    "GLCONST_POST::LOADFILE(), %1%", FileName.c_str()));
			return FALSE;
		}

		// Note : ��ü�� �̿밡���� �ִ� �Ÿ�
		cFILE.getflag( "fPOSTBOX_DISTANCE",				1, 1, fPOSTBOX_DISTANCE, true );
		
		// Note : ÷���Ҽ� �ִ� �ݾ��� �ִ�ġ
		cFILE.getflag( "llPOST_MAX_ATTACH_MONEY",		1, 1, llPOST_MAX_ATTACH_MONEY, true );

		// Note : ÷�� �ݾ׿����� ������ ����
		cFILE.getflag( "fPOST_COMMISSION",				1, 1, fPOST_COMMISSION, true );
		// Note : �⺻ ������
		cFILE.getflag( "dwPOST_BASE_FEE",				1, 1, dwPOST_BASE_FEE, true );
		// Note : ÷�� �����ۿ����� ������
		cFILE.getflag( "dwPOST_COMMISSION_ITEM",		1, 1, dwPOST_COMMISSION_ITEM, true );
		// Note : ������ ī�忡 ���� ������
		cFILE.getflag( "dwPOST_COMMISSION_DESIGN",		1, 1, dwPOST_COMMISSION_DESIGN, true );


		// Note : �ѹ��� ���� �� �ִ� �ִ� ���� ����
		cFILE.getflag( "dwPOST_HOW_MANY_GET",			1, 1, dwPOST_HOW_MANY_GET, true );
        // Note : �ѹ��� ���� �� �ִ� ������ �ִ� ��Ȳ ����
        cFILE.getflag( "dwPOST_STATE_HOW_MANY_GET",     1, 1, dwPOST_STATE_HOW_MANY_GET, true );
        // Note : �ѹ��� ���� �� �ִ� ���� ������ �ִ� ��Ȳ ����
        cFILE.getflag( "dwPOST_SEND_STATE_HOW_MANY_GET",1, 1, dwPOST_SEND_STATE_HOW_MANY_GET, true );
        // Note : �ѹ��� ���� �� �ִ� ���� ������ �ִ� ��Ȳ ����
        cFILE.getflag( "dwPOST_RECV_STATE_HOW_MANY_GET",1, 1, dwPOST_RECV_STATE_HOW_MANY_GET, true );

		// Note : �ڵ� �Ҹ�Ǵ� ��¥ (��)
		cFILE.getflag( "dwPOST_EXPIRE_DURATION_DAY",	1, 1, dwPOST_EXPIRE_DURATION_DAY, true );

		// Note : ���� ������ ��Ÿ�� (ms)
		cFILE.getflag( "dwPOST_SEND_COOL_TIME_MS",		1, 1, dwPOST_SEND_COOL_TIME_MS, true );

		// Note : ���� �ޱ� ��Ÿ�� (ms)
		cFILE.getflag( "dwPOST_RECV_COOL_TIME_MS",		1, 1, dwPOST_RECV_COOL_TIME_MS, true );
		
		// Note : ������ ���������� �ּ� ����
		cFILE.getflag( "dwPOST_SEND_REQUIRE_LEVEL",		1, 1, dwPOST_SEND_REQUIRE_LEVEL, true );
		
		//-------------------------------------------------------------------------------------------------//

		// Note : �ڵ� �Ҹ� ��¥�� �ʷ� ���
		GLPost::llPOST_EXPIRE_DURATION_SEC = ((LONGLONG)dwPOST_EXPIRE_DURATION_DAY * 24 * 60 * 60); 

		return TRUE;
	}
};
