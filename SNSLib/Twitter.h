#pragma once

//
//mjeon.sns.twitter
//

#include <string>
#include <list>
#include <vector>

#include "../=cURL/include/curl/curl.h"
#include "oauth\oauth.h"
#include "SNS.h"

// Definitions for RAN-Online Twitter	


//#define TW_CONSUMER_KEY				"r9DOo3z0eUI2jQ6QBg2aw"
//#define TW_CONSUMER_SECRET			"LvkR0PzqBXOj4bndVKrg9OQtXC45WNCP7Nbn9jOMc"

#define TW_REQUEST_TOKEN_URL			"https://api.twitter.com/oauth/request_token"	//HTTP POST
#define TW_ACCESS_TOKEN_URL				"https://api.twitter.com/oauth/access_token"	//HTTP POST

//#define TW_AUTHORIZE_URL				"https://api.twitter.com/oauth/authorize"		//HTTP GET

//
//mjeon.sns
//TW_AUTHENTICATE_URL is the replacement of TW_AUTHORIZE_URL for forcing users login
// everytime they tries to login.
//
#define TW_AUTHENTICATE_URL				"https://api.twitter.com/oauth/authenticate?force_login=true" //HTTP GET


//#define TW_REST_STATUS_UPDATE_URL		"https://twitter.com/statuses/update.xml"		//Tweet
#define TW_REST_STATUS_UPDATE_URL		"https://api.twitter.com/1/statuses/update.xml"		//Tweet

#define TW_REST_FRIENDSHIPSCREATE_URL	"https://twitter.com/friendships/create.xml"	//Follow
#define TW_REST_FRIENDSHIPSDESTROY_URL	"https://twitter.com/friendships/destroy.xml"	//UnFollow
#define TW_REST_FRIENDSHIPSSHOW_URL		"https://twitter.com/friendships/show.xml"


/*
 Note 1:
 서비스 프로바이더는 모든 요청에 대하여 HMAC-SHA1 시그네쳐를 지원하며, 
 보안(HTTPS)요청일 경우에만 PLAINTEXT을 사용할 수 있음을 말해줍니다.
*/

/*
 Note 2:
 OAuth 1.0a added the PIN to solve a social engineering attack to which OAuth 1.0 was vulnerable.
 But the PIN is only necessary to be entered manually by the user if your application is a desktop/iphone app.
 If it's a web app the PIN should flow automatically as part of the browser redirects
 and the user shouldn't have to see or enter this 
*/

/*
 Note 3:
 If you don't specify an oauth_callback parameter with the URL you'd like the user redirected to
 when they accept, it will default to the pin-based authentication mechanism.
 Using oauth_callback=oob will also trigger the pin-based authentication flow.
*/

/* 
 Note 4:
 한번 발급된 access token 계속 사용하기
  Twitter 정책 상 한번 발급된 access token은 사용자가 명시적으로 Twitter 홈페이지의
  Settings -> Connections 페이지에서 revoke 처리하기 전까지는 계속 저장된 채로 유지됩니다.
  그리고, 매번 로그인할 때마다 PIN 입력하고 새로운 access token을 발급받는 것은 꽤 번거로운 일이지요.
  그래서, 한번 발급된 access token을 계속 사용하도록 설정을 해두면 여러 모로 편리합니다.
*/

/*
	Authenticating with OAuth is easiest if you use a library in your favorite programming language.
	
	The OAuth request cycle is roughly:
	
	- Retrieve a request token
	- Request user authorization by sending the user to a Twitter.com login page
	- Exchange the request token for an access token
*/



/*
OAuth at Twitter

OAuth can be confusing because there are a few different variations on how it works.

Here's a few key takeaways from the team:

- Use header-based OAuth - OAuth provides for means to pass OAuth-related parameters 
  on a query string or to provide them as a HTTP Authorization header.
  Twitter prefers header-based auth because it separates concerns, makes debugging easier, 
  and avoids common issues with under or over URL escaping parameters.
  Most OAuth libraries let you choose which kind of OAuth you are executing.
  Go for the gold. Go for header-based OAuth.

- Use SSL for /oauth/* end points - for all steps of the OAuth dance like request_token, access_token, 
  and authorize, use SSL

- Use api.twitter.com - for all your OAuth steps, use api.twitter.com as the hostname, not just "twitter.com"

- Always use an explicit oauth_callback - It is recommended that you specify a default OAuth callback
  in your client record, but explicitly declare your oauth_callback on each request token fetch request
  your application makes. 
  By dynamically setting your oauth_callback, you can pass additional state information back 
  to your application and control the experience best.
  If using the PIN code flow, specify your oauth_callback as "oob".
*/



