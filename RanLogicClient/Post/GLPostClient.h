#pragma once

#include <map>
#include <list>
#include <vector>
#include <queue>

#include "../GLGaeaClient.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Post/GLPostData.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"

#include "../Widget/GLWidgetScript.h"

struct SAPOSTCLIENT : public SAPOST
{
public:
	enum
	{
		EMSTATE_WAIT_DELETE		= 0x0001, // 삭제 대기중
		EMSTATE_WAIT_RETURN		= 0x0002, // 반환 대기중
	};

public:
	DWORD dwSTATE;

public:
	SAPOSTCLIENT  ()
		: SAPOST  ()
		, dwSTATE ( 0 )
	{

	}

	SAPOSTCLIENT  ( const SAPOST& rValue )
		: SAPOST  ( rValue )
		, dwSTATE ( 0 )
	{

	}

	const SAPOSTCLIENT& operator = ( const SAPOST& rValue )
	{
		*((SAPOST*)this) = rValue;
		return *this;
	}

public:
	const BOOL IsUnRead()
	{
		if ( m_byPostState == EMPOST_UNREAD )
		{
			return TRUE;
		}
		else if ( m_byPostState == EMPOST_RETURN )
		{
			if ( m_RecvBackDate == 0 )
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	const BOOL IsReturnPost ()
	{
		if ( m_ReturnDate )
		{
			return TRUE;
		}

		return FALSE;
	}

	const BOOL IsDeletePost ()
	{
		if ( m_byPostState == EMPOST_USER_DELETE )
		{
			return TRUE;
		}

		if ( m_byPostState == EMPOST_AUTO_DELETE )
		{
			return TRUE;
		}

		if ( m_DeleteDate )
		{
			return TRUE;
		}

		return FALSE;
	}

	const __time64_t GetRecentSendDate () 
	{
		if ( m_ReturnDate )
		{
			return m_ReturnDate;	
		}

		return m_SendDate;
	}

	const DWORD GetExpireDay(GLGaeaClient* pGaeaClient);
};

struct SAPOSTHISTORY : public SAPOSTSTATE
{

public:
	enum
	{
		ID_NULL = -1,
	};

public:
	SAPOSTHISTORY ()
		: SAPOSTSTATE ()
	{

	}

	SAPOSTHISTORY ( const SAPOSTSTATE& rValue )
		: SAPOSTSTATE ( rValue )
	{

	}

	const SAPOSTHISTORY& operator = ( const SAPOSTSTATE& rValue )
	{
		*((SAPOSTSTATE*)this) = rValue;
		return *this;
	}

public:
	void SET ( const SAPOSTCLIENT& sPost )
	{
		this->llPostID	  = sPost.m_llPostID;

		this->byPostState = sPost.m_byPostState;
		this->byPostClass = sPost.m_byPostClass;
		this->byPostType  = sPost.m_byPostType;

		this->iSender  = sPost.m_iSender;
		this->iRecver  = sPost.m_iRecver;

		this->iDesign  = sPost.m_iDesign;
		this->llCharge = sPost.m_llCharge;

		this->SendDate = sPost.m_SendDate;
		this->RecvDate = sPost.m_RecvDate;
		this->ReturnDate = sPost.m_ReturnDate;
		this->RecvBackDate = sPost.m_RecvBackDate;
		this->DeleteDate = sPost.m_DeleteDate;

		strcpy_s ( this->SenderName, POST_NAME_LENGTH, sPost.m_SenderName );
		strcpy_s ( this->RecverName, POST_NAME_LENGTH,	sPost.m_RecverName );
	}

	void SET(GLGaeaClient* pGaeaClient, GLMSG::SNET_POST_SEND& nmg);

public:
	const BOOL IsUnRead()
	{
		if ( byPostState == EMPOST_UNREAD )
		{
			return TRUE;
		}
		else if ( byPostState == EMPOST_RETURN )
		{
			if ( RecvBackDate == 0 )
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	const BOOL IsReturnPost ()
	{
		if ( ReturnDate )
		{
			return TRUE;
		}

		return FALSE;
	}

	const BOOL IsDeletePost ()
	{
		if ( byPostState == EMPOST_USER_DELETE )
		{
			return TRUE;
		}
		
		if ( byPostState == EMPOST_AUTO_DELETE )
		{
			return TRUE;
		}

		if ( DeleteDate )
		{
			return TRUE;
		}

		return FALSE;
	}

	const __time64_t GetRecentSendDate () 
	{
		if ( ReturnDate != 0 )
		{
			return ReturnDate;	
		}
		
		return SendDate;
	}
};

class GLGaeaClient;

class GLPostClient : public GLPost
{

public:
	struct SPOSTSENDREQ
	{
		DWORD					ReqID;
		GLMSG::SNET_POST_SEND	Data;
	};

	struct SPOSTINFO
	{
		DWORD nNew;
		DWORD nRead;
		DWORD nReportNewPost;
		DWORD nWaiting;
		DWORD nTotal;

		SPOSTINFO ()
			: nNew		( 0 )
			, nRead		( 0 )
			, nWaiting	( 0 )
			, nReportNewPost( 0	)
		{
		}
	};

    typedef std::map<LONGLONG,SAPOSTHISTORY>	SPOSTHISTORYMAP;
    typedef	SPOSTHISTORYMAP::iterator			SPOSTHISTORYMAPITER;
    typedef std::pair<LONGLONG,SAPOSTHISTORY>	SPOSTHISTORYPAIR;

    typedef std::list<SAPOSTHISTORY>	        SPOSTHISTORYLIST;
    typedef	SPOSTHISTORYLIST::iterator			SPOSTHISTORYLISTITER;

    typedef std::vector<SAPOSTHISTORY*>			SPOSTHISTORYVEC;
    typedef SPOSTHISTORYVEC::iterator			SPOSTHISTORYVECITER;
    typedef SPOSTHISTORYVEC::reverse_iterator	SPOSTHISTORYVECRITER;

    struct SPOSTHISTORYINFOSET
    {
        enum SPOSTHISTORYINFOTYPE
        {
            SPOSTHISTORYALLINFO,
            SPOSTHISTORYSENDINFO,
            SPOSTHISTORYRECVINFO,
        };

        SPOSTHISTORYMAP	     HistoryMap;
        SPOSTHISTORYLIST     HistoryVirtualList;
        SPOSTHISTORYVEC	     HistoryVec;
        BOOL                 bReqHistory; // Note : 우편 현황 요청중
        DWORD                dwStateHowManyGet; // Postboxsize = 30
        DWORD			     dwLastHistTime;
        SPOSTHISTORYINFOTYPE spostHistoryType;

        SPOSTHISTORYINFOSET(SPOSTHISTORYINFOTYPE spostHistoryType)
            : bReqHistory( FALSE ),
              dwLastHistTime( 0 ),
              spostHistoryType( spostHistoryType )
        {
            switch( spostHistoryType )
            {
            case SPOSTHISTORYALLINFO:
                dwStateHowManyGet = GLCONST_POST::dwPOST_STATE_HOW_MANY_GET;
                break;

            case SPOSTHISTORYSENDINFO:
                dwStateHowManyGet = GLCONST_POST::dwPOST_SEND_STATE_HOW_MANY_GET;
                break;

            case SPOSTHISTORYRECVINFO:
                dwStateHowManyGet = GLCONST_POST::dwPOST_RECV_STATE_HOW_MANY_GET;
                break;
            }
        }

        void ClearHistory()
        {
            if( !HistoryMap.empty() )
            {
                HistoryMap.clear();
            }

            if( !HistoryVirtualList.empty() )
            {
                HistoryVirtualList.clear();
            }

            if (!HistoryVec.empty() )
            {
                HistoryVec.clear();
            }
        }

        DWORD GetMaxHistory()
        {
            return HistoryVec.size();
        }

        BOOL IsHistCoolTime()
        {
            DWORD dwCurrentPostHist = ::GetTickCount();	

            if ( dwCurrentPostHist > dwLastHistTime + GLCONST_POST::dwPOST_RECV_COOL_TIME_MS )
            {				
                return FALSE;
            }

            return TRUE;
        }
    };

public:
	typedef std::map<LONGLONG,SAPOSTCLIENT>		SPOSTRCVMAP;
	typedef	SPOSTRCVMAP::iterator				SPOSTRCVMAPITER;
	typedef std::pair<LONGLONG,SAPOSTCLIENT>	SPOSTRCVPAIR;

	typedef std::vector<SAPOSTCLIENT*>			SPOSTRCVVEC;
	typedef SPOSTRCVVEC::iterator				SPOSTRCVVECITER;
	typedef SPOSTRCVVEC::reverse_iterator		SPOSTRCVVECRITER;

public:
	typedef std::queue<SPOSTSENDREQ>			SPOSTSENDQUEUE;

public:    
    GLPostClient(GLGaeaClient* pGaeaClient);
    virtual ~GLPostClient(void);

protected:
    GLGaeaClient*    m_pGaeaClient;
	SPOSTRCVMAP		 m_ReceiveMap;
	SPOSTRCVVEC		 m_ReceiveVec;

    SPOSTHISTORYINFOSET m_HistoryInfoSet;
    SPOSTHISTORYINFOSET m_SendHistoryInfoSet;
    SPOSTHISTORYINFOSET m_RecvHistoryInfoSet;

	SPOSTINFO		 m_sPostInfo;

protected:
	BOOL			 m_bReqPost;		// Note : 우편 받기 요청중
	BOOL			 m_bReqSend;		// Note : 우편 전송 요청중

protected:
	SPOSTSENDQUEUE	 m_SendQueue;

protected:
	DWORD			 m_dwLastSendTime;
	DWORD			 m_dwLastRecvTime;

public:
	void			 ClearReceivePost	();

public:
	const BOOL		 IsRequirePost		  () { return m_bReqPost;		}
	const BOOL		 IsRequireSend		  () { return m_bReqSend;		}

public:
	SPOSTRCVMAP&	 GetReceiveMap () { return m_ReceiveMap; }
	SPOSTRCVVEC&	 GetReceiveVec () { return m_ReceiveVec; }

	const SAPOSTCLIENT*	 GetSAPOSTCLIENT_ReceiveVec( DWORD dwIndex ) const;

    SPOSTHISTORYINFOSET& GetHistoryInfoSet () { return m_HistoryInfoSet; }
    SPOSTHISTORYINFOSET& GetSendHistoryInfoSet () { return m_SendHistoryInfoSet; }
    SPOSTHISTORYINFOSET& GetRecvHistoryInfoSet () { return m_RecvHistoryInfoSet; }

public:
	HRESULT			 FrameMove ( float fTime, float fElapsedTime );

public:
	const DWORD		 GetMaxPost		  ();
	const DWORD		 GetNewPost		  () { return m_sPostInfo.nNew;			}
	const DWORD		 GetWaitingPost	  () { return m_sPostInfo.nWaiting;		}
	const DWORD		 GetTotalPost	  () { return m_sPostInfo.nTotal;			}
	const DWORD		 GetReadPost	  () { return m_sPostInfo.nRead;			}

public:
	const DWORD		 GetReceivedNewPost ();

public:
	const BOOL		 ReqReceivePost		     ();
	const BOOL		 ReqReceivePostInfo	     ();
    const BOOL		 ReqReceiveCurrentHistory( SPOSTHISTORYINFOSET* currentInfoSet );
    const BOOL		 ReqReceiveHistory	     ();
    const BOOL		 ReqReceiveSendHistory   ();
    const BOOL		 ReqReceiveRecvHistory   ();
	const BOOL		 ReqSendPost		     ( GLMSG::SNET_POST_SEND* nmg );
	const BOOL		 ReqOpenPost		     ( SAPOSTCLIENT* pPost );
	const BOOL		 ReqReturnPost		     ( SAPOSTCLIENT* pPost );
	const BOOL		 ReqDeletePost		     ( SAPOSTCLIENT* pPost, const BOOL bAttachDelete=TRUE );
	const BOOL		 ReqTakeItem		     ( SAPOSTCLIENT* pPost );

public:
	const BOOL		 MsgReceivePost		     (NET_MSG_GENERIC* nmg);
	const BOOL		 MsgReceivePostInfo	     (NET_MSG_GENERIC* nmg);
    const BOOL		 MsgReceiveCurrentHistory(NET_MSG_GENERIC* nmg, SPOSTHISTORYINFOSET* currentInfoSet);
    const BOOL		 MsgReceiveHistory	     (NET_MSG_GENERIC* nmg);
    const BOOL		 MsgReceiveSendHistory   (NET_MSG_GENERIC* nmg);
    const BOOL		 MsgReceiveRecvHistory   (NET_MSG_GENERIC* nmg);
	const BOOL		 MsgSendPostFB_CharID    (NET_MSG_GENERIC* nmg);
	const BOOL		 MsgSendPostFB_Result    (NET_MSG_GENERIC* nmg);
	const BOOL		 MsgReturnPost		     (NET_MSG_GENERIC* nmg);
	const BOOL		 MsgDeletePost		     (NET_MSG_GENERIC* nmg);
	const BOOL		 MsgTakeItem		     (NET_MSG_GENERIC* nmg);
	const BOOL		 MsgReceivePostNotify    (NET_MSG_GENERIC* nmg);

public:
	// Note : 우편현황을 재요청하지 않고 갱신하기 위해 사용한다.
	//		  새로운 현황 삽입시 최대우편 갯수를 넘지않고 오래된 우편을 삭제한다.
	const BOOL		 InsertVirtualHistory ( GLMSG::SNET_POST_SEND* nmg );
	const BOOL		 PopVirtualHistory	  ();
	const BOOL		 UpdateHistory		  ( SAPOSTCLIENT* pPost );

public:
	const BOOL		 IsSendAblePost		  ( GLMSG::SNET_POST_SEND* nmg );
	const BOOL		 IsReturnAblePost	  ( SAPOSTCLIENT* pPost );
	const BOOL		 IsDeleteAblePost	  ( SAPOSTCLIENT* pPost, const BOOL bAttachDelete=TRUE );
    bool IsAttachPost( SAPOSTCLIENT* pPost );

public:
	const BOOL		 IsSendCoolTime		  ();
	const BOOL		 IsRecvCoolTime		  ();

public:
	// 받기 페이지;
	LuaTable GetReceivePagePost();						// 받은우편 리스트;
	LuaTable GetReceivePageSelectPost( int nSelect );	// 선택 우편;

	// 보내기 페이지;
	LuaTable GetSendPageDesignType();					// 우편보내기 디자인;
	const DWORD GetSendPageSlotItem();
	const WORD GetSendPageSlotItemTurnNum();

	// 우편현황 페이지;
	LuaTable GetHistoryInfoSet( GLPostClient::SPOSTHISTORYINFOSET& historyInfoSet );
	LuaTable GetHistoryPageInfoSet();
	LuaTable GetHistorySendPageInfoSet();
	LuaTable GetHistoryRecievePageInfoSet();
	const DWORD GetHistorySendPageMaxPage();
	const DWORD GetHistoryRecievePageMaxPage();

	// 미리보기 페이지;
	LuaTable GetPreviewPagePost();						// 미리보기 우편;

	//
	const bool			HaveReceivePostAttachments( const unsigned int nSelect );	// 선택우편 첨부물 확인;
	const LONGLONG		GetBaseFee();
	const LONGLONG		GetMaxAttachMoney();
	const LONGLONG		GetCommission ( const bool IsAttachDesign, const bool IsAttachItem, const LONGLONG llAttachMoney );

public:
	bool				ReqReceivePageRefresh();
	void				ReqSelectPostOpen( const unsigned int nSelect );
	void				ReqSelectPostDelete( const unsigned int nSelect, bool bAttachDelete = true );
	void				ReqSelectPostTake( const unsigned int nSelect );
	void				ReqSelectPostReturn( const unsigned int nSelect );
	void				ReqSelectPostTakeItem( const unsigned int nSelect );

	void				ReqCheckedPostDelete( const unsigned int nSelect );

	const bool			ReqSendPostTable( LuaTable tbPost );
	void ResetCharPostItemData();

	void				ReqUseInvenPostBoxCard();

public :
	bool IsWidgetOpenAble( const bool bPrint );

private :
	int CalcSelectIndex( int nSelect );
};
