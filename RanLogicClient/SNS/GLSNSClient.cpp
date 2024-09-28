// bjju.sns

#include "pch.h"
#include "../../=CryptoPP/Rijndael.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/String/STRINGUTILS.h"
#include "../../SigmaCore/Encrypt/compbyte.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/Common/StlFunctions.h"
#include "../../EngineLib/Common/gltexfile.h"
#include "../../EngineLib/Common/IniLoader.h"

#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../../SNSLib/SNS.h"
#include "../../SNSLib/FaceBook.h"
#include "../../SNSLib/Twitter.h"

#include "../../RanUI/InnerInterface.h"
#include "../../RanUI/ModalCallerID.h"
#include "../../RanUI/OldUI/SNS/FaceBookWindow.h"
#include "../../RanUI/OldUI/SNS/SNSWindow.h"
#include "../../RanUI/OldUI/SNS/SNSFacebookPage.h"
#include "../../RanUI/OldUI/SNS/SNSTwitterPage.h"
#include "../../RanUI/OldUI/Util/ModalWindow.h"

#include "../Char/GLCharClient.h"
#include "../GLGaeaClient.h"
#include "../Stage/DxGlobalStage.h"

#include "../../BugTrap/BugTrap.h"
#include "./GLSNSClient.h"
#include "shlwapi.h"
#include <process.h>

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Log  ---------------------------------------------------------------------------------------------------//

CTime GLSNSClient::g_LogTime;

void GLSNSClient::BEGINLOGTIME ()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return;
    }

#if defined(_RELEASED) || defined(_DEBUG)

	g_LogTime = CTime::GetCurrentTime();
	
#endif
}

void GLSNSClient::ENDLOGTIME ()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return;
    }

#if defined(_RELEASED) || defined(_DEBUG)

	CTimeSpan cGapSpan	= CTime::GetCurrentTime() - g_LogTime;
	int		  nSec		= (int)cGapSpan.GetTotalSeconds();

	sc::writeLogError(
        sc::string::format(
            "  (%d Sec)", nSec));
#endif
}

void GLSNSClient::TOLOG ( const char *szFormat, ... )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return;
    }

#if defined(_RELEASED) || defined(_DEBUG)
	va_list argList;
	char szBuffer[2048] = "";

	va_start(argList, szFormat);
	StringCbVPrintf(szBuffer, 2048, szFormat, argList);
	va_end(argList);

	sc::writeLogError(std::string(szBuffer));
#endif
}

// Msg Thread ---------------------------------------------------------------------------------------------------//

HANDLE	GLSNSClient::g_hThread	       = NULL;
DWORD	GLSNSClient::g_dwThreadID	   = 0;
DWORD	GLSNSClient::g_dwThreadSuspend = 0;
BOOL	GLSNSClient::g_bThreadProc	   = FALSE;
BOOL	GLSNSClient::g_bThreadEnd	   = FALSE;

GLSNSClient::SSNSMSGQUEUE	GLSNSClient::g_SNSReqQueue;
GLSNSClient::SSNSFBQUEUE	GLSNSClient::g_SNSFBQueue;
GLSNSClient::CS				GLSNSClient::g_SuspendLock;
GLSNSClient::CS				GLSNSClient::g_SNSReqLock;
GLSNSClient::CS				GLSNSClient::g_SNSFBLock;

BOOL GLSNSClient::BEGINTHREAD ( GLSNSClient* pSNSClient )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( g_hThread || !pSNSClient )
	{
		return FALSE;
	}

	g_bThreadProc = TRUE;

	g_hThread = (HANDLE) ::_beginthreadex(	NULL,
											0,
											&GLSNSClient::THREADMSGPROCESS,
											pSNSClient,
											CREATE_SUSPENDED,
											(unsigned int*) &g_dwThreadID );
	if ( !g_hThread )
	{
		g_bThreadProc = FALSE;
		return FALSE;
	}

	++g_dwThreadSuspend;

	GLSNSClient::TOLOG ( "GLSNSClient::BEGINTHREAD 시작" );

	return TRUE;
}

void GLSNSClient::ENDTHREAD	()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return;
    }

	if ( g_hThread )
	{
		g_bThreadEnd = TRUE;

		while ( g_dwThreadSuspend )
		{
			RESUMETHREAD ();
		}

		WAITTHREAD ();

		CloseHandle( g_hThread );
		g_hThread = NULL;
	}

	GLSNSClient::TOLOG ( "GLSNSClient::ENDTHREAD 종료" );
}

void GLSNSClient::WAITTHREAD ()
{
	if ( g_hThread && g_bThreadProc )
	{
		DWORD waitResult = WaitForSingleObject( g_hThread, INFINITE );

		if( waitResult == WAIT_FAILED )
		{
			//;
		}
	}
}

void GLSNSClient::SUSPENDTHREAD	()
{
	g_SuspendLock.Enter();

	if ( g_bThreadEnd )
	{
		return;
	}

	if ( !g_hThread )
	{
		return;
	}

	if ( !g_dwThreadSuspend )
	{
		++g_dwThreadSuspend;
		g_SuspendLock.Leave();

		SuspendThread( g_hThread );
		return;
	}

	g_SuspendLock.Leave();
}

