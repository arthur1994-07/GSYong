#ifndef _DB_ACTION_GAME_POST_H_
#define _DB_ACTION_GAME_POST_H_

#pragma once

#include "../../../RanLogic/Msg/GLContrlMsg.h"
#include "../../../RanLogic/Msg/PostRnMsg.h"

#include "DbAction.h"

class GLCharAG;
class GLGaeaServer;
//
//mjeon.Post
//
class CPostOpenAndTake : public db::DbAction
{
public:	
	CPostOpenAndTake(GLMSG::SNET_POST_OPEN_TAKE_AF *msg/*current post status*/, BOOL bMoneyAccept /*user's request*/, BOOL bAttach01Accept /*user's request*/, DWORD dwOpenAndTakeUserNum );
	virtual ~CPostOpenAndTake() {};
	int Execute(NetServer* m_pServer);

protected:
	/* Use  m_dwClient in CDbAction */

	SAPOST		m_aPOST;
	BOOL		m_bMoneyAccept;
	BOOL		m_bAttach01Accept;
	DWORD		m_dwOpenAndTakeUserNum;

/*
	int			m_iRecver;			// Recver's CharID
	int			m_iSender;			// to receive Charge for the attached item.
	LONGLONG	m_llAttachMoney;
	LONGLONG	m_llCharge;
	LONGLONG	m_llPostID;
	BOOL		m_bMoneyAccept;
	BOOL		m_bAttach01Accept;
	SITEMCUSTOM m_sAttach01;
*/
};


//
//mjeon.Post.Send
//DBAction for PostSend
//
class CPostSend : public db::DbAction
{
public:
	CPostSend( DWORD dwClientID, /*Sender*/ SPOSTINFO POSTSEND, const SITEMCUSTOM& sItem);
	virtual ~CPostSend() {};
	int Execute(NetServer* m_pServer);

protected:
	
	SPOSTINFO	m_PostInfo;
	SITEMCUSTOM	m_sItem;
};

class CPostSendRollBack : public db::DbAction
{
public:
	CPostSendRollBack(LONGLONG &llPostID);
	virtual ~CPostSendRollBack();
	virtual int Execute(NetServer* m_pServer) override;

protected:
	LONGLONG m_llPostID;	
};

//
//mjeon.Post
//
class CGetChaNum: public db::DbAction
{
public:
	CGetChaNum (DWORD dwClient, DWORD dwGaeaID, const char *szReqChaName, int nReqID); //, const char *szUserIP, USHORT uPort=0);
	virtual ~CGetChaNum() {;}
	virtual int Execute(NetServer *pServer);

protected:
	DWORD	m_dwGaeaID;						//Sender

	int		m_nReqID;

	int		m_nChaNum;						//Recver
	TCHAR	m_szChaName[CHR_ID_LENGTH+1];	//Recver
	int		m_nUserNum;						//Recver

};


//
//mjeon.Post.Get
//
class CPostGet: public db::DbAction
{
public:
	//typedef std::vector<SAPOST>	VPOST;
	//typedef VPOST::iterator		VPOST_ITER;


	// Get recent 30 undeleted-mails from DB with the key value, iRecver.
	CPostGet ( int iRecver, DWORD dwClient, DWORD dwGaeaID, BOOL bGetOnlyInfo, std::tr1::shared_ptr<VPOST> &pvPost /*VPOST &vPost*/, DWORD dwRecverUserNum );
	virtual ~CPostGet() {;}
	virtual int Execute(NetServer *pServer);

protected:
	int		m_iRecver;		//Recver's ChaNum

	BOOL	m_bGetOnlyInfo;	//Get only post info but not data.	

	//VPOST	&m_vPost;
	std::tr1::shared_ptr<VPOST> m_pvPost;

	int		m_nRowCount;	// Count of row selected (0~31) : 31 means more posts in DB.
	int		m_cntAllPost;	// Count of all posts in DB ( 0~ )
	int		m_cntNewPost;	// Count of new posts in DB ( 0~ )


	DWORD	m_dwGaeaID;
	DWORD	m_dwRecverUserNum;
	//DWORD	m_dwClientID;	//m_dwClient is already in CDbAction.
};


