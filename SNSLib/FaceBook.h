#pragma once

//
//mjeon.sns.facebook
//

//#include <cstdlib>
//#include <typeinfo>

#include <string>
#include <list>
#include <vector>

#include "../=cURL/include/curl/curl.h"
#include "SNS.h"

// Definitions for RAN-Online Facebook


//#define FB_APP_ID				"151612284850725"
//#define FB_API_KEY			"6621f90a9d435c42e7040e2d1e2f6942"
//#define FB_APP_SECRET			"11dc7cfd51c85d282a240b8b37ed95b3"

#define FB_REST_SERVER			"http://api.facebook.com/restserver.php"			// HTTP
#define FB_LOGIN_URL			"http://api.facebook.com/login.php"					// HTTP

#define FB_LOGOUT_URL			"http://www.facebook.com/logout.php"				// HTTP
#define FB_LOGOUT_URL_M			"http://m.facebook.com/logout.php" //?confirm=1&next=http://www.ran-world.com/ko/fb_logout.htm"

#define FB_EXCHANGE_SESSION		"https://graph.facebook.com/oauth/exchange_sessions"// HTTPS
#define FB_METHOD_URL			"https://api.facebook.com/method/"					// HTTPS

//
//http://www.facebook.com/addfriend.php?id=USER_ID
//
#define FB_ADD_FRIEND			"http://www.facebook.com/addfriend.php"				// HTTP


#define FB_RAN_ALBUM			"RAN Online"

#define MAX_BUFFER_LENGTH	4096

typedef std::vector<char> VECBUFFER;
typedef std::vector<char>::iterator VECBUFFER_ITER;


struct URLINFO
{
	std::string url;
	std::string name;
	std::string desc;
};

struct FBCUSTOMINFOS
{
	std::string AppID;
	std::string ApiKey;
	std::string AppSecret;

	URLINFO urlLOGINCANCEL;	
	URLINFO urlLOGOUTNEXT;
	URLINFO urlIMAGELOGO;
	URLINFO urlIMAGEWELCOME;
	URLINFO urlIMAGELEVELUP;
	URLINFO urlIMAGEENCHANTED;
	URLINFO urlHREF;
	URLINFO urlACTIONLINK;
};


class CFacebook {
public:

	static CFacebook * GetInstance();
	static void	CleanUp();

	void	SetupCustomInfo(FBCUSTOMINFOS &custominfo);

	//void	LinkConsole(CListBox *plb)	{ m_plbConsole = plb; }

	//BOOL	GetAccessToken(char **ppURL);	// Graph API

	int		GetAuthToken();
	void	GetAuthURL(std::string &url);
	int		GetSession();
	int		GetAccessToken();	//ExchangeSession

	std::string& GetAToken()						{ return m_AccessToken; }
	void	SetAToken( std::string &AToken )		{ m_AccessToken = AToken; }

	std::string& GetUID()						{ return m_UID; }
	void	SetUID( std::string &UID)			{ m_UID = UID; }	
	
	std::string&	GetSKey()						{ return m_SessionKey; }
	void	SetSKey( std::string &SKey )			{ m_SessionKey = SKey; }


	void	Reset();
	int		LogOut();

	
	void	GetAddFriendURL(std::string &id, std::string &url);
	void	GetLogOutURL(std::string &url);

	int		AddFriend(std::string &id);
	int		WallPost(std::string &msg, EMSNS_ACTION_BY by = EMSNS_USER);
	int		GetAlbumID();
	int		CreateAlbum();
	int		UploadPhoto(std::string &caption, std::string &path);
	int		Request( /*std::string method,*/std::string url, BOOL bSSL, BOOL bMultipart, std::list<std::string> &params, std::string *result );


	void	CreatePhotoForm(std::string &caption, std::string &path);
	void	DeletePhotoForm();

	//
	// simple UTF-8 converting functions
	//
	//std::string	utf8_to_ansi(char *utf8, int length);		//Overloaded function using char-array
	//std::string	ansi_to_utf8(std::string& ansi);
	//std::string	utf8_to_ansi(std::string &utf8);


	~CFacebook();

private:

	CURL		*m_curl;
	CURLcode	m_result;

	//char		m_bfCallback[MAX_BUFFER_LENGTH];

	std::vector<char> m_vecXML;	//XML buffer which is used in write_callback_function.

	std::string		m_authToken;

	std::string		m_AccessToken;
	std::string		m_SessionKey;	
	std::string		m_Expires;

	std::string		m_UID;	//UserID
	std::string		m_AID;	//AlbumID

	BOOL		m_bSession;

	int			m_nCallID;

	FBCUSTOMINFOS m_customInfo;

	struct curl_httppost * m_headForm;	//in case of multipart/form-data, this is the headptr of the form-list.

	static CFacebook	*m_pInstance;
	//CListBox	*m_plbConsole;

	CFacebook();	

	std::string	get_signature( std::list<std::string> &params );

	std::string	md5( std::string &str );

public:
	static std::string get_param_string( std::list<std::string> &params, BOOL separate );
	static size_t write_callback( void *ptr, size_t size, size_t nmemb, void *userp );
	static size_t header_callback( void *ptr, size_t size, size_t nmemb, void *userp );
	static int	  debug_callback(void *ptr, curl_infotype info, char * p, size_t size, void *pv);
};

#ifdef __cplusplus
extern "C" {
#endif

	size_t wrapper_write_callback( void *ptr, size_t size, size_t nmemb, void *userp );

#ifdef __cplusplus
}
#endif
