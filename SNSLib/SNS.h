#pragma once

//
//mjeon.sns
//
/* FACEBOOK: duplicate status message */
/* TWITTER: status is a duplicate */



#define CONCAT(a,b)	a ## b



enum EMSNS_CORE_RESULT {
	EMSNS_NOK				=0,		// NOT OK: Just a zero-base value. (default for variables).
	EMSNS_OK				=1,		// OK		

	EMSNS_CURL_NOK			=2,		// FB: cURL perform failure. (FB and TW use this.)
	EMSNS_CURL_HEADFORM		=3,		// FB: In case of Multipart, headForm is NULL.

	EMSNS_XML_EMPTY			=4,		// FB: XML is empty (currently, XML is being used only for FB.)
	EMSNS_XML_EX			=5,		// FB: An error occured during XML parsing, refer to additional XML exception msg.

	EMSNS_JSON_READFAIL		=6,		// FB: json read function failure.
	EMSNS_JSON_NOOBJECT		=7,		// FB: The json result does not include the expected token or value.
	EMSNS_JSON_INVALID		=8,		// FB: The json result exists but the value's type is invalid.
	EMSNS_JSON_NULL			=9,		// FB: The json result has null.
	EMSNS_JSON_EMPTY		=10,	// FB: The json result is empty.
	EMSNS_JSON_WRONG		=11,	// FB: The json result is wrong type.
	

	EMSNS_FB_NOALBUM		=12,	// FB: Could not get AlbumID.

	EMSNS_TW_GETURLFAIL		=13,	// TW: TWM_GETURL failure.
	EMSNS_TW_AUTHFAIL		=14,	// TW: TWM_AUTH failure
	EMSNS_TW_TWEETFAIL		=15,	// TW: TWM_TWEET failure.
	EMSNS_TW_FOLLOWFAIL		=16,	// TW: TWM_FOLLOW failure.
	EMSNS_TW_UNFOLLOWFAIL	=17,	// TW: TWM_UNFOLLOW failure.
	EMSNS_TW_EMPTYNAME		=18,	// TW: Follower/Unfollower name is empty.
	EMSNS_TW_EMPTYRESULT	=19,	// TW: result string is empty.
	EMSNS_TW_INVAILDRESULT	=20,	// TW: Result format is invalid.
	EMSNS_TW_NOPIN			=21,	// TW: No PIN value exists. It's required in order to get AccessToken.
	EMSNS_TW_NOACCESSTOKEN	=22,	// TW: Couldn't get AccessToken.
	EMSNS_TW_NEEDMOREINFO	=23,		// TW: There's only one token in result. TokenS and UID are also required.
	EMSNS_TW_EMPTYMSG		=24,	// TW: TweetMsg is empty.
};


//
//mjeon.sns
//
enum EMSNS_ACTION_BY {
	EMSNS_USER				=0,		// User made an custom message using SNS-UI in RAN client.
	EMSNS_START_SNS			=1,		// User finished authenticating and started using SNS in RAN.
	EMSNS_LEVEL_UP			=2,		// Level Up
	EMSNS_ENCHANT_SUCCESS	=3,		// Succeed enchant	
};



namespace SNS {

	//
	// SNS process has one thread-flow. Therefor, no need lock.
	//
	extern std::string	exWhat;	//XML exception message: ex.what()

	extern BOOL	bInitialized;

	//initSNS() MUST be called once before using of SNS.
	int initSNS();

	BOOL BypassCookie(std::string &url);
	BOOL DeleteCookie();

	//cleanSNS() MUST be called once after you finished use of SNS.
	void cleanSNS();
};

