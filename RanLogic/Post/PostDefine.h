#pragma once
#include <winnt.h>
#include "../Item/GLItemCustom.h"

enum EMPOST_STATE
{
    EMPOST_UNREAD			= 0,	// This post is valid and not read yet.
    EMPOST_READ				= 1,	// This post is valid and read.
    EMPOST_USER_DELETE		= 2,	// This post is no more valid and deleted by user.
    EMPOST_AUTO_DELETE		= 3,	// This post is no more valid and deleted by system of expiration.
    EMPOST_RETURN			= 4,	// This post is returned.	
};


enum EMPOST_TYPE
{
    EMPOST_TEXT				= 0,	// Post includes only Text
    EMPOST_MONEY			= 1,	// Post includes Text and Money
    EMPOST_ITEM				= 2,	// Post includes Text and Item
    EMPOST_MONEYITEM		= 3,	// Post includes Text and Money and Item
};


enum EMPOST_CLASS
{
    EMPOST_USER				= 0,	// User Post
    EMPOST_SYSTEM			= 1,	// System Post
    EMPOST_GM				= 2,	// Gm Post ( Not yet )
};


//
//mjeon.post
//
enum EMPOST_RESULT
{
    EMPOST_OK					= 0,	// OK
    EMPOST_RECVER_NOTEXIST		= 1,	// There does not exist the character(Recver)
    EMPOST_SENDER_NOTEXIST		= 2,	// There does not exist the character(Sender)	
    EMPOST_NOTENOUGH_MONEY		= 3,	// Not enough money (for attachedMoney or payment of Charge)
    EMPOST_WRONG_ATTACH01		= 4,	// Wrong item for attach01
    EMPOST_NOTRADE_ATTACH01		= 5,	// Attach01 cannot be traded
    EMPOST_WRONG_CHARGE			= 6,	// Wrong charge (There is no attached item.)
    EMPOST_SAME_ACCOUNT			= 7,	// Sender's account is same as Recver's
    EMPOST_LOW_LEVEL			= 8,	// Sender's level is lower than 10
    EMPOST_EXCEED_MONEY			= 9,	// Attached money is too big
    EMPOST_COOLTIME_REMAIN		= 10,	// Postsend cooltime remains
    EMPOST_POST_NOT_EXIST		= 11,	// The Post does not exist with the PostID
    EMPOST_REACCEPT_ITEM		= 12,	// Already accepted the item before
    EMPOST_REACCEPT_MONEY		= 13,	// Already accepted the money before
    EMPOST_NOTENOUGH_INVEN		= 14,	// Recver does not have enough inventory to take attach01.
    EMPOST_SYSTEMPOST_RETURN	= 15,	// User tried to return System Post.
    EMPOST_INVALID_RETURN		= 16,	// User tried to return Deleted or Returned Post.
    EMPOST_EMPTY_RETURN			= 17,	// User tried to return empty post after taking attachment.
    EMPOST_DB_ERROR				= 18,	// DB error occured.
    EMPOST_NOT_AVAILABLE		= 19,	// PostSystem is not available now. (e.g.>too long distance from the postbox to the character.)
    EMPOST_EMPTY_TITLE_CONTENT	= 20,	// Title or Content is empty.
    EMPOST_MINUS_MONEY			= 21,	// Charge or AttachMoney is less than 0.
    EMPOST_EMPTY_AGENTSIDE		= 22,	// There's no postinfo in the post vector managed by AgentServer.
    EMPOST_INVALID_ACCESS		= 23,	// User tried to deleted post.
    EMPOST_ETC					= 24,
};

enum EMINVEN_POSTBOX_OPEN_FB
{
    EMINVEN_POSTBOX_OPEN_FB_FAIL	 = 0,  // 실패
    EMINVEN_POSTBOX_OPEN_FB_SUCCESS	 = 1,  // 성공
};

enum EMINVEN_POSTBOX_OPEN_CARD_FB
{
    EMINVEN_POSTBOX_OPEN_CARD_FB_FAIL		= 0,  // 실패
    EMINVEN_POSTBOX_OPEN_CARD_FB_SUCCESS	= 1,  // 성공
    EMINVEN_POSTBOX_OPEN_CARD_FB_NOITEM		= 2,  // 아이템이 존재하지 않음
};

