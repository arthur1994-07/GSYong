// bjju.sns

#pragma once

#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/SNS/GLSNSData.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../EngineLib/Common/gltexfile.h"

#include "../../SNSLib/SNS.h"

#include <string>
#include <queue>

//---------------------------------------------------------------------------------------------------//

enum EMSNSREQTYPE
{
	EMSNSREQTYPE_AUTOREQ = 0x0001,
};

enum EMSNSREQ
{
	EMSNSREQ_NONE,
	EMSNSREQ_FACEBOOK_WALL,
	EMSNSREQ_FACEBOOK_PICTURE,
	EMSNSREQ_TWITTER_TWEET,
	EMSNSREQ_FACEBOOK_WALL_LEVUP,
	EMSNSREQ_FACEBOOK_WALL_ENCHANTED,
	EMSNSREQ_FACEBOOK_WALL_WELCOME,
	EMSNSREQ_TWITTER_FOLLOW,
	EMSNSREQ_TWITTER_UNFOLLOW,
};

enum EMSNSREQFB
{
	EMSNSREQFB_NONE,
	EMSNSREQFB_FB_WALL_OK,
	EMSNSREQFB_FB_WALL_FAIL,
	EMSNSREQFB_FB_PICTURE_OK,
	EMSNSREQFB_FB_PICTURE_FAIL,
	EMSNSREQFB_TT_TWEET_OK,
	EMSNSREQFB_TT_TWEET_FAIL,
	EMSNSREQFB_TT_FOLLOW_OK,
	EMSNSREQFB_TT_FOLLOW_FAIL,
	EMSNSREQFB_TT_UNFOLLOW_OK,
	EMSNSREQFB_TT_UNFOLLOW_FAIL,
};

//---------------------------------------------------------------------------------------------------//

struct SSNSMSGREQ
{
	EMSNSREQ	emSNSREQ;
	
	std::string	strCONTENTS;
	std::string	strSCREENSHOTPATH;
	DWORD		dwFLAG;
	EMSNS_ACTION_BY actionBy;		//only valid for WallPost
	

	SSNSMSGREQ ()
		: emSNSREQ  ( EMSNSREQ_NONE )
		, actionBy  ( EMSNS_USER	)
		, dwFLAG	( 0 )
	{
	}
};

struct SSNSMSGFB
{
	EMSNSREQFB	emFB;
	DWORD		dwFLAG;

	SSNSMSGFB ()
		: emFB	 ( EMSNSREQFB_NONE )
		, dwFLAG ( 0 )
	{
	}

	SSNSMSGFB ( const EMSNSREQFB FB, const SSNSMSGREQ& sReq )
		: emFB ( FB )
	{
		dwFLAG = sReq.dwFLAG;
	}
};

//---------------------------------------------------------------------------------------------------//

class CFacebook;
class CTwitter;
class GLGaeaClient;
class GLCharClient;

class GLSNSClient : public GLSNS
{

private:
	typedef std::queue<SSNSMSGREQ> SSNSMSGQUEUE;
	typedef std::queue<SSNSMSGFB>  SSNSFBQUEUE;

private:
	struct CS
	{
		CRITICAL_SECTION CSPCLock;

		CS         () { InitializeCriticalSection(&CSPCLock); }
		~CS        () { DeleteCriticalSection(&CSPCLock);     }
		void Enter () { EnterCriticalSection(&CSPCLock);      }
		void Leave () { LeaveCriticalSection(&CSPCLock);      }
	};

// Note : 로그
private:
	static CTime			g_LogTime;

public:
	static void BEGINLOGTIME	();
	static void ENDLOGTIME		();
	static void TOLOG			( const char *szFormat, ... );

// Note : 쓰레드

private:
	static HANDLE			g_hThread;
	static DWORD			g_dwThreadID;
	static BOOL				g_bThreadProc;
	static BOOL				g_bThreadEnd;
	static DWORD			g_dwThreadSuspend;
	static SSNSMSGQUEUE		g_SNSReqQueue;
	static SSNSFBQUEUE		g_SNSFBQueue;

private:
	static CS				g_SuspendLock;
	static CS				g_SNSReqLock;
	static CS				g_SNSFBLock;

public:
	static BOOL		BEGINTHREAD		 ( GLSNSClient* pSNSClient );
	static void		ENDTHREAD		 ();
	static void		WAITTHREAD		 ();
	static void		SUSPENDTHREAD	 ();
	static void		RESUMETHREAD	 ();
	static BOOL		ISTHREADVALID	 () { return !(!g_dwThreadID); }

public:
	static void		PUSHSNSREQUEST   ( SSNSMSGREQ& sInReq  );
	static void		POPSNSREQUEST    ( SSNSMSGREQ& sOutReq );
	static UINT32	GETREQCOUNT		 ();

