
//
//mjeon.sns.twitter
//

#include "stdafx.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/erase.hpp>

#include "Twitter.h"

#include "../SigmaCore/XML/2.5.3/ticpp.h"
#include "../SigmaCore/Json/json_spirit.h"
#include "md5.h"



int CTwitter::GetRequestToken()
{	
	std::string result;
	int nResult = EMSNS_NOK;

	nResult = Request( TW_REQUEST_TOKEN_URL, TWM_AUTH, TRUE, NULL, NULL, NULL, &result );
	if( nResult != EMSNS_OK )
		return nResult;


	if( result.length( ) == 0 )
	{		
		return EMSNS_TW_EMPTYRESULT;
	}

	std::vector<std::string> vecSplit;	
	boost::split( vecSplit, result, boost::algorithm::is_any_of( "&" ) );

	if ( 3 == vecSplit.size() )
	{
		boost::algorithm::erase_first( vecSplit[0], "oauth_token=" );
		m_rToken = vecSplit[0];

		boost::algorithm::erase_first( vecSplit[1], "oauth_token_secret=" );
		m_rTokenSecret = vecSplit[1];
	}
	else
	{
		return EMSNS_TW_INVAILDRESULT;
	}	

	return EMSNS_OK;
}


int CTwitter::MakeAuthURL()
{
	std::string result;
	int nResult = EMSNS_NOK;

	//nResult = Request( TW_AUTHORIZE_URL, TWM_GETURL, FALSE, m_rToken.c_str(), m_rTokenSecret.c_str(), NULL, &result );

	nResult = Request( TW_AUTHENTICATE_URL, TWM_GETURL, FALSE, m_rToken.c_str(), m_rTokenSecret.c_str(), NULL, &result );
	if( nResult != EMSNS_OK )
		return nResult;

	if( result.length( ) == 0 )
	{		
		return EMSNS_TW_EMPTYRESULT;
	}

	m_urlAuth = result;

	return EMSNS_OK;
}


int CTwitter::GetAccessToken( std::string PIN )
{
	if ( PIN.length() <= 0 )
	{
		//CConsoleMessage::GetInstance()->PrintTo(m_plbConsole, _T("PIN is empty."));

		return EMSNS_TW_NOPIN;
	}

	m_PIN = PIN;

	std::string result;
	int nResult = EMSNS_NOK;

	nResult = Request( TW_ACCESS_TOKEN_URL, TWM_AUTH, TRUE, m_rToken.c_str(), m_rTokenSecret.c_str(), NULL, &result );
	if( nResult != EMSNS_OK )
		return nResult;


	if( result.length( ) == 0 )
	{		
		return EMSNS_TW_EMPTYRESULT;
	}


	std::vector<std::string> vecSplit;
	boost::split( vecSplit, result, boost::algorithm::is_any_of( "&" ) );

	size_t splitSize = vecSplit.size();

	if ( 0 >= splitSize )
	{
		return EMSNS_TW_NOACCESSTOKEN;
	}

	if ( 3 > splitSize )
	{
		return EMSNS_TW_NEEDMOREINFO;
	}

	BOOL foundAToken = FALSE;
	BOOL foundATokenS = FALSE;
	BOOL foundUID = FALSE;
	BOOL foundSNAME = FALSE;

	if ( 3 <= splitSize )
	{
		for (size_t i=0; i<splitSize; i++)
		{
			if ( !foundAToken && (vecSplit[i].find("oauth_token=") != std::string::npos) )
			{
				boost::algorithm::erase_first( vecSplit[i], "oauth_token=" );		
				m_aToken = vecSplit[i];

				foundAToken = TRUE;

				continue;
			}

			if ( !foundATokenS && (vecSplit[i].find("oauth_token_secret=") != std::string::npos) )
			{
				boost::algorithm::erase_first( vecSplit[i], "oauth_token_secret=" );
				m_aTokenSecret = vecSplit[i];

				foundATokenS = TRUE;

				continue;
			}

			if ( !foundUID && (vecSplit[i].find("user_id=") != std::string::npos) )
			{
				boost::algorithm::erase_first( vecSplit[i], "user_id=" );
				m_UserID = vecSplit[i];

				foundUID = TRUE;

				continue;
			}

			if ( !foundSNAME && (vecSplit[i].find("screen_name=") != std::string::npos) )
			{
				boost::algorithm::erase_first( vecSplit[i], "screen_name=" );
				m_ScreenName = vecSplit[i];

				continue;
			}
		}
	}

	return ( (foundATokenS && foundATokenS && foundUID) ? EMSNS_OK : EMSNS_TW_NOACCESSTOKEN );	//SNAME is optional;


	//return ( ( (m_aToken.length() != 0) && (m_aTokenSecret.length() != 0) ) ? EMSNS_OK : EMSNS_TW_NOACCESSTOKEN ); //TRUE;
}