void GLSNSClient::RESUMETHREAD ()
{
	g_SuspendLock.Enter();

	if ( !g_hThread )
	{
		return;
	}

	if ( g_dwThreadSuspend )
	{
		--g_dwThreadSuspend;
		ResumeThread( g_hThread );
	}

	g_SuspendLock.Leave();
}

UINT GLSNSClient::THREADMSGPROCESS ( LPVOID pData )
{
    BT_SetTerminate();

    // TODO(bjju) pData를 SNSClient로 바꾼다.
	GLSNSClient& SNSClient = *(reinterpret_cast<GLSNSClient*>(pData));
	CFacebook*   pFaceBook = SNSClient.m_pFaceBook;
	CTwitter*    pTwitter  = SNSClient.m_pTwitter;

	SSNSMSGREQ sReqMsg;

	while ( !g_bThreadEnd )
	{
		UINT32 nReqCount = GETREQCOUNT ();

		// Note : 요청이 없다면 자기 자신을 멈춘다
		if ( !nReqCount )
		{
			SUSPENDTHREAD ();
			continue;
		}

		// Note : 요청을 가져와서 처리한다.
		POPSNSREQUEST ( sReqMsg );

		GLSNSClient::BEGINLOGTIME();

		switch ( sReqMsg.emSNSREQ )
		{
		// 페이스북 담벼락 쓰기
		case EMSNSREQ_FACEBOOK_WALL:
			{
				GLSNSClient::TOLOG ( "	<<EMSNSREQ_FACEBOOK_WALL>>" );

				if ( !pFaceBook || !SNSClient.FBIsAuthed() )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_FACEBOOK_WALL : 일반실패" );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_FB_WALL_FAIL, sReqMsg ) );
					break;
				}

				int nRet = pFaceBook->WallPost ( sReqMsg.strCONTENTS, sReqMsg.actionBy );

				if ( EMSNS_OK != nRet )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_FACEBOOK_WALL : 실패 %d", nRet );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_FB_WALL_FAIL, sReqMsg ) );
					break;
				}

				GLSNSClient::TOLOG ( "	EMSNSREQ_FACEBOOK_WALL : 성공" );
				PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_FB_WALL_OK, sReqMsg ) );	
				break;
			}
			break;

		// 페이스북 앨범에 등록
		case EMSNSREQ_FACEBOOK_PICTURE:
			{
				GLSNSClient::TOLOG ( "	<<EMSNSREQ_FACEBOOK_PICTURE>>" );

				if ( !pFaceBook || !SNSClient.FBIsAuthed() )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_FACEBOOK_PICTURE : 일반실패" );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_FB_PICTURE_FAIL, sReqMsg ) );
					break;
				}

				int nRet1 = pFaceBook->GetAlbumID();

				if ( EMSNS_OK != nRet1 )
				{
					// Create the album here.
					int nRet2 = pFaceBook->CreateAlbum();

					if ( EMSNS_OK != nRet2 )
					{
						GLSNSClient::TOLOG ( "	EMSNSREQ_FACEBOOK_PICTURE : 앨범 생성 실패 : %d", nRet2 );
						PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_FB_PICTURE_FAIL, sReqMsg ) );
						break;
					}
				}

				int nRet3 = pFaceBook->UploadPhoto( sReqMsg.strCONTENTS, sReqMsg.strSCREENSHOTPATH );

				if ( EMSNS_OK != nRet3 )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_FACEBOOK_PICTURE : 실패 : %d", nRet3 );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_FB_PICTURE_FAIL, sReqMsg ) );
					break;
				}

				GLSNSClient::TOLOG ( "	EMSNSREQ_FACEBOOK_PICTURE : 성공" );
				PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_FB_PICTURE_OK, sReqMsg ) );
			}
			break;

		// 트위터에 트윗하기
		case EMSNSREQ_TWITTER_TWEET:
			{
				GLSNSClient::TOLOG ( "	<<EMSNSREQ_TWITTER_TWEET>>" );

				if ( !pTwitter || !SNSClient.TTIsAuthed() )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_TWEET : 일반실패" );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_TWEET_FAIL, sReqMsg ) );
					break;
				}

				if ( sReqMsg.strCONTENTS.size() == 0 )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_TWEET : 컨텐츠가 없음" );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_TWEET_FAIL, sReqMsg ) );
					break;
				}

				int nRet = pTwitter->TweetMsg( sReqMsg.strCONTENTS );

				if ( EMSNS_OK != nRet )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_TWEET : 실패 : %d", nRet );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_TWEET_FAIL , sReqMsg) );
					break;
				}

				GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_TWEET : 성공" );
				PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_TWEET_OK, sReqMsg ) );
			}
			break;

		// 트위터 팔로우하기
		case EMSNSREQ_TWITTER_FOLLOW:
			{
				GLSNSClient::TOLOG ( "	<<EMSNSREQ_TWITTER_FOLLOW>>" );

				if ( !pTwitter || !SNSClient.TTIsAuthed() )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_FOLLOW : 일반실패" );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_FOLLOW_FAIL, sReqMsg ) );
					break;
				}

				if ( sReqMsg.strCONTENTS.size() == 0 )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_FOLLOW : 컨텐츠가 없음" );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_FOLLOW_FAIL, sReqMsg ) );
					break;
				}

				int nRet = pTwitter->Follow( FALSE, sReqMsg.strCONTENTS );

				if ( EMSNS_OK != nRet )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_FOLLOW : 실패 : %d", nRet );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_FOLLOW_FAIL, sReqMsg ) );
					break;
				}

				GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_FOLLOW : 성공" );
				PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_FOLLOW_OK, sReqMsg ) );
			}
			break;

		// 트위터 언팔로우하기
		case EMSNSREQ_TWITTER_UNFOLLOW:
			{
				GLSNSClient::TOLOG ( "	<<EMSNSREQ_TWITTER_UNFOLLOW>>" );

				if ( !pTwitter || !SNSClient.TTIsAuthed() )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_UNFOLLOW : 일반실패" );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_UNFOLLOW_FAIL, sReqMsg ) );
					break;
				}

				if ( sReqMsg.strCONTENTS.size() == 0 )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_UNFOLLOW : 컨텐츠가 없음" );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_UNFOLLOW_FAIL, sReqMsg ) );
					break;
				}

				int nRet = pTwitter->UnFollow( FALSE, sReqMsg.strCONTENTS );

				if ( EMSNS_OK != nRet )
				{
					GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_UNFOLLOW : 실패 : %d", nRet );
					PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_UNFOLLOW_FAIL, sReqMsg ) );
					break;
				}

				GLSNSClient::TOLOG ( "	EMSNSREQ_TWITTER_UNFOLLOW : 성공" );
				PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_TT_UNFOLLOW_OK, sReqMsg ) );
			}
			break;

		default:
			{
				GLSNSClient::TOLOG ( "	default : 일반실패" );
				PUSHSNSFB ( SSNSMSGFB( EMSNSREQFB_NONE, sReqMsg ) );
			}
			break;
		};

		GLSNSClient::ENDLOGTIME();
	}

	g_bThreadProc = FALSE;
	g_bThreadEnd  = TRUE;

	return 0;
}

