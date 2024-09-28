
//
//mjeon.sns.facebook
//
#include "stdafx.h"
#include <strsafe.h>
#include "FaceBook.h"

#include "../SigmaCore/XML/2.5.3/ticpp.h"
#include "../SigmaCore/Json/json_spirit.h"
#include "md5.h"

//#include "../RanClientLib/G-Logic/GLSNSData.h"
//#include "../RanClientUILib/Interface/GameTextControl.h"

int CFacebook::GetAuthToken()
{
	std::list<std::string> params;
	std::string xml;
	int nResult = EMSNS_NOK;

	//mjeon
	// auth.createToken => Authentication Token
	// This method is intended for desktop applications, not for Facebook canvas page applications.

	params.push_back( "method=auth.createToken" );
	params.push_back( "api_key=" + m_customInfo.ApiKey/*std::string(FB_API_KEY)*/ );
    std::stringstream call_str;
	call_str << "call_id=" << m_nCallID++;
	params.push_back( call_str.str() );

	params.push_back( "sig=" + get_signature(params) );


	nResult = Request( FB_REST_SERVER, FALSE, FALSE, params, &xml );
	if( nResult != EMSNS_OK )
		return nResult;


	if( xml.length( ) == 0 )
		return EMSNS_XML_EMPTY;


	try
	{
		ticpp::Document doc;
		doc.Parse( xml );

		ticpp::Element* pHead = NULL;		

		pHead = doc.FirstChildElement("auth_createToken_response");			
		
		m_authToken = pHead->GetText();
	}
	catch ( ticpp::Exception& ex )
	{
		//CConsoleMessage::GetInstance()->PrintTo(m_plbConsole, _T("%s"), ex.what() );
		SNS::exWhat = ex.what();	// C-str to std::string

		return EMSNS_XML_EX;
	}

/*
	ticpp::Document Xml;
	ticpp::Element* pElem = NULL;


	Xml.Parse( xml );	

	//
	// This methos MUST be fixed in order to use Exception for error handling.
	//
	pElem = (ticpp::Element*) Xml.FirstChild( "auth_createToken_response", FALSE );

	if ( NULL == pElem )
		return FALSE;

	m_authToken = pElem->GetText();
*/

	return EMSNS_OK;    
}


void CFacebook::GetAuthURL(std::string &url)
{
	url.append( FB_LOGIN_URL );
	url.append( "?api_key=" );
	url.append( m_customInfo.ApiKey/*FB_API_KEY*/ );

	url.append( "&connect_display=popup" );
	url.append( "&v=1.0" );
	
	//url.append( "&next=http://www.ran-world.com/" );

	std::string cancel_url = std::string("&cancel_url=") + m_customInfo.urlLOGINCANCEL.url;//GLCONST_SNS::strSNS_LOGIN_CANCEL_URL; //+ std::string("fb_cancel.htm");
	url.append( cancel_url.c_str() );

	url.append( "&skipcookie=1" );

	url.append( "&fbconnect=true" );
	url.append( "&return_session=true" );
	url.append( "&req_perms=publish_stream,user_photos,offline_access" );	//note: offline_access

	url.append( "&auth_token=" ); //auth_token
	url.append( m_authToken );
}


