
//
//mjeon.sns.facebook
//

#include "stdafx.h"

#include "FaceBook.h"

#include "../SigmaCore/XML/2.5.3/ticpp.h"
#include "../SigmaCore/Json/json_spirit.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Log/LogMan.h"
#include "md5.h"



//int cntTmp;

CFacebook* CFacebook::m_pInstance;


CFacebook * CFacebook::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CFacebook();
	}
	
	return m_pInstance;
}

CFacebook::CFacebook()
{
	m_bSession = FALSE;
	m_nCallID = 0;
	m_headForm = NULL;

	//global_init will be done in RanSNS or PropSheet constructor.
	//curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();	
}


CFacebook::~CFacebook()
{
	curl_easy_cleanup(m_curl);
	//curl_global_cleanup();
}


void CFacebook::CleanUp()
{
	delete m_pInstance;
	m_pInstance = NULL;	
}


//
// Following method MUST be called before using of Facebook features.
//  this method sets some urls and also specific description for them.
//
void CFacebook::SetupCustomInfo(FBCUSTOMINFOS &custominfo)
{
	m_customInfo = custominfo;
}

void CFacebook::CreatePhotoForm(std::string &caption, std::string &photoPath)
{
	//struct curl_httppost *headptr=NULL;
	struct curl_httppost *lastptr=NULL;
	
	CURLFORMcode fc = CURL_FORMADD_OK;

	
	std::string utf8_caption = sc::string::ansi_to_utf8(caption);
	//string utf8_path = ansi_to_utf8(photoPath);


	fc = curl_formadd(&m_headForm,
		&lastptr,
		CURLFORM_COPYNAME, "method",
		CURLFORM_COPYCONTENTS, "photos.upload",
		CURLFORM_END);


	fc = curl_formadd(&m_headForm,
		&lastptr,
		CURLFORM_COPYNAME, "access_token",
		CURLFORM_COPYCONTENTS, m_AccessToken.c_str(),
		CURLFORM_END);


	fc = curl_formadd(&m_headForm,
		&lastptr,
		CURLFORM_COPYNAME, "aid",
		CURLFORM_COPYCONTENTS, m_AID.c_str(),
		CURLFORM_END);


	fc = curl_formadd(&m_headForm,
		&lastptr,
		CURLFORM_COPYNAME, "caption",
		CURLFORM_COPYCONTENTS, utf8_caption.c_str(),
		CURLFORM_END);


	/* Fill in the file upload field */ 
	fc = curl_formadd(&m_headForm,
		&lastptr,
		CURLFORM_COPYNAME, "jpg",
		CURLFORM_FILE, photoPath.c_str(), //"d:\\temp\\fbupload.jpg"
		CURLFORM_END);
}


void CFacebook::DeletePhotoForm()
{
	curl_formfree(m_headForm);
}

//
// Note: result string is UTF-8 format
//
int CFacebook::Request( /*string method,*/std::string url, BOOL bSSL, BOOL bMultipart, std::list<std::string> &params, std::string *result )
{
	BOOL bRet = TRUE;
	VECBUFFER byBuffer;
	std::string query;
	std::string utf8;

	//curl_easy_setopt( m_curl, CURLOPT_VERBOSE, TRUE );

	curl_easy_setopt( m_curl, CURLOPT_URL, url.c_str() );

	if (bSSL)
	{
		curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
	}

	//curl_easy_setopt( m_curl, CURLOPT_USERAGENT, "Mozilla 4.0 (compatible; MSIE 6.0; Win32" );	


	if (bMultipart)
	{
		if (m_headForm == NULL)
		{
			return EMSNS_CURL_HEADFORM;
		}

		curl_easy_setopt( m_curl, CURLOPT_HTTPPOST, m_headForm);
	}
	else
	{
		query = get_param_string( params, TRUE );

		//
		// ANSI-to-UTF-8 for the Request
		//
		utf8 = sc::string::ansi_to_utf8(query);

		curl_easy_setopt( m_curl, CURLOPT_POST, TRUE );
		curl_easy_setopt( m_curl, CURLOPT_POSTFIELDS, utf8.c_str( ) );
	}


	curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, wrapper_write_callback );
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
				sc::writeLogError(sc::string::format("CFacebook::Request() - cURL response code is an error(%1%).", statLong));
				
				bRet = FALSE;
				goto _fb_request_end;
			}

			if (byBuffer.empty())
			{
				sc::writeLogError(std::string("CFacebook::Request() - response buffer is empty!"));

				bRet = FALSE;
				goto _fb_request_end;
			}