class CPostGetState: public db::DbAction
{
public:
    //
    //mjeon.dbman
    //
    //typedef std::vector<SAPOSTSTATE>	VPOSTSTATE;
    //typedef VPOSTSTATE::iterator		VPOSTSTATE_ITER;

    // Get recent 30 undeleted-mails from DB with the key value, iRecver.
    CPostGetState(int iRecver, DWORD dwClient, DWORD dwChaDbNum);
    virtual ~CPostGetState();
    virtual int Execute(NetServer *pServer);

protected:
    int		m_iRecver;		//Recver's ChaNum
    int		m_nRowCount;
    VPOSTSTATE	m_vPostState;

    DWORD m_dwChaDbNum;
    //DWORD	m_dwClientID;	//m_dwClient is already in CDbAction.
};


class CPostGetSendState: public db::DbAction
{
public:
    // Get recent 15 undeleted-mails from DB with the key value, iRecver.
    CPostGetSendState(int iRecver, DWORD dwClient, DWORD dwChaDbNum);
    virtual ~CPostGetSendState();
    virtual int Execute(NetServer *pServer);

protected:
    int		m_iRecver;		//Recver's ChaNum
    int		m_nRowCount;
    VPOSTSTATE	m_vPostState;

    DWORD m_dwChaDbNum;
};


class CPostGetRecvState: public db::DbAction
{
public:
    // Get recent 15 undeleted-mails from DB with the key value, iRecver.
    CPostGetRecvState(int iRecver, DWORD dwClient, DWORD dwChaDbNum);
    virtual ~CPostGetRecvState();
    virtual int Execute(NetServer *pServer);

protected:
    int		m_iRecver;		//Recver's ChaNum
    int		m_nRowCount;
    VPOSTSTATE	m_vPostState;

    DWORD m_dwChaDbNum;
};


class CPostOpen: public db::DbAction
{
public:
	CPostOpen ( LONGLONG llPostID );
	virtual ~CPostOpen() {;}
	virtual int Execute(NetServer *pServer);

protected:
	LONGLONG	m_llPostID;
};


class CPostDelete : public db::DbAction
{
public:
	CPostDelete ( DWORD dwClientID, LONGLONG llPostID );
	virtual ~CPostDelete() {;}
	virtual int Execute(NetServer *pServer);

protected:
	LONGLONG	m_llPostID;
};


class CPostReturn : public db::DbAction
{
public:
	CPostReturn(DWORD dwClientID, LONGLONG llPostID);
	virtual ~CPostReturn();
	virtual int Execute(NetServer *pServer);

protected:
	LONGLONG	m_llPostID;
};



class CPostMaintenance: public db::DbAction
{
public:
	CPostMaintenance ();
	virtual ~CPostMaintenance() {;}
	virtual int Execute(NetServer *pServer);

protected:
	char m_szCurrentDate[24];
	//char m_szTargetDate[24];
};

//
//PostClean will be called in CPostMaintenance after processing unhandled chargement.
//
class CPostClean: public db::DbAction
{
public:
    enum {
        POST_CLEAN_STR_LENGTH = 24,
    };

	CPostClean();
	virtual ~CPostClean() {;}
	virtual int Execute(NetServer *pServer);

protected:
	char m_szCurrentDate[POST_CLEAN_STR_LENGTH];
	char m_szTargetDate[POST_CLEAN_STR_LENGTH];
};

class CPostSendSystemMail : public db::DbAction
{
public:
	CPostSendSystemMail(SPOSTINFO POSTSEND, SITEMCUSTOM &itemcustom);
	virtual ~CPostSendSystemMail() {};
	int Execute(NetServer* m_pServer);

protected:
	
	SPOSTINFO			m_PostInfo;
	SITEMCUSTOM			m_AttachedItem;
};

// 에이전트서버에서 우편 전송할 때 사용
class CPostSendSystemMailAgent : public db::DbAction
{
public:
	CPostSendSystemMailAgent( SPOSTINFO POSTSEND, SITEMCUSTOM& itemcustom );
	virtual ~CPostSendSystemMailAgent() {};
	int Execute( NetServer* m_pServer );

protected:
	SPOSTINFO m_PostInfo;
	SITEMCUSTOM m_AttachedItem;
};