int CFacebook::GetSession()
{
	std::list<std::string> params;
	std::string xml;
	int nResult = EMSNS_NOK;

	params.push_back( "method=auth.getSession" );
	params.push_back( "auth_token=" + m_authToken );
	params.push_back( "api_key=" + m_customInfo.ApiKey/*std::string(FB_API_KEY)*/ );

    std::stringstream call_str;
	call_str << "call_id=" << m_nCallID++;
	params.push_back( call_str.str() );

	params.push_back( "sig=" + get_signature(params) );

	nResult = Request(FB_REST_SERVER, FALSE, FALSE,  params, &xml );
	if( nResult != EMSNS_OK )
		return nResult;

	if( xml.length( ) == 0 )
		return EMSNS_XML_EMPTY;


	try
	{
		ticpp::Document doc;
		doc.Parse( xml );

		ticpp::Element* pHead = NULL;
		ticpp::Element* pElem = NULL;

		pHead = doc.FirstChildElement("auth_getSession_response");

		pElem = pHead->FirstChildElement( "session_key" );		
		m_SessionKey = pElem->GetText();

		pElem = pHead->FirstChildElement( "uid" );		
		m_UID = pElem->GetText();		

		pElem = pHead->FirstChildElement( "expires", false ); // Expires is maybe optional
		m_Expires = pElem->GetText();
	}
	catch ( ticpp::Exception& ex )
	{
		//CConsoleMessage::GetInstance()->PrintTo(m_plbConsole, _T("%s"), ex.what() );
		SNS::exWhat = ex.what();	// C-str to std::string

		return EMSNS_XML_EX;
	}

	m_bSession = TRUE;
	return EMSNS_OK;
}


int CFacebook::GetAccessToken()	//ExchangeSession
{
	std::list<std::string> params;
	std::string xml;
	int nResult = EMSNS_NOK;

	params.push_back( "type=client_cred" );
	params.push_back( "client_id=" + m_customInfo.AppID/*std::string(FB_APP_ID)*/ );
	params.push_back( "client_secret=" + m_customInfo.AppSecret/*std::string(FB_APP_SECRET)*/ );
	params.push_back( "sessions=" + m_SessionKey );


	nResult = Request(FB_EXCHANGE_SESSION, TRUE, FALSE, params, &xml );
	if( nResult != EMSNS_OK )
		return nResult;

	if( xml.length( ) == 0 )
		return EMSNS_XML_EMPTY;


	//result is JSON

	json_spirit::mValue value;

	if ( FALSE == json_spirit::read(xml, value) )
	{
		// Json read error
		return EMSNS_JSON_READFAIL;
	}

	json_spirit::Value_type t = value.type();

	if (value.is_null())
		return EMSNS_JSON_EMPTY;

	if (t != json_spirit::array_type)
		return EMSNS_JSON_EMPTY;

	json_spirit::mArray a = value.get_array();	// typedef std::vector< Value > Array;


	if (a[0].is_null())
		return EMSNS_JSON_EMPTY;	

	const json_spirit::mObject& o = a[0].get_obj();

	if ( o.empty() )
		return EMSNS_JSON_EMPTY;

	//json_spirit::mObject o = value.get_obj();

	json_spirit::mObject::const_iterator i = o.find("access_token");

	if ( i == o.end() || i->first != "access_token" )
	{
		return EMSNS_JSON_NOOBJECT;
	}
	else
	{
		if ( json_spirit::str_type != i->second.type() )
		{
			return EMSNS_JSON_INVALID;
		}
		else
		{
			m_AccessToken = i->second.get_str();			
		}
	}

	return EMSNS_OK;
}


void CFacebook::Reset()
{
	m_authToken.clear();
	m_AccessToken.clear();
	m_SessionKey.clear();	
	m_Expires.clear();

	m_UID.clear();	//UserID
	m_AID.clear();	//AlbumID

	m_bSession = FALSE;
}


int CFacebook::AddFriend(std::string &id)
{
	std::list<std::string> params;
	std::string xml;
	int nResult = EMSNS_NOK;
	
	params.push_back( "id=" + id );	

	nResult = Request(FB_ADD_FRIEND, FALSE, FALSE, params, &xml );
	if( nResult != EMSNS_OK )
		return nResult;

	//if( xml.length( ) == 0 )
	//	return EMSNS_XML_EMPTY;

	return EMSNS_OK;
}


void CFacebook::GetAddFriendURL(std::string &id, std::string &url)
{
	url = std::string(FB_ADD_FRIEND) + std::string("?id=") + id;	
}