void GLSNSClient::PUSHSNSREQUEST ( SSNSMSGREQ& sInReq )
{
	g_SNSReqLock.Enter();
	
	g_SNSReqQueue.push( sInReq );

	GLSNSClient::TOLOG ( "PUSHSNSREQUEST() emSNSREQ:%d, strCONTENTS:%s actionBy:%d", sInReq.emSNSREQ, sInReq.strCONTENTS.c_str(), sInReq.actionBy );

	g_SNSReqLock.Leave();

	// Note : 만약 잠들어 있다면 깨운다.
	RESUMETHREAD ();
}

void GLSNSClient::POPSNSREQUEST ( SSNSMSGREQ& sOutReq )
{
	g_SNSReqLock.Enter();

	if ( g_SNSReqQueue.size() )
	{
		sOutReq = g_SNSReqQueue.front();
		g_SNSReqQueue.pop();

		GLSNSClient::TOLOG ( "POPSNSREQUEST() emSNSREQ:%d, strCONTENTS:%s actionBy:%d", sOutReq.emSNSREQ, sOutReq.strCONTENTS.c_str(), sOutReq.actionBy );
	}

	g_SNSReqLock.Leave();
}

UINT32 GLSNSClient::GETREQCOUNT ()
{
	g_SNSReqLock.Enter();

	UINT32 nCount = (UINT32) g_SNSReqQueue.size();

	g_SNSReqLock.Leave();

	return nCount;
}

void GLSNSClient::PUSHSNSFB	( SSNSMSGFB& sInFB )
{
	g_SNSFBLock.Enter();

	g_SNSFBQueue.push( sInFB );

	g_SNSFBLock.Leave();
}

void GLSNSClient::POPSNSRFB	( SSNSMSGFB& sOutFB )
{
	g_SNSFBLock.Enter();

	if ( g_SNSFBQueue.size() )
	{
		sOutFB = g_SNSFBQueue.front();
		g_SNSFBQueue.pop();
	}

	g_SNSFBLock.Leave();
}

UINT32 GLSNSClient::GETFBCOUNT ()
{
	g_SNSFBLock.Enter();

	UINT32 nCount = (UINT32) g_SNSFBQueue.size();

	g_SNSFBLock.Leave();

	return nCount;
}

// class member  ---------------------------------------------------------------------------------------------------//

BOOL GLSNSClient::g_bFBAuthMsg = FALSE; 
BOOL GLSNSClient::g_bTWAuthMsg = FALSE; 