namespace db
{

	class CPostRnSelectAll : public db::DbAction
	{
	public:
		CPostRnSelectAll( DWORD dwRecvChaNum ): m_dwRecvChaNum(dwRecvChaNum) {};
		virtual ~CPostRnSelectAll() {};
		int Execute(NetServer* m_pServer);

	protected:
		DWORD m_dwRecvChaNum;
	};

	class CPostRnSetRead : public db::DbAction
	{
	public:
		CPostRnSetRead( DWORD PostID ): m_nPostID(PostID) {};
		virtual ~CPostRnSetRead() {};
		int Execute(NetServer* m_pServer);

	protected:
		DWORD m_nPostID;
	};

	// 필드서버 전용
	class CPostRnDelAttach : public db::DbAction
	{
	public:
		// 사용하지는 않지만 에러 검출용
		CPostRnDelAttach( GLGaeaServer* pGaeaServer, const GLMSG::NET_POST_RENEW_ATTACH_ACK& backup )
			: m_Backup(backup)
		{
			GASSERT( pGaeaServer );
		};
		virtual ~CPostRnDelAttach() {};
		int Execute(NetServer* m_pServer);

	protected:
		GLMSG::NET_POST_RENEW_ATTACH_ACK m_Backup;
	};

	class CPostRnDel : public db::DbAction
	{
	public:
		CPostRnDel( DWORD PostID, DWORD dwReqClientSlot  )
			: m_nPostID(PostID)
			, m_dwReqClientSlot(dwReqClientSlot) {};
		virtual ~CPostRnDel() {};
		int Execute(NetServer* m_pServer);

	protected:
		DWORD m_nPostID;
		DWORD m_dwReqClientSlot;
	};

	class CPostRnSend : public db::DbAction
	{
	public:
		CPostRnSend( PostInfo post, DWORD dwReqClientSlot )
			: m_PostInfo(post)
			, m_dwReqClientSlot(dwReqClientSlot) {};
		virtual ~CPostRnSend() {};
		int Execute(NetServer* m_pServer);

	protected:
		PostInfo m_PostInfo;
		DWORD m_dwReqClientSlot;
	};


	class CPostRnCharCheck : public db::DbAction
	{
	public:
		CPostRnCharCheck( const GLMSG::NET_POST_RENEW_SEND_REQ_FOR_CLIENT& msg, const DWORD& dwSendChaDbNum )
			: m_Backup(msg)
			, m_dwSendChaDbNum(dwSendChaDbNum)
		{};
		virtual ~CPostRnCharCheck() {};
		int Execute(NetServer* m_pServer);

	protected:
		// 정보를 처리 후에 다시 사용해야 하기때문에.
		GLMSG::NET_POST_RENEW_SEND_REQ_FOR_CLIENT m_Backup;
		DWORD m_dwSendChaDbNum;
	};


	class CPostRnReturn : public db::DbAction
	{
	public:
		CPostRnReturn( DWORD dwFieldSlot, DWORD dwPostID, DWORD dwReturnChaNum, std::string strReturnChaName, DWORD dwReturnUserNum, DWORD dwRecvChaNum )
			: m_dwFieldSlot(dwFieldSlot)
			, m_dwPostID(dwPostID)
			, m_dwReturnChaNum(dwReturnChaNum)
			, m_strReturnChaName(strReturnChaName)
			, m_dwReturnUserNum(dwReturnUserNum)
			, m_dwRecvChaNum(dwRecvChaNum) {};
		virtual ~CPostRnReturn() {};
		int Execute(NetServer* m_pServer);

	protected:
		DWORD m_dwFieldSlot;
		DWORD m_dwPostID;
		DWORD m_dwReturnChaNum;
		std::string m_strReturnChaName;
		DWORD m_dwReturnUserNum;
		DWORD m_dwRecvChaNum;
	};

}

#endif // _DB_ACTION_GAME_POST_H_