class PostInfo
{
public:
// 	DWORD			PostID() const					{ return dwPostID; }
// 	void			PostID(DWORD val)				{ dwPostID = val; }
// 	DWORD			SendChaNum() const				{ return dwSendChaNum; }
// 	void			SendChaNum(DWORD val)			{ dwSendChaNum = val; }
// 	std::string		SendChaName() const				{ return strSendChaName; }
// 	void			SendChaName(std::string val)	{ strSendChaName = val; }
// 	DWORD			SendUserNum() const				{ return dwSendUserNum; }
// 	void			SendUserNum(DWORD val)			{ dwSendUserNum = val; }
// 	DWORD			RecvChaNum() const				{ return dwRecvChaNum; }
// 	void			RecvChaNum(DWORD val)			{ dwRecvChaNum = val; }
// 	__time64_t		SendDate() const				{ return tSendDate; }
// 	void			SendDate(__time64_t val)		{ tSendDate = val; }
// 	WORD			Style() const					{ return wStyle; }
// 	void			Style(WORD val)					{ wStyle = val; }
// 	WORD			PostType() const				{ return wPostType; }
// 	void			PostType(WORD val)				{ wPostType = val; }
// 	bool			Read() const					{ return bRead; }
// 	void			Read(bool val)					{ bRead = val; }
// 	LONGLONG		Money() const					{ return llMoney; }
// 	void			Money(LONGLONG val)				{ llMoney = val; }
// 	SITEMCUSTOM		ItemCutom() const				{ return sItemCutom; }
// 	void			ItemCutom(SITEMCUSTOM val)		{ sItemCutom = val; }
// 	std::string		Title() const					{ return strTitle; }
// 	void			Title(std::string val)			{ strTitle = val; }
// 	std::string		Content() const					{ return strContent; }
// 	void			Content(std::string val)		{ strContent = val; }
	PostInfo()
		: dwPostID(0)
		, dwSendChaNum(0)
		, dwSendUserNum(0)
		, dwRecvChaNum(0)
		, tSendDate(0)
		, wStyle(0)
		, wMailType(0)
		, wAttachType(0)
		, bRead(0)
		, wMoneyType(0)
		, llMoney(0)
		, sItemCutom(SITEMCUSTOM(NATIVEID_NULL())) {}

	DWORD			dwPostID;
	DWORD			dwSendChaNum;
	std::string		strSendChaName;
	DWORD			dwSendUserNum;
	DWORD			dwRecvChaNum;
	__time64_t		tSendDate;
	WORD			wStyle;
	WORD			wMailType;		// EMPOST_MAIL_TYPE
	WORD			wAttachType;	// EMPOST_ATTACH_TYPE
	bool			bRead;
	WORD			wMoneyType;		// EMPOST_MONEY_TYPE
	LONGLONG		llMoney;
	SITEMCUSTOM		sItemCutom;
	std::string		strTitle;
	std::string		strContent;
	bool			bAttachHole;

	MSGPACK_DEFINE( 
		dwPostID, 
		dwSendChaNum, 
		strSendChaName, 
		dwSendUserNum, 
		dwRecvChaNum, 
		tSendDate,
		wStyle,
		wAttachType,
		bRead,
		wMoneyType,
		llMoney,
		sItemCutom,
		strTitle,
		strContent )
	
};

enum EMPOST_SEND_CODE
{
	EMPOST_SEND_RECV_CHAR_NOTEXIST		= -2,
	EMPOST_SEND_ERROR					= -1,
	EMPOST_SEND_OK						= 0,

};

enum EMPOST_DEL_CODE
{
	EMPOST_DEL_ERROR					= -1,
	EMPOST_DEL_OK						= 0,

};

enum EMPOST_ATTACH_CODE
{
	EMPOST_ATTACH_ERROR					= -1,
	EMPOST_ATTACH_OK					= 0,
	EMPOST_ATTACH_ITEM_IS_NULL			= 1,
	EMPOST_ATTACH_INVEN_IS_FULL			= 2,
	EMPOST_ATTACH_NOT_ENOUGH_MONEY		= 3,
	EMPOST_ATTACH_NOT_ENOUGH_POINT		= 4,

};

enum EMPOST_RETURN_CODE
{
	EMPOST_RETURN_ERROR					= -1,
	EMPOST_RETURN_OK						= 0,

};

enum EMPOST_MAIL_TYPE
{
	EMPOST_MAIL_NOT_USE		= 0,
	EMPOST_MAIL_USER,
	EMPOST_MAIL_GM,
	EMPOST_MAIL_SYSTEM,
	EMPOST_MAIL_RETURN,

};

enum EMPOST_ATTACH_TYPE
{
	EMPOST_ATTACH_NOT_USE	= 0x000,

	EMPOST_ATTACH_TEXT		= 0x01,
	EMPOST_ATTACH_MONEY		= 0x02,	// 돈 첨부(청구가될수도있음)
	EMPOST_ATTACH_ITEM		= 0x04,	// 아이템 첨부
};



//charge
enum EMPOST_MONEY_TYPE
{
	EMPOST_MONEY_NOT_USE = 0,
	EMPOST_MONEY_GOLD,			// 골드 첨부
	EMPOST_MONEY_POINT,			// 포인트 첨부
	EMPOST_MONEY_CHARGE_GOLD,	// 골드 청구
	EMPOST_MONEY_CHARGE_POINT,	// 포인트 청구
};