GLSNSClient::GLSNSClient(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient     ( pGaeaClient )
    , m_dwMsgWait		( 0		 )
	, m_bReqWait		( FALSE  )
	, m_pFaceBook		( NULL   )
	, m_bFaceBookAuthed ( FALSE  )
	, m_pTwitter		( NULL   )
	, m_bTwitterAuthed	( FALSE  )
{
	SNS::initSNS();

	m_pFaceBook = CFacebook::GetInstance();
	m_pTwitter  = CTwitter::GetInstance();
}

GLSNSClient::~GLSNSClient()
{
	ENDTHREAD ();

	if ( m_pFaceBook )
	{
		m_pFaceBook->LogOut();

		m_pFaceBook->CleanUp();
		m_pFaceBook = NULL;
	}

	if ( m_pTwitter )
	{
		m_pTwitter->CleanUp();
		m_pTwitter = NULL;
	}

	SNS::cleanSNS();
}

void GLSNSClient::InitDeviceObjects ()
{
    // FACEBOOK Setting
    if ( m_pFaceBook )
    {
        FBCUSTOMINFOS custominfo;

        URLINFO IMAGELOGO;
        URLINFO HREF;
        URLINFO ACTIONLINK;
        URLINFO LOGINCANCEL;
        URLINFO LOGOUTNEXT;
        URLINFO IMAGEWELCOME;
        URLINFO IMAGELEVELUP;
        URLINFO IMAGEENCHANTED;

        IMAGELOGO.url	     = GLCONST_SNS::strSNS_IMAGE_URL + GLCONST_SNS::strSNS_IMAGE_LOGO;
        IMAGELOGO.name	     = ID2GAMEINTEXT("EMSNS_FB_NAME_LOGO");
        IMAGELOGO.desc	     = ID2GAMEINTEXT("EMSNS_DESC_LOGO");

        IMAGEWELCOME.url     = GLCONST_SNS::strSNS_IMAGE_URL + GLCONST_SNS::strSNS_IMAGE_WELCOME;
        IMAGEWELCOME.name    = ID2GAMEINTEXT("EMSNS_FB_NAME_WELCOME");
        IMAGEWELCOME.desc    = ID2GAMEINTEXT("EMSNS_DESC_WELCOME");

        IMAGELEVELUP.url     = GLCONST_SNS::strSNS_IMAGE_URL + GLCONST_SNS::strSNS_IMAGE_LEVELUP;
        IMAGELEVELUP.name    = ID2GAMEINTEXT("EMSNS_FB_NAME_LEVELUP");
        IMAGELEVELUP.desc    = ID2GAMEINTEXT("EMSNS_DESC_LEVELUP");

        IMAGEENCHANTED.url   = GLCONST_SNS::strSNS_IMAGE_URL + GLCONST_SNS::strSNS_IMAGE_ENCHANTED;
        IMAGEENCHANTED.name  = ID2GAMEINTEXT("EMSNS_FB_NAME_ENCHANTED");
        IMAGEENCHANTED.desc  = ID2GAMEINTEXT("EMSNS_DESC_ENCHANTED");

        ACTIONLINK.url	     = GLCONST_SNS::strSNS_ACTION_LINK;
        ACTIONLINK.name	     = ID2GAMEINTEXT("EMSNS_FB_ACTION_NAME");

        HREF.url		 = GLCONST_SNS::strSNS_FB_HREF;
        LOGINCANCEL.url  = GLCONST_SNS::strSNS_LOGIN_CANCEL_URL;
        LOGOUTNEXT.url   = GLCONST_SNS::strSNS_LOGOUT_NEXT_URL;

        custominfo.urlHREF			 = HREF;
        custominfo.urlACTIONLINK	 = ACTIONLINK;
        custominfo.urlLOGINCANCEL    = LOGINCANCEL;
        custominfo.urlLOGOUTNEXT     = LOGOUTNEXT;
        custominfo.urlIMAGELOGO	     = IMAGELOGO;
        custominfo.urlIMAGEWELCOME   = IMAGEWELCOME;
        custominfo.urlIMAGELEVELUP   = IMAGELEVELUP;
        custominfo.urlIMAGEENCHANTED = IMAGEENCHANTED;

		custominfo.AppID	 = GLCONST_SNS::strSNS_FB_APP_ID;
		custominfo.ApiKey	 = GLCONST_SNS::strSNS_FB_API_KEY;
		custominfo.AppSecret = GLCONST_SNS::strSNS_FB_APP_SECRET;

        m_pFaceBook->SetupCustomInfo(custominfo);
    }

	// TWITTER Setting
	if ( m_pTwitter )
	{
		TWCUSTOMINFOS custominfo;

		custominfo.ConsumerKey	  = GLCONST_SNS::strSNS_TW_CONSUMER_KEY;
		custominfo.ConsumerSecret = GLCONST_SNS::strSNS_TW_CONSUMER_SECRET;
		m_pTwitter->SetupCustomInfo(custominfo);
	}
}

void GLSNSClient::DeleteDeviceObjects ()
{
	m_bReqWait = FALSE;

	GLSNSClient::TOLOG ( "GLSNSClient::DeleteDeviceObjects : m_dwMsgWait = %d, m_bReqWait = %d ", m_dwMsgWait, m_bReqWait );

	//while ( IsWait() ) ;

	m_bFaceBookAuthed = FALSE;
	m_bTwitterAuthed  = FALSE;
}

HRESULT	GLSNSClient::FrameMove ( float fTime, float fElapsedTime )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return S_OK;
    }

	// Note : 요청에대한 응답 처리
	if ( GETFBCOUNT () )
	{
		SSNSMSGFB sFB;
		POPSNSRFB( sFB );

		switch ( sFB.emFB )
		{
		case EMSNSREQFB_FB_WALL_OK:
			{

				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_FB_WALL_OK"), MODAL_INFOMATION, OK );
					//m_pInterface->SNSFBPageClearEditWall ();
					//m_pInterface->SNSFBPageSetEnableWallButton( TRUE );
				}
			}
			break;

		case EMSNSREQFB_FB_WALL_FAIL:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR"), MODAL_ERROR, OK );
                    //m_pInterface->SNSFBPageSetEnableWallButton( TRUE );
				}
			}
			break;

		case EMSNSREQFB_FB_PICTURE_OK:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_FB_PICTURE_OK"), MODAL_INFOMATION, OK );
                    //m_pInterface->SNSFBPageClearEditPicture ();
					//m_pInterface->SNSFBPageSetEnablePicture ( TRUE );
				}
			}
			break;

		case EMSNSREQFB_FB_PICTURE_FAIL:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR"), MODAL_ERROR, OK );
                    //m_pInterface->SNSFBPageSetEnablePicture ( TRUE );
				}
			}
			break;

		case EMSNSREQFB_TT_TWEET_OK:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_TT_TWEET_OK"), MODAL_INFOMATION, OK );
					//m_pInterface->SNSTWPageClearEditTweet ();
					//m_pInterface->SNSTWPageSetEnableTweet ( TRUE );
				}
			}
			break;

		case EMSNSREQFB_TT_TWEET_FAIL:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR"), MODAL_ERROR, OK );
					//m_pInterface->SNSTWPageSetEnableTweet ( TRUE );
				}
			}
			break;

		case EMSNSREQFB_TT_FOLLOW_OK:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_TT_FOLLOW_OK"), MODAL_INFOMATION, OK );
                    //m_pInterface->SNSTWPageSetEnableFollower( TRUE );
				}
			}
			break;

		case EMSNSREQFB_TT_FOLLOW_FAIL:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR"), MODAL_ERROR, OK );
					//m_pInterface->SNSTWPageSetEnableFollower( TRUE );
				}
			}
			break;

		case EMSNSREQFB_TT_UNFOLLOW_OK:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_TT_UNFOLLOW_OK"), MODAL_INFOMATION, OK );
                    //m_pInterface->SNSTWPageSetEnableFollower( TRUE );
				}
			}
			break;

		case EMSNSREQFB_TT_UNFOLLOW_FAIL:
			{
				if ( sFB.dwFLAG&EMSNSREQTYPE_AUTOREQ )
				{
					//
				}
				else
				{
					//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR"), MODAL_ERROR, OK );
                    //m_pInterface->SNSTWPageSetEnableFollower( TRUE );
				}
			}
			break;
		};

		if ( m_dwMsgWait )
		{
			--m_dwMsgWait;
		}
	}

	return S_OK;
}