#define MAX_BUFFER_LENGTH	4096

typedef std::vector<char> VECBUFFER;
typedef std::vector<char>::iterator VECBUFFER_ITER;

typedef enum {TWM_GETURL = 0, TWM_AUTH = 1, TWM_TWEET = 2, TWM_FOLLOW = 3, TWM_UNFOLLOW = 4} TW_METHOD;


struct TWCUSTOMINFOS
{	
	std::string ConsumerKey;
	std::string ConsumerSecret;
};


class CTwitter {
public:

	static CTwitter * GetInstance();
	static void	CleanUp();

	void	SetupCustomInfo(TWCUSTOMINFOS &custominfo);

	//void	LinkConsole(CListBox *plb)	{ m_plbConsole = plb; }


	int		GetRequestToken();
	int		MakeAuthURL();
	int		GetAccessToken( std::string PIN );
	int		TweetMsg( std::string &msg );

	std::string&    GetAToken()						    { return m_aToken; }
	void	        SetAToken( std::string &AToken )	{ m_aToken = AToken; }

	std::string&    GetUID()						    { return m_UserID; }
	void	        SetUID( std::string &UID )			{ m_UserID = UID; }	

	std::string&	GetATokenS()					    { return m_aTokenSecret; }
	void	        SetATokenS( std::string &ATokenS )	{ m_aTokenSecret = ATokenS; }

	// bUserID: TRUE -> name is UserID
	//			FALSE-> name is ScreenName
	int		Follow( BOOL bUserID, std::string &name );
	int		UnFollow( BOOL bUserID, std::string &name );

	void	Reset();

	// Basic mjeon tools
	
	void	SetFollowName( BOOL bUserID, std::string &name ) { m_bUserID = bUserID; m_FollowName = name; }
	void	SetUnFollowName( BOOL bUserID, std::string &name ) { SetFollowName( bUserID, name ); }
	int		Request( std::string url, TW_METHOD httpMethod, BOOL bSSL, const char *token, const char *tokenSecret, const char *TweetMsg, std::string *result );

	const char* GetAuthURL() { return m_urlAuth.c_str(); }

	//
	// simple UTF-8 converting functions
	//
	//std::string	utf8_to_ansi(char *utf8, int length);		//Overloaded function using char-array
	//std::string	ansi_to_utf8(std::string& ansi);
	//std::string	utf8_to_ansi(std::string &utf8);


	~CTwitter();

private:

	CURL			*m_curl;
	CURLcode		m_result;

	std::string		m_PIN;
	std::string		m_urlAuth;		// URL for Authorize (PINcode URL)	
	
	std::string		m_UserID;			// My User ID
	std::string		m_ScreenName;		// My Screen Name

	std::string		m_FollowName;		// UserID or ScreenName for following/Unfollowing.
	BOOL			m_bUserID;			// Is FollowName UserID ?

	TWCUSTOMINFOS	m_customInfo;

public:
	std::string		m_rToken;		// Request Token
	std::string		m_rTokenSecret;	// Request Token Secret
	std::string		m_aToken;		// Access  Token
	std::string		m_aTokenSecret;	// Access  Token Secret

private:
    
	//char		m_bfCallback[MAX_BUFFER_LENGTH];

	std::vector<char> m_vecXML;	//XML buffer which is used in write_callback_function.

	
	struct curl_httppost * m_headForm;	//in case of multipart/form-data, this is the headptr of the form-list.

	static CTwitter	*m_pInstance;
	//CListBox	*m_plbConsole;

	CTwitter();
	

public:	
	static size_t write_callback( void *ptr, size_t size, size_t nmemb, void *userp );
	static size_t header_callback( void *ptr, size_t size, size_t nmemb, void *userp );
	static int	  debug_callback(void *ptr, curl_infotype info, char * p, size_t size, void *pv);
};

#ifdef __cplusplus
extern "C" {
#endif

	size_t tw_wrapper_write_callback( void *ptr, size_t size, size_t nmemb, void *userp );

#ifdef __cplusplus
}
#endif