//#if defined(_RELEASED) || defined(_DEBUG)			
			sc::writeLogInfo(sc::string::format("CFacebook::Request() - Response Code: %1%", statLong));
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
				sc::writeLogInfo(sc::string::format("CFacebook::Request() - Content type: %1%", statString));
			}
		}

        // 다운로드한 문서의 크기를 얻어온다.
        if( CURLE_OK == curl_easy_getinfo( m_curl , CURLINFO_SIZE_DOWNLOAD , &statDouble ) )
		{
			sc::writeLogInfo(sc::string::format("CFacebook::Request() - Download size: %1%bytes", statDouble));
        }

        // 
        if( CURLE_OK == curl_easy_getinfo( m_curl , CURLINFO_SPEED_DOWNLOAD , &statDouble ) )
		{
			sc::writeLogInfo(sc::string::format("CFacebook::Request() - Download speed: %1%bytes/sec", statDouble));
        }
//#endif

	
	//
	// UTF-8-to-ANSI for the result
	//	
		*result = std::string( sc::string::utf8_to_ansi(&byBuffer[0], (int)byBuffer.size()) );

		if (result->length() <= 0)
		{
			sc::writeLogError(std::string("CFacebook::Request() - result is empty"));
			
			//return nUserNum;
			bRet = FALSE;
			goto _fb_request_end;
		}
	}


	//
	//result should be parsed for good-fashioned error handling.
	//
	// especially,
	/* FACEBOOK: duplicate status message */
	/* TWITTER: status is a duplicate */


/*
	if (bMultipart)
		DeletePhotoForm();
*/
	


	//
	// CURLE_SSL_CONNECT_ERROR
	//
	//if (ccOK == CURLE_SSL_CONNECT_ERROR)
_fb_request_end:
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


std::string CFacebook::get_signature( std::list<std::string> &params )
{
	std::string str = get_param_string( params, FALSE );

	/*
	if( m_bSession )
		str.append( session_secret );
	else
	*/
	
	str.append( m_customInfo.AppSecret/*FB_APP_SECRET*/ );

	return md5(str);
}

std::string CFacebook::md5( std::string &str )
{
	MD5_CTX state;
	unsigned char digest[16];

	char hex_output[16*2 + 1];
	int di;

	MD5_Init(&state);
	MD5_Update(&state, str.c_str(), (unsigned int)strlen(str.c_str()) );
	MD5_Final(digest, &state);

	for( di = 0; di < 16; ++di )
		sprintf_s( hex_output+di * 2, sizeof(hex_output), "%02x", digest[di] );

	return std::string( hex_output );
}


std::string CFacebook::get_param_string( std::list<std::string> &params, BOOL separate )
{
	std::list<std::string>::iterator iter;
	std::string str;

	params.sort( );

	for( iter = params.begin( ); iter != params.end( ); iter++ )
	{
		str.append( *iter );
		if( separate )
			str.append( "&" );
	}

	if( separate )
		str.erase( str.size() - 1, 1 );

	return str;
}

/* following methods is moved into SigmaCore StringUtil.

//
// simple UTF-8 converting functions
//
std::string CFacebook::utf8_to_ansi(char *utf8, int length)		//Overloaded function using char-array
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


std::string CFacebook::ansi_to_utf8(std::string& ansi)
{
	WCHAR unicode[MAX_BUFFER_LENGTH];
	char utf8[MAX_BUFFER_LENGTH];

	memset(unicode, 0, sizeof(unicode));
	memset(utf8, 0, sizeof(utf8));

	::MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, unicode, sizeof(unicode));
	::WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, sizeof(utf8), NULL, NULL);

	return std::string(utf8);
}


std::string CFacebook::utf8_to_ansi(std::string &utf8)
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



size_t wrapper_write_callback( void *ptr, size_t size, size_t nmemb, void *userp )
{
	return CFacebook::write_callback( ptr, size, nmemb, userp );
}



//
// write_callback function could be called several times for a Request!!
// Therefore, result string MUST be appended to userp but not just copy.
//

size_t CFacebook::write_callback( void *ptr, size_t size, size_t nmemb, void *userp )
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