// ------------------------------------------------------------------------------------------------------------//

BOOL GLSNSClient::GetAuthInfoReq ()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( IsWait() || !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::GetAuthInfoReq ( IsWait() || !m_pFaceBook ) is TRUE" );

		return FALSE;
	}

	GLMSG::SNET_SNS_GETINFO Netmsg;
	m_pGaeaClient->NETSENDTOAGENT( &Netmsg );

	m_bReqWait = TRUE;

	return TRUE;
}


BOOL GLSNSClient::GetAuthInfoMsg ( NET_MSG_GENERIC* nmg )
{
	m_bReqWait = FALSE;

	if ( !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::GetAuthInfoMsg (!m_pFaceBook) " );

		return FALSE;
	}

	GLMSG::SNET_SNS_GETINFO_FB *msg = (GLMSG::SNET_SNS_GETINFO_FB *)nmg;

	switch ( msg->result )
	{
	case EMSNS_RES_OK:
		{
			// 페이스북
			if (m_pFaceBook && strlen(msg->FBSKey) > 0 && strlen(msg->FBUID) > 0)
			{
				m_bFaceBookAuthed = TRUE;

                m_pFaceBook->SetSKey		( std::string(msg->FBSKey) );
				m_pFaceBook->SetUID			( std::string(msg->FBUID ) );

				if ( m_pFaceBook->GetAccessToken () != EMSNS_OK )
				{
					m_bFaceBookAuthed = FALSE;

					m_pFaceBook->Reset();

					GLSNSClient::TOLOG ( "ERROR : GLSNSClient::GetAuthInfoMsg (m_pFaceBook->GetAccessToken () != EMSNS_OK) " );

					return FALSE;
				}
			}
			else
			{
				m_bFaceBookAuthed = FALSE;
			}

			// 트위터
			if ( m_pTwitter && strlen(msg->TWAToken) > 0 && strlen(msg->TWATokenS) && strlen(msg->TWUID) > 0)
			{
				m_bTwitterAuthed = TRUE;

				m_pTwitter->SetAToken	( std::string(msg->TWAToken) );
				m_pTwitter->SetATokenS	( std::string(msg->TWATokenS) );
				m_pTwitter->SetUID		( std::string(msg->TWUID) );
			}
			else
			{
				m_bTwitterAuthed = FALSE;
			}
		}
		break;

	default:
		{
			m_bFaceBookAuthed = FALSE;
			m_bTwitterAuthed  = FALSE;
		}
		return FALSE;
	};

	return TRUE;
}

// Facebook //------------------------------------------------------------------------------------------------------------//

BOOL GLSNSClient::FBGetAuthURL ( std::string& strOutURL )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBGetAuthURL (!m_pFaceBook) " );

		return FALSE;
	}
	//
	// 1. Get AuthToken
	//

	int nRet = m_pFaceBook->GetAuthToken ();

	if ( nRet != EMSNS_OK )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBGetAuthURL ( m_pFaceBook->GetAuthToken() Returned %d ) ", nRet );
        //m_pInterface->SNSWindowDoModalError( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_FB_AUTHTOKEN") );

		return FALSE;
	}

	//
	// 2.Authenticate & Authorize
	//
	m_pFaceBook->GetAuthURL (strOutURL);

	return TRUE;
}