void CFacebook::GetLogOutURL(std::string &url)
{
	//url = string(FB_LOGOUT_URL) + string("?api_key=") + string(FB_API_KEY) + string("session_key=") + m_SessionKey;
	url = std::string(FB_LOGOUT_URL_M) + std::string("?confirm=1") + std::string("&next=") + m_customInfo.urlLOGOUTNEXT.url;//GLCONST_SNS::strSNS_LOGOUT_NEXT_URL;
}


int CFacebook::LogOut()
{
	/*
	std::list<string> params;
	string xml;
	int nResult = EMSNS_NOK;

	std::string nexturl = std::string("&next=") + std::string(EMSNS_NEXT_URL) + std::string("fb_logout.htm");

	params.push_back( "?confirm=1" );
	params.push_back( nexturl );
	//params.push_back( "api_key="  + string(FB_API_KEY) );
	//params.push_back( "session_key=" + m_SessionKey );

	nResult = Request(FB_LOGOUT_URL_M, FALSE, FALSE, params, &xml );
	if( nResult != EMSNS_OK )
		return nResult;

	//if( xml.length( ) == 0 )
	//	return EMSNS_XML_EMPTY;
*/
	return EMSNS_OK;
}


int CFacebook::WallPost(std::string &msg, EMSNS_ACTION_BY by)
{
	std::list<std::string> params;
	std::string xml;
	int nResult = EMSNS_NOK;

	params.push_back( "method=" + std::string("stream.publish"));
	params.push_back( "access_token=" + m_AccessToken );	
	
	
	//
	//mjeon.sns.facebook.json encoding
	//

	json_spirit::Object oAttachment;
	json_spirit::Array	aMedia;
	json_spirit::Object oImage;


	json_spirit::Array	aLinks;
	json_spirit::Object oLink;

	

	/*	action_links

	[ { 'text': 'Code', 'href': 'http://github.com/facebook/connect-js' } ]

	*/

	
	/* JSON-encoded format of attachment object
	
	{'media':[{'type':'image','src':'$http_imgurl/logo.jpg','href':'$http_fburl'}]};     
	
	*/
	
	
	/* example of JSON-encoded attachment object

	{"media": [
	{ 
		"type": "image", 
		"src": "http://icanhascheezburger.files.wordpress.com/2009/03/funny-pictures-kitten-finished-his-milk-and-wants-a-cookie.jpg", 
		"href": "http://icanhascheezburger.com/2009/03/30/funny-pictures-awlll-gone-cookie-now/"
	}, 
	{
		"type": "image", 
		"src": "http://photos.icanhascheezburger.com/completestore/2009/1/18/128768048603560273.jpg", 
		"href": "http://ihasahotdog.com/upcoming/?pid=20869"
	}]
	}

	*/

	std::string srcURL;
	std::string name;
	std::string desc;
	std::string href;	
	
	switch (by)
	{
	case EMSNS_USER:
		{
			params.push_back( "message=" + msg );

			// 국가별로 그림을 달리해야할 경우가 있다! 대만-용온라인

			srcURL = m_customInfo.urlIMAGELOGO.url;//GLCONST_SNS::strSNS_IMAGE_URL + GLCONST_SNS::strSNS_IMAGE_LOGO;

			name = m_customInfo.urlIMAGELOGO.name;//string(ID2GAMEINTEXT("EMSNS_FB_NAME_LOGO"));
			desc = m_customInfo.urlIMAGELOGO.desc;//string(ID2GAMEINTEXT("EMSNS_DESC_LOGO"));	
		}
		break;
	case EMSNS_START_SNS:
		{
			srcURL = m_customInfo.urlIMAGEWELCOME.url;//GLCONST_SNS::strSNS_IMAGE_URL + GLCONST_SNS::strSNS_IMAGE_WELCOME;

			name = m_customInfo.urlIMAGEWELCOME.name;//string(ID2GAMEINTEXT("EMSNS_FB_NAME_WELCOME"));
			desc = msg + m_customInfo.urlIMAGEWELCOME.desc; //string(ID2GAMEINTEXT("EMSNS_DESC_WELCOME"));			
		}
		break;
	case EMSNS_LEVEL_UP:
		{
			srcURL = m_customInfo.urlIMAGELEVELUP.url;//GLCONST_SNS::strSNS_IMAGE_URL + GLCONST_SNS::strSNS_IMAGE_LEVELUP;

			name = m_customInfo.urlIMAGELEVELUP.name;//string(ID2GAMEINTEXT("EMSNS_FB_NAME_LEVELUP"));
			desc = msg + m_customInfo.urlIMAGELEVELUP.desc;//string(ID2GAMEINTEXT("EMSNS_DESC_LEVELUP"));			
		}
		break;
	case EMSNS_ENCHANT_SUCCESS:
		{
			srcURL = m_customInfo.urlIMAGEENCHANTED.url;//GLCONST_SNS::strSNS_IMAGE_URL + GLCONST_SNS::strSNS_IMAGE_ENCHANTED;

			name = m_customInfo.urlIMAGEENCHANTED.name;//string(ID2GAMEINTEXT("EMSNS_FB_NAME_ENCHANTED"));
			desc = msg + m_customInfo.urlIMAGEENCHANTED.desc;//string(ID2GAMEINTEXT("EMSNS_DESC_ENCHANTED"));			
		}
		break;
	default:
		break;
	}
	href = m_customInfo.urlHREF.url;//GLCONST_SNS::strSNS_HREF;


	//
	//mjeon.sns.facebook.create an image
	//
	oImage.push_back( json_spirit::Pair( "type", "image" ) );	
	//oImage.push_back( json_spirit::Pair( "src", "http://www.ran-world.com/Images/main/tube_btn.png" ) );
	oImage.push_back( json_spirit::Pair( "src", srcURL.c_str() ) );
	oImage.push_back( json_spirit::Pair( "href", href.c_str() ) );

	//
	//mjeon.sns.facebook.create an array of image(s)
	//
	aMedia.push_back( oImage );

	//
	//mjeon.sns.facebook.create an attachment object
	//
	oAttachment.push_back( json_spirit::Pair( "name", name.c_str() ) );
	oAttachment.push_back( json_spirit::Pair( "href", href.c_str() ) );
	//oAttachment.push_back( json_spirit::Pair( "caption", "some captions" ) );
	oAttachment.push_back( json_spirit::Pair( "description", desc.c_str() ) );
	oAttachment.push_back( json_spirit::Pair( "media", aMedia  ) );


	std::string strAttachment;
	strAttachment = write_formatted(oAttachment);

	params.push_back( "attachment=" + strAttachment );



	std::string actionlink = m_customInfo.urlACTIONLINK.url;//GLCONST_SNS::strSNS_ACTION_LINK;
	std::string actionname = m_customInfo.urlACTIONLINK.name;//ID2GAMEINTEXT("EMSNS_FB_ACTION_NAME");

	//
	//mjeon.sns.facebook.create an link
	//
	oLink.push_back( json_spirit::Pair( "text", actionname.c_str() ) );
	oLink.push_back( json_spirit::Pair( "href", actionlink.c_str() ) );

	//
	//mjeon.sns.facebook.create an array of link(s)
	//
	aLinks.push_back( oLink );	

    
	std::string strActionLink;
	strActionLink = write_formatted(aLinks);

	params.push_back( "action_links=" + strActionLink );


	nResult = Request(FB_METHOD_URL, TRUE, FALSE, params, &xml );
	if( nResult != EMSNS_OK )
		return nResult;

	if( xml.length( ) == 0 )
		return EMSNS_XML_EMPTY;


	return EMSNS_OK;
}