	static void		PUSHSNSFB		 ( SSNSMSGFB& sInFB  );
	static void		POPSNSRFB		 ( SSNSMSGFB& sOutFB );
	static UINT32	GETFBCOUNT		 ();

private:
	static UINT __stdcall THREADMSGPROCESS ( LPVOID pData );

 // Note : 클래스 멤버
public:
    GLSNSClient(GLGaeaClient* pGaeaClient);
    ~GLSNSClient(void);

private:
    // bjju.sns 클라이언트를 종료하지 않는 이상 변수의 값이 유효하도록 하기위해
    //          재접속시 글을 등록하는것을 방지하기 위함

	static BOOL g_bFBAuthMsg; // 최초에 페이스북 인증 메세지를 보냈는가 
	static BOOL g_bTWAuthMsg; // 최초에 트위터 인증 메세지를 보냈는가

private:
    GLGaeaClient*  m_pGaeaClient;

private:
	DWORD	   m_dwMsgWait; // 요청중인 메세지 갯수
	BOOL	   m_bReqWait;  // 요청 중

private:
	CFacebook* m_pFaceBook;
	BOOL	   m_bFaceBookAuthed;

private:
	CTwitter*  m_pTwitter;
	BOOL	   m_bTwitterAuthed;

public:
    void InitDeviceObjects   ();
	void DeleteDeviceObjects ();

public:
	BOOL GetAuthInfoReq		();
	BOOL GetAuthInfoMsg		(NET_MSG_GENERIC* nmg);

	BOOL FBWallPost			( std::string& strWall, EMSNS_ACTION_BY by = EMSNS_USER, DWORD dwFlag=0 );
	BOOL FBUploadPhoto		( std::string& strCaption, std::string& strPath );
	BOOL FBGetAuthURL		( std::string& strOutURL );
	BOOL FBGetLogOutURL		( std::string& strLogOutURL );
	BOOL FBGetAccessToken	();
	BOOL FBLogOut			();
	BOOL FBIsAuthed			() { return m_bFaceBookAuthed; }

	BOOL FBAuthedReq		();
	BOOL FBUnAuthedReq		();
	BOOL FBFriendAddReq		( std::tr1::shared_ptr<GLCharClient> pCharClient );

	BOOL FBAuthedMsg		(NET_MSG_GENERIC* nmg);
	BOOL FBUnAuthedMsg		(NET_MSG_GENERIC* nmg);
	BOOL FBFriendAddMsg		(NET_MSG_GENERIC* nmg);

public:
	BOOL TTTweetMsg			( std::string& strTweet, DWORD dwFlag=0 );
	BOOL TTGetAuthURL		( std::string& strOutURL );
	BOOL TTGetAccessToken	( std::string& strPIN    );
	BOOL TTFollowID			( std::string& strID   );
	BOOL TTUnFollowID		( std::string& strID   );
	BOOL TTIsAuthed			() { return m_bTwitterAuthed; }

	BOOL TTAuthedReq		();
	BOOL TTUnAuthedReq		();
	BOOL TTFollowReq		( std::tr1::shared_ptr<GLCharClient> pCharClient );

	BOOL TTAuthedMsg		(NET_MSG_GENERIC* nmg);
	BOOL TTUnAuthedMsg		(NET_MSG_GENERIC* nmg);
	BOOL TTFollowMsg		(NET_MSG_GENERIC* nmg);

public:

public:
	BOOL IsWait ()	{ return (m_dwMsgWait || m_bReqWait); }
	BOOL IsReq  ()  { return m_bReqWait; }

public:
	HRESULT	FrameMove ( float fTime, float fElapsedTime );

};