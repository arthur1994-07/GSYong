
//
//mjeon.sns.twitter
//

#include "stdafx.h"

#include <cassert>

#include "Twitter.h"

#include "../SigmaCore/XML/2.5.3/ticpp.h"
#include "../SigmaCore/Json/json_spirit.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Log/LogMan.h"
#include "md5.h"



//int cntTmp;

CTwitter* CTwitter::m_pInstance;


CTwitter * CTwitter::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CTwitter();
	}
	
	return m_pInstance;
}

CTwitter::CTwitter()
{
	m_headForm = NULL;

	//global_init will be done in RanSNS or PropSheet constructor.
	//curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();
}


CTwitter::~CTwitter()
{
	curl_easy_cleanup(m_curl);
	//curl_global_cleanup();
}


void CTwitter::CleanUp()
{
	delete m_pInstance;
	m_pInstance = NULL;
}


//
// Following method MUST be called before using of Facebook features.
//  this method sets some urls and also specific description for them.
//
void CTwitter::SetupCustomInfo(TWCUSTOMINFOS &custominfo)
{
	m_customInfo = custominfo;
}


//
// Note: result string is UTF-8 format
//
int CTwitter::Request( std::string url, TW_METHOD twMethod, BOOL bSSL, const char *token, const char *tokenSecret, const char *TweetMsg, std::string *result )
{
	BOOL bRet = TRUE;
	VECBUFFER byBuffer;	
	std::string query;
	//std::string utf8;
	std::string msg;

	char *req_url = NULL;
	char *postarg = NULL;
	char *reply	  = NULL;


	ASSERT(url.length() != 0);

	//utf8 = ansi_to_utf8(url);

	//
	//TWM_GETURL = 0, TWM_AUTH = 1, TWM_TWEET = 2, TWM_FOLLOW = 3, TWM_UNFOLLOW = 4
	//
	switch ( twMethod )
	{
		case TWM_GETURL:	// In this case, Request just returns the URL returend by liboauth.
		{
			req_url = oauth_sign_url2( url.c_str(), NULL, OA_HMAC, NULL, m_customInfo.ConsumerKey.c_str()/*TW_CONSUMER_KEY*/,
					m_customInfo.ConsumerSecret.c_str()/*TW_CONSUMER_SECRET*/, token, tokenSecret );
			
			if (req_url)
			{
				*result = req_url;	// C-str to std::string. implicit casting.

				free(req_url);
				req_url = NULL;
			}
			else
			{
				return EMSNS_TW_GETURLFAIL;
			}

			return EMSNS_OK;
		}
		break;

		case TWM_AUTH:
		case TWM_TWEET:
		{
			if ( TweetMsg )
			{
				msg = sc::string::ansi_to_utf8(std::string(TweetMsg));
			}

			const char *p = (m_PIN.length() > 0) ? m_PIN.c_str() : NULL ;
			const char *m = (msg.length() > 0) ? msg.c_str() : NULL;			

			req_url = oauth_sign_url2(url.c_str(), &postarg, OA_HMAC, NULL, m_customInfo.ConsumerKey.c_str()/*TW_CONSUMER_KEY*/,
				m_customInfo.ConsumerSecret.c_str()/*TW_CONSUMER_SECRET*/, token, tokenSecret, p, m);

			if ( p )
				m_PIN.clear();	// the PIN code will not be needed any more. Reset here.


			if (req_url)
			{
				url = req_url;		// C-str to std::string. implicit casting.

				free(req_url);
				req_url = NULL;
			}
			else
			{
				return ( twMethod == TWM_AUTH ? EMSNS_TW_AUTHFAIL : EMSNS_TW_TWEETFAIL );
			}
			
			if (postarg)
			{
				query = postarg;	// C-str to std::string. implicit casting.

				free(postarg);
				postarg = NULL;
			}
 

			/*
			//
			// Refer to the below NOTE: that describes why i use my own cURL code 
			// rather than using following liboauth code.
			//
			reply = oauth_http_post(req_url, postarg, bSSL!=0);

			*result = reply;	// C-str to std::string. implicit casting.

			if (req_url)
			{
				free(req_url);
				req_url = NULL;
			}
			if (reply)
			{
				free(reply);
				reply = NULL;
			}
			*/
		}
		break;


		case TWM_FOLLOW:
		{
			size_t nLenFollowName = m_FollowName.length();

			if ( nLenFollowName <= 0 )
				return EMSNS_TW_EMPTYNAME;

			std::string utf8 = sc::string::ansi_to_utf8(m_FollowName);			
			
			req_url = oauth_twitter_follow_unfollow(
							TW_REST_FRIENDSHIPSCREATE_URL,
							&postarg,
							OA_HMAC,
							NULL,
							m_customInfo.ConsumerKey.c_str()/*TW_CONSUMER_KEY*/,
							m_customInfo.ConsumerSecret.c_str()/*TW_CONSUMER_SECRET*/,
							token,
							tokenSecret,
							(m_bUserID == TRUE),
							utf8.c_str() );

			// Reset
			m_bUserID = FALSE;
			m_FollowName.clear();

			if (req_url)
			{
				url = req_url;		// C-str to std::string. implicit casting.

				free(req_url);
				req_url = NULL;
			}
			else
			{
				return EMSNS_TW_FOLLOWFAIL;
			}

			if (postarg)	
			{
				query = postarg;	// C-str to std::string. implicit casting.

				free(postarg);
				postarg = NULL;
			}
		}
		break;

		case TWM_UNFOLLOW:
		{
			size_t nLenFollowName = m_FollowName.length();

			if ( nLenFollowName <= 0 )
				return EMSNS_TW_EMPTYNAME;

			std::string utf8 = sc::string::ansi_to_utf8(m_FollowName);			

			req_url = oauth_twitter_follow_unfollow(
				TW_REST_FRIENDSHIPSDESTROY_URL,
				&postarg,
				OA_HMAC,
				NULL,
				m_customInfo.ConsumerKey.c_str()/*TW_CONSUMER_KEY*/,
				m_customInfo.ConsumerSecret.c_str()/*TW_CONSUMER_SECRET*/,
				token,
				tokenSecret,
				(m_bUserID == TRUE),
				utf8.c_str() );

			// Reset
			m_bUserID = FALSE;
			m_FollowName.clear();

			if (req_url)
			{
				url = req_url;		// C-str to std::string. implicit casting.

				free(req_url);
				req_url = NULL;
			}
			else
			{
				return EMSNS_TW_UNFOLLOWFAIL;
			}

			if (postarg)	
			{
				query = postarg;	// C-str to std::string. implicit casting.

				free(postarg);
				postarg = NULL;
			}
		}
		break;
            
		default:
			return EMSNS_NOK;
	}
	

	//
	// mjeon.oauth
	//
	// NOTE: !!!
	//
	// oauth_http_post & oauth_http_get will NOT BE USED because they process cURL result as a string.
	// Actually, the curl result is not a string. Refer to the cURL references.
	// That's why I'm using byteVector for the result.
	// instead of them I use own cURL routines made by myself such like Facebook class.
	//	


	//curl_easy_setopt( m_curl, CURLOPT_VERBOSE, TRUE );

	curl_easy_setopt( m_curl, CURLOPT_URL, url.c_str() );

	if (bSSL)
	{
		curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
	}

	//curl_easy_setopt( m_curl, CURLOPT_USERAGENT, "Mozilla 4.0 (compatible; MSIE 6.0; Win32" );



	//query = get_param_string( params, TRUE );

	//
	// ANSI-to-UTF-8 for the Request
	//
	//utf8 = ansi_to_utf8(query);
	//
	

	curl_easy_setopt( m_curl, CURLOPT_POST, TRUE );
	curl_easy_setopt( m_curl, CURLOPT_POSTFIELDS, query.c_str( ) );



	curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, tw_wrapper_write_callback );
	curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, &byBuffer );

	CURLcode ccOK = curl_easy_perform( m_curl );


	if (ccOK == CURLE_OK)
	{
		long statLong;

		// HTTP 응답코드를 얻어온다.
        if(CURLE_OK == curl_easy_getinfo( m_curl , CURLINFO_HTTP_CODE , &statLong))
		{	
			if (statLong >= 400)
			{
				sc::writeLogError(sc::string::format("CTwitter::Request() - cURL response code is an error(%1%).", statLong));
				
				bRet = FALSE;
				goto _tw_request_end;
			}

			if (byBuffer.empty())
			{
				sc::writeLogError(std::string("CTwitter::Request() - response buffer is empty!"));

				bRet = FALSE;
				goto _tw_request_end;
			}
//#if defined(_RELEASED) || defined(_DEBUG)			
			sc::writeLogInfo(sc::string::format("CTwitter::Request() - Response Code: %1%", statLong));
//#endif
		}

//#if defined(_RELEASED) || defined(_DEBUG)		
        double statDouble;        
        char* statString = NULL;

		if (statLong < 400)
		{
			// Content-Type 를 얻어온다.
			if( CURLE_OK == curl_easy_getinfo( m_curl , CURLINFO_CONTENT_TYPE , &statString ) )
			{
				sc::writeLogInfo(sc::string::format("CTwitter::Request() - Content type: %1%", statString));
			}
		}

        // 다운로드한 문서의 크기를 얻어온다.
        if( CURLE_OK == curl_easy_getinfo( m_curl , CURLINFO_SIZE_DOWNLOAD , &statDouble ) )
		{
			sc::writeLogInfo(sc::string::format("CTwitter::Request() - Download size: %1%bytes", statDouble));
        }

        // 
        if( CURLE_OK == curl_easy_getinfo( m_curl , CURLINFO_SPEED_DOWNLOAD , &statDouble ) )
		{
			sc::writeLogInfo(sc::string::format("CTwitter::Request() - Download speed: %1%bytes/sec", statDouble));
        }
//#endif	

	
		//
		// UTF-8-to-ANSI for the result
		//	
		*result = std::string( sc::string::utf8_to_ansi(&byBuffer[0], (int)byBuffer.size()) );

		if (result->length() <= 0)
		{
			sc::writeLogError(std::string("CTwitter::Request() - result is empty"));
			
			//return nUserNum;
			bRet = FALSE;
			goto _tw_request_end;
		}
	}


	//
	//result should be parsed for good-fashioned error handling.
	//
	// especially,
	/* FACEBOOK: duplicate status message */
	/* TWITTER: status is a duplicate */


	//
	// CURLE_SSL_CONNECT_ERROR
	//
	//if (ccOK == CURLE_SSL_CONNECT_ERROR)