int CFacebook::GetAlbumID()
{
	std::list<std::string> params;
	std::string xml;
	int nResult = EMSNS_NOK;

	BOOL	bFound = FALSE;

	params.push_back( "method=" + std::string("photos.getAlbums"));
	params.push_back( "access_token=" + m_AccessToken );

	nResult = Request(FB_METHOD_URL, TRUE, FALSE, params, &xml );
	if( nResult != EMSNS_OK )
		return nResult;

	if( xml.length( ) == 0 )
		return EMSNS_XML_EMPTY;


	//
	//Exception Driven
	//no need to check return value for each functions.

	try
	{
		ticpp::Document doc;
		doc.Parse( xml );

		ticpp::Element* pHead = NULL;
		ticpp::Element* pElem = NULL;

		std::string strAlbumName;

		ticpp::Element *name = NULL;
		ticpp::Element *aid  = NULL;


		pHead = doc.FirstChildElement("photos_getAlbums_response");


		// Only iterates through Element nodes with value "album"
		ticpp::Iterator< ticpp::Element > album( "album" );

		for ( album = album.begin( pHead ); album != album.end(); album++ )
		{
			name = album->FirstChildElement("name");
			strAlbumName = name->GetText();

			if ( strAlbumName.compare(FB_RAN_ALBUM) )
				continue;

			aid = album->FirstChildElement("aid");
			m_AID = aid->GetText();
			bFound = TRUE;
		}
	}	
	catch ( ticpp::Exception& ex )
	{
		//CConsoleMessage::GetInstance()->PrintTo(m_plbConsole, _T("%s"), ex.what() );
		SNS::exWhat = ex.what();	// C-str to std::string

		return EMSNS_XML_EX;
	}

	return (bFound ? EMSNS_OK : EMSNS_FB_NOALBUM);
}