BOOL GLSNSClient::FBGetAccessToken ()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBGetAccessToken (!m_pFaceBook) " );

		return FALSE;
	}

	//
	// 3.Get SessionKey
	//
	int nRet = m_pFaceBook->GetSession();

	if( nRet != EMSNS_OK )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBGetAccessToken ( m_pFaceBook->GetSession() Returned %d  ) ", nRet );

		return FALSE;
	}

	//
	// 4.Get AccessToken
	//
	nRet = m_pFaceBook->GetAccessToken();

	if( nRet != EMSNS_OK )
	{
		//FBLogOut ();

		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBGetAccessToken ( m_pFaceBook->GetAccessToken() Returned %d ) ", nRet );

		return FALSE;
	}

	//FBLogOut ();

	return TRUE;
}

BOOL GLSNSClient::FBLogOut()
{	
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !m_pFaceBook )
	{
		return FALSE;
	}

	//m_pFaceBook->LogOut();

	return TRUE;
}


BOOL GLSNSClient::FBGetLogOutURL ( std::string& strLogOutURL )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBGetLogOutURL (!m_pFaceBook) " );

		return FALSE;
	}

	m_pFaceBook->GetLogOutURL( strLogOutURL );

	return TRUE;
}

BOOL GLSNSClient::FBWallPost ( std::string& strWall, EMSNS_ACTION_BY by, DWORD dwFlag )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !FBIsAuthed() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBWallPost ( !FBIsAuthed() ) " );

		return FALSE;
	}

	SSNSMSGREQ sReq;

	if ( by == EMSNS_USER )
	{
		if ( !strWall.size() )
		{
			GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBWallPost ( strWall.size() == 0 ) " );

			return FALSE;
		}
		else
			sReq.strCONTENTS = strWall;
	}

	sReq.strCONTENTS = strWall;	
	sReq.emSNSREQ	= EMSNSREQ_FACEBOOK_WALL;
	sReq.actionBy	= by;
	sReq.dwFLAG		= dwFlag;
	
	PUSHSNSREQUEST( sReq );

	++m_dwMsgWait;

	return TRUE;
}

BOOL GLSNSClient::FBUploadPhoto ( std::string& strCaption, std::string& strPath )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !FBIsAuthed() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBUploadPhoto ( !FBIsAuthed() ) " );

		return FALSE;
	}

	if ( !strPath.size() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBUploadPhoto ( strPath.size() == 0 ) " );

		return FALSE;
	}

	SSNSMSGREQ sReq;
	sReq.emSNSREQ			= EMSNSREQ_FACEBOOK_PICTURE;
	sReq.strCONTENTS		= strCaption;
	sReq.strSCREENSHOTPATH	= strPath;
	PUSHSNSREQUEST( sReq );

	++m_dwMsgWait;
	
	//m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_FB_REQWAIT"), MODAL_INFOMATION, OK );

	return TRUE;
}

BOOL GLSNSClient::FBAuthedReq ()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( IsWait() || !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBAuthedReq ( IsWait() || !m_pFaceBook ) " );

		return FALSE;
	}

	GLMSG::SNET_SNS_FB_AUTHED Netmsg;
	StringCchCopy( Netmsg.SKey,	SNS_MAX_SKEY_LENGTH, m_pFaceBook->GetSKey().c_str() );
	StringCchCopy( Netmsg.UID,	SNS_MAX_UID_LENGTH,   m_pFaceBook->GetUID().c_str() );
	m_pGaeaClient->NETSENDTOAGENT( &Netmsg );

	m_bReqWait = TRUE;

	return TRUE;
}

BOOL GLSNSClient::FBUnAuthedReq	()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( IsWait() || !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBUnAuthedReq ( IsWait() || !m_pFaceBook ) " );

		return FALSE;
	}

	GLMSG::SNET_SNS_FB_UNAUTHED Netmsg;
	m_pGaeaClient->NETSENDTOAGENT( &Netmsg );

	m_bReqWait = TRUE;

	return TRUE;
}

BOOL GLSNSClient::FBFriendAddReq(std::tr1::shared_ptr<GLCharClient> pCharClient)
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBFriendAddReq ( !m_pFaceBook ) " );

		return FALSE;
	}

	if ( !pCharClient )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBFriendAddReq ( !pCharClient ) " );
	
		return FALSE;
	}

	GLMSG::SNET_SNS_FB_GETUID Netmsg;
	Netmsg.dwTargetGaeaID = pCharClient->GetGaeaID();
	m_pGaeaClient->NETSENDTOAGENT( &Netmsg );

	return TRUE;
}

BOOL GLSNSClient::FBAuthedMsg ( NET_MSG_GENERIC* nmg )
{
	m_bReqWait = FALSE;

	if ( !m_pFaceBook )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBAuthedMsg ( !m_pFaceBook ) " );

		return FALSE;
	}	

	GLMSG::SNET_SNS_FB_AUTHED_FB *msg = (GLMSG::SNET_SNS_FB_AUTHED_FB *)nmg;

	switch ( msg->result )
	{
	case EMSNS_RES_OK:
		{
			//FBLogOut();
			//BOOL bBypassed = SNS::BypassCookie(std::string("http://www.facebook.com"));
			//BOOL bDeleted = SNS::DeleteCookie();

			m_bFaceBookAuthed = TRUE;

			//
			//mjeon.sns
			//

			//const char *pChaName = GLGaeaClient::GetInstance().GetCharacter()->GetName();
			//string strChaName(pChaName);

            RANPARAM::SNSFACEBOOK_SET& sOptFaceBook = RANPARAM::sSNSFaceBook;			

			if ( sOptFaceBook.bSTART && !g_bFBAuthMsg )
			{
				FBWallPost ( std::string(""), EMSNS_START_SNS, EMSNSREQTYPE_AUTOREQ );

				g_bFBAuthMsg = TRUE;
			}
		}
		break;

	default:
		{
			m_bFaceBookAuthed = FALSE;
		}
		return FALSE;
	};

	return TRUE;
}

