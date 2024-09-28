
#pragma once

#include <string>

//
//mjeon.Add
//We can move following definitions for post system to anywhere in valid scope.
//

enum EMPOSTBOXID_TYPE
{
	EMPOSTBOXID_TYPE_NONE		= 0,
	EMPOSTBOXID_TYPE_NPC		= 1,
	EMPOSTBOXID_TYPE_ITEM_CARD  = 2,
};

enum EMPOSTBOX_DESIGN_TYPE
{
	EMPOSTBOX_DESIGN_TYPE_SIZE = 20,
};

class GLPost
{
public:
	GLPost ();
	virtual ~GLPost();

public:
	static LONGLONG llPOST_EXPIRE_DURATION_SEC;

public:
	static const LONGLONG GetCommision ( const BOOL IsAttachDesign, const BOOL IsAttachItem, const LONGLONG llAttachMoney );

};

namespace COMMENT
{
	extern std::string	POSTBOX_DESIGN_TYPE[ EMPOSTBOX_DESIGN_TYPE_SIZE ];
};

namespace GLCONST_POST
{
	extern LONGLONG		llPOST_MAX_ATTACH_MONEY;

	extern FLOAT		fPOSTBOX_DISTANCE;				// 우체통 이용가능한 최대 거리

	extern FLOAT		fPOST_COMMISSION;				// 우편 첨부 금액에 대한 수수료
	extern DWORD		dwPOST_BASE_FEE;				// Post Fee for Text mail
	extern DWORD		dwPOST_COMMISSION_ITEM;			// Post Commission for Item attachment
	extern DWORD		dwPOST_COMMISSION_DESIGN;		// Post Commission for Design card

	extern DWORD		dwPOST_HOW_MANY_GET;				// how many posts will be get at once
    extern DWORD		dwPOST_STATE_HOW_MANY_GET;		    // how many post state will be get at once (bjju.post)
    extern DWORD		dwPOST_SEND_STATE_HOW_MANY_GET;		// how many post state which be sent mail will be get at once
    extern DWORD		dwPOST_RECV_STATE_HOW_MANY_GET;		// how many post state which be received mail will be get at once
	extern DWORD		dwPOST_EXPIRE_DURATION_DAY;
	extern DWORD		dwPOST_SEND_COOL_TIME_MS;			// 1min (ms)
	extern DWORD		dwPOST_RECV_COOL_TIME_MS;			// 1min (ms)
	extern DWORD		dwPOST_SEND_REQUIRE_LEVEL;		// Require cha's Level for post sending

    BOOL LOADFILE(const std::string& FileName, BOOL bServer=TRUE);
}