_tw_request_end:
	{
		//
		// let's try to reset curl
		//

		curl_easy_cleanup(m_curl);

		m_curl = curl_easy_init();
	}


	//return (ccOK == CURLE_OK ? EMSNS_OK : EMSNS_CURL_NOK);
	return (bRet ? EMSNS_OK : EMSNS_CURL_NOK);
}


/* following methods moved into SigmaCore StringUtil

//
// simple UTF-8 converting functions
//
std::string CTwitter::utf8_to_ansi(char *utf8, int length)		//Overloaded function using char-array
{
	WCHAR unicode[MAX_BUFFER_LENGTH];
	char ansi[MAX_BUFFER_LENGTH];
	int length_unicode = 0;
	int length_ansi = 0;

	memset(unicode, 0, sizeof(unicode));
	memset(ansi, 0, sizeof(ansi));

	length_unicode =::MultiByteToWideChar(CP_UTF8, 0, utf8, length, unicode, sizeof(unicode));
	length_ansi =	::WideCharToMultiByte(CP_ACP, 0, unicode, length_unicode, ansi, sizeof(ansi), NULL, NULL);

	return std::string(ansi);
}


std::string CTwitter::ansi_to_utf8(std::string& ansi)
{
	WCHAR unicode[MAX_BUFFER_LENGTH];
	char utf8[MAX_BUFFER_LENGTH];

	memset(unicode, 0, sizeof(unicode));
	memset(utf8, 0, sizeof(utf8));

	::MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, unicode, sizeof(unicode));
	::WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, sizeof(utf8), NULL, NULL);

	return std::string(utf8);
}


std::string CTwitter::utf8_to_ansi(std::string &utf8)
{
	WCHAR unicode[MAX_BUFFER_LENGTH];
	char ansi[MAX_BUFFER_LENGTH];

	memset(unicode, 0, sizeof(unicode));
	memset(ansi, 0, sizeof(ansi));

	::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, unicode, sizeof(unicode));
	::WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, sizeof(ansi), NULL, NULL);

	return std::string(ansi);
}
*/



size_t tw_wrapper_write_callback( void *ptr, size_t size, size_t nmemb, void *userp )
{
	return CTwitter::write_callback( ptr, size, nmemb, userp );
}



//
// write_callback function could be called several times for a Request!!
// Therefore, result string MUST be appended to userp but not just copy.
//

size_t CTwitter::write_callback( void *ptr, size_t size, size_t nmemb, void *userp )
{ 
	/*
	This function gets called by libcurl as soon as there is data received that needs to be saved.
	The size of the data pointed to by ptr is size multiplied with nmemb,
	it will not be zero terminated.
	*/

	size_t nLength = min( (size * nmemb), MAX_BUFFER_LENGTH );

	VECBUFFER *pVec = (VECBUFFER *) userp;

/*
	char *p = (char *) userp;
	memcpy (p + cntTmp, (char *)ptr, nLength);
	cntTmp += nLength;
*/
	
	char *begin = (char *) ptr;
	char *end = begin + nLength;
	pVec->insert(pVec->end(), begin, end);


	
/* For Debugging
	FILE *fp = fopen("test.txt", "wb");
	fwrite(&(*pVec)[0], 1, pVec->size(), fp);
	fclose(fp);
*/

	return nLength;
}