BOOL GLSNSClient::FBUnAuthedMsg ( NET_MSG_GENERIC* nmg )
{
	m_bReqWait = FALSE;

	GLMSG::SNET_SNS_FB_UNAUTHED_FB *msg = (GLMSG::SNET_SNS_FB_UNAUTHED_FB *)nmg;

	switch ( msg->result )
	{
	case EMSNS_RES_OK:
		{
			if ( m_pFaceBook )
			{
				//m_pInterface->SNSWebFBWindowSetPageType ( CFaceBookWindow::PAGETYPE_UNAUTH );

				//if ( !m_pInterface->UiIsVisibleGroup ( FACEBOOK_WINDOW ) )
				//{
				//	m_pInterface->UiShowGroupFocus ( FACEBOOK_WINDOW );
				//}

				m_pFaceBook->Reset();

				//BOOL bBypassed = SNS::BypassCookie(std::string("http://www.facebook.com"));
				//BOOL bDeleted = SNS::DeleteCookie();
			}

			m_bFaceBookAuthed = FALSE;
		}
		break;

	default:
		{
			return FALSE;
		}
	};

	return TRUE;
}

BOOL GLSNSClient::FBFriendAddMsg ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SNS_FB_GETUID_FB *msg = (GLMSG::SNET_SNS_FB_GETUID_FB *)nmg;

	switch ( msg->result )
	{
	case EMSNS_RES_OK:
		{
			if ( !m_pFaceBook )
			{
				GLSNSClient::TOLOG ( "ERROR : GLSNSClient::FBFriendAddMsg ( !m_pFaceBook ) " );

				return FALSE;
			}

			std::string strURL;
			m_pFaceBook->GetAddFriendURL ( std::string(msg->TargetUID), strURL );

			//m_pInterface->SNSWebFBWindowSetAddFriendURL ( strURL );
			//m_pInterface->SNSWebFBWindowSetPageType	    ( CFaceBookWindow::PAGETYPE_FRIEND_ADD );

			//if ( !m_pInterface->UiIsVisibleGroup ( FACEBOOK_WINDOW ) )
			//{
			//	m_pInterface->UiShowGroupFocus ( FACEBOOK_WINDOW );
			//}
		}
		break;

	default:
		{
			return FALSE;
		}
	}

	return TRUE;
}

// Twitter //------------------------------------------------------------------------------------------------------------//

BOOL GLSNSClient::TTGetAuthURL ( std::string& strOutURL )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !m_pTwitter )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTGetAuthURL ( !m_pTwitter ) " );

		return FALSE;
	}

	//
	// 1. Acquiring RequestToken
	//
	int nRet = m_pTwitter->GetRequestToken();

	if ( nRet != EMSNS_OK )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTGetAuthURL ( m_pTwitter->GetRequestToken() Returned %d ) ", nRet );

		//m_pInterface->SNSWindowDoModalError( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_TW_REQTOKEN") );
		return FALSE;
	}

	//
	// 2. Compose a URL for Authorization
	//
	nRet = m_pTwitter->MakeAuthURL();

	if ( nRet != EMSNS_OK )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTGetAuthURL ( m_pTwitter->MakeAuthURL() Returned %d ) ", nRet );

		//m_pInterface->SNSWindowDoModalError( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_TW_AUTHURL") );
		return FALSE;
	}

	//
	// 3. Authorize (Input PIN code) on web-browser
	//	
	strOutURL = m_pTwitter->GetAuthURL();

	return TRUE;
}

BOOL GLSNSClient::TTGetAccessToken	( std::string& strPIN )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !m_pTwitter )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTGetAccessToken ( !m_pTwitter ) " );

		return FALSE;
	}

	//
	// 4.Exchanging a RequestToken for an AccessToken
	//	

	int nRet = m_pTwitter->GetAccessToken( strPIN );

	if ( EMSNS_OK != nRet )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTGetAccessToken ( m_pTwitter->GetAccessToken() Returned %d ) ", nRet );

		return FALSE;
	}

	return TRUE;
}

BOOL GLSNSClient::TTTweetMsg ( std::string& strTweet, DWORD dwFlag )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !TTIsAuthed() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTTweetMsg ( !TTIsAuthed() ) " );

		return FALSE;
	}

	if ( !strTweet.size() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTTweetMsg ( strTweet.size() == 0 ) " );

		return FALSE;
	}

	SSNSMSGREQ sReq;
	sReq.emSNSREQ	 = EMSNSREQ_TWITTER_TWEET;
	sReq.strCONTENTS = strTweet;
	sReq.dwFLAG		 = dwFlag;
	PUSHSNSREQUEST( sReq );

	++m_dwMsgWait;

	return TRUE;
}