int CTwitter::TweetMsg( std::string &msg )
{
	if ( msg.length() <= 0 )
	{
		//CConsoleMessage::GetInstance()->PrintTo(m_plbConsole, _T("TweetMsg is empty."));

		return EMSNS_TW_EMPTYMSG;
	}

	msg += " #RanOnline";

	std::string result;
	int nResult = EMSNS_NOK;

	nResult = Request( TW_REST_STATUS_UPDATE_URL, TWM_TWEET, TRUE, m_aToken.c_str(), m_aTokenSecret.c_str(), msg.c_str(), &result );
	if( nResult != EMSNS_OK )
		return nResult;


	if( result.length( ) == 0 )
	{		
		return EMSNS_TW_EMPTYRESULT;
	}

	//
	//	ToDo.
	//
	// add some result handling here.
	//
	//
	//


	return EMSNS_OK;
}


int CTwitter::Follow( BOOL bUserID, std::string &name )
{
	if ( name.length() <= 0 )
	{
		//CConsoleMessage::GetInstance()->PrintTo(m_plbConsole, _T("Follow name is empty."));

		return EMSNS_TW_EMPTYNAME;
	}

	SetFollowName ( bUserID, name );

	std::string result;
	int nResult = EMSNS_NOK;

	nResult = Request( TW_REST_FRIENDSHIPSCREATE_URL, TWM_FOLLOW, TRUE, m_aToken.c_str(), m_aTokenSecret.c_str(), NULL, &result );
	if( nResult != EMSNS_OK )
		return nResult;


	if( result.length( ) == 0 )
	{		
		return EMSNS_TW_EMPTYRESULT;
	}

	//
	//	ToDo.
	//
	// add some result handling here.
	//
	//
	//

	return EMSNS_OK;
}


int CTwitter::UnFollow( BOOL bUserID, std::string &name )
{

	if ( name.length() <= 0 )
	{
		//CConsoleMessage::GetInstance()->PrintTo(m_plbConsole, _T("UnFollow name is empty."));

		return EMSNS_TW_EMPTYNAME;
	}

	SetUnFollowName ( bUserID, name );

	std::string result;
	int nResult = EMSNS_NOK;

	nResult = Request( TW_REST_FRIENDSHIPSCREATE_URL, TWM_UNFOLLOW, TRUE, m_aToken.c_str(), m_aTokenSecret.c_str(), NULL, &result );
	if( nResult != EMSNS_OK )
		return nResult;


	if( result.length( ) == 0 )
	{		
		return EMSNS_TW_EMPTYRESULT;
	}

	//
	//	ToDo.
	//
	// add some result handling here.
	//
	//
	//

	return EMSNS_OK;
}


void CTwitter::Reset()
{
	m_rToken.clear();
	m_PIN.clear();
	m_urlAuth.clear();	
	m_UserID.clear();
	m_ScreenName.clear();
	m_FollowName.clear();
	
	m_bUserID = FALSE;

	m_rToken.clear();
	m_rTokenSecret.clear();
	m_aToken.clear();
	m_aTokenSecret.clear();
}