int CFacebook::CreateAlbum()
{
	std::list<std::string> params;
	std::string xml;
	int nResult = EMSNS_NOK;

	BOOL	bFound = FALSE;

	params.push_back( "method=" + std::string("photos.createAlbum"));
	params.push_back( "access_token=" + m_AccessToken );
	params.push_back( "name=" + std::string(FB_RAN_ALBUM) );
	params.push_back( "visible=friends" );								//visible setting
	params.push_back( "location=" + std::string("World of RAN Online") );
	params.push_back( "description=" + std::string("created by RAN Online") );	//Optional

	nResult = Request(FB_METHOD_URL, TRUE, FALSE, params, &xml );
	if( nResult != EMSNS_OK )
		return nResult;

	if( xml.length( ) == 0 )
		return EMSNS_XML_EMPTY;

	try
	{
		ticpp::Document doc;
		doc.Parse( xml );

		ticpp::Element* pHead = NULL;
		ticpp::Element* pElem = NULL;

		pHead = doc.FirstChildElement("photos_createAlbum_response");

		pElem = pHead->FirstChildElement( "aid" );
		m_AID = pElem->GetText();

	}
	catch ( ticpp::Exception& ex )
	{
		//CConsoleMessage::GetInstance()->PrintTo(m_plbConsole, _T("%s"), ex.what() );
		SNS::exWhat = ex.what();	// C-str to std::string

		return EMSNS_XML_EX;
	}

	return EMSNS_OK;
}



int CFacebook::UploadPhoto(std::string &caption, std::string &path)
{
	std::list<std::string> params;
	std::string xml;
	BOOL	bRet = FALSE;

	BOOL	bFound = FALSE;
	int nResult = EMSNS_NOK;

	//string caption("test1");
	//string path("d:\\misc\\싱아-msn.jpg");

	CreatePhotoForm(caption, path);	// Do not need to call DeletePhotoForm. It will be done automatically by Request().
	
	nResult = Request(FB_METHOD_URL, TRUE, TRUE, params, &xml ) || (xml.length( ) == 0);
		
	DeletePhotoForm();

	return nResult;
}