BOOL GLSNSClient::TTFollowID ( std::string& strID )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !TTIsAuthed() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTFollowID ( !TTIsAuthed() ) " );

		return FALSE;
	}

	if ( !strID.size() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTFollowID ( strID.size() == 0 ) " );

		return FALSE;
	}

	SSNSMSGREQ sReq;
	sReq.emSNSREQ	 = EMSNSREQ_TWITTER_FOLLOW;
	sReq.strCONTENTS = strID;
	PUSHSNSREQUEST( sReq );

	++m_dwMsgWait;

	return TRUE;
}

BOOL GLSNSClient::TTUnFollowID	( std::string& strID )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !TTIsAuthed() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTUnFollowID ( !TTIsAuthed() ) " );

		return FALSE;
	}

	if ( !strID.size() )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTUnFollowID ( strID.size() == 0 ) " );

		return FALSE;
	}

	SSNSMSGREQ sReq;
	sReq.emSNSREQ	 = EMSNSREQ_TWITTER_UNFOLLOW;
	sReq.strCONTENTS = strID;
	PUSHSNSREQUEST( sReq );

	++m_dwMsgWait;

	return TRUE;
}

BOOL GLSNSClient::TTAuthedReq ()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( IsWait() || !m_pTwitter )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTAuthedReq ( IsWait() || !m_pTwitter ) " );

		return FALSE;
	}

	GLMSG::SNET_SNS_TW_AUTHED Netmsg;
	StringCchCopy( Netmsg.AToken,	SNS_MAX_TOKEN_LENGTH, m_pTwitter->GetAToken().c_str() );
	StringCchCopy( Netmsg.ATokenS,	SNS_MAX_TOKEN_LENGTH, m_pTwitter->GetATokenS().c_str() );
	StringCchCopy( Netmsg.UID,		SNS_MAX_UID_LENGTH,   m_pTwitter->GetUID().c_str() );
	m_pGaeaClient->NETSENDTOAGENT( &Netmsg );

	m_bReqWait = TRUE;

	return TRUE;
}

BOOL GLSNSClient::TTUnAuthedReq ()
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( IsWait() || !m_pTwitter )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTUnAuthedReq ( IsWait() || !m_pTwitter ) " );

		return FALSE;
	}

	GLMSG::SNET_SNS_TW_UNAUTHED Netmsg;
	m_pGaeaClient->NETSENDTOAGENT( &Netmsg );

	m_bReqWait = TRUE;

	return TRUE;
}

BOOL GLSNSClient::TTFollowReq ( std::tr1::shared_ptr<GLCharClient> pCharClient )
{
    if ( !GLUseFeatures::GetInstance().IsUsingSNS() )
    {
        return FALSE;
    }

	if ( !m_pTwitter )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTUnAuthedReq ( !m_pTwitter ) " );

		return FALSE;
	}

	if ( !pCharClient )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTUnAuthedReq ( !pCharClient ) " );

		return FALSE;
	}

	GLMSG::SNET_SNS_TW_GETUID Netmsg;
	Netmsg.dwTargetGaeaID = pCharClient->GetGaeaID();
	m_pGaeaClient->NETSENDTOAGENT( &Netmsg );

	return TRUE;

}

BOOL GLSNSClient::TTAuthedMsg ( NET_MSG_GENERIC* nmg )
{
	m_bReqWait = FALSE;

	if ( !m_pTwitter )
	{
		GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTAuthedMsg ( !m_pTwitter ) " );

		return FALSE;
	}

	GLMSG::SNET_SNS_TW_AUTHED_FB *msg = (GLMSG::SNET_SNS_TW_AUTHED_FB *)nmg;

	switch ( msg->result )
	{
	case EMSNS_RES_OK:
		{
			m_bTwitterAuthed = TRUE;


            RANPARAM::SNSTWITTER_SET& sOptTwitter = RANPARAM::sSNSTwitter;	

			if ( sOptTwitter.bSTART && !g_bTWAuthMsg )
			{
				TTTweetMsg( std::string(ID2GAMEINTEXT("EMSNS_DESC_WELCOME")), EMSNSREQTYPE_AUTOREQ );

				g_bTWAuthMsg = TRUE;
			}
		}
		break;

	default:
		{
			m_bTwitterAuthed = FALSE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL GLSNSClient::TTUnAuthedMsg	( NET_MSG_GENERIC* nmg )
{
	m_bReqWait = FALSE;

	GLMSG::SNET_SNS_TW_UNAUTHED_FB *msg = (GLMSG::SNET_SNS_TW_UNAUTHED_FB *)nmg;

	switch ( msg->result )
	{
	case EMSNS_RES_OK:
		{
			m_bTwitterAuthed = FALSE;
		}
		break;

	default:
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL GLSNSClient::TTFollowMsg ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SNS_TW_GETUID_FB *msg = (GLMSG::SNET_SNS_TW_GETUID_FB *)nmg;

	switch ( msg->result )
	{
	case EMSNS_RES_OK:
		{
			if ( m_pTwitter )
			{
				int nRet = m_pTwitter->Follow( TRUE, std::string(msg->TargetUID) );

				if ( EMSNS_OK != nRet )
				{
					GLSNSClient::TOLOG ( "ERROR : GLSNSClient::TTFollowMsg ( !m_pTwitter->Follow() Returned %d  ) ", nRet );

					//m_pInterface->SNSWindowDoModalError( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_TW_FOLLOW") );

					return FALSE;
				}
			}
		}
		break;

	default:
		{
			return FALSE;
		}
	};

	return TRUE;
}
