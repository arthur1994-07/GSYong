#include "pch.h"
#include "./GLSNSData.h"
#include "../GLogicData.h"
#include "../s_NetGlobal.h"
#include "../GLogicDataMan.h"

#include "../../SigmaCore/String/BaseString.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/Common/GLTexFile.h"

#include "../../EngineLib/G-Logic/Glogic.h"
#include "../../EngineLib/Common/IniLoader.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLSNS::GLSNS ()
{

}

GLSNS::~GLSNS ()
{

}

// Note : 파일을 읽어들인다. (bjju.sns)
namespace GLCONST_SNS
{
	bool		bANIMATION_FLAG				= true;

	std::string	strSNS_FB_HREF			    = "http://www.facebook.com/apps/application.php?id=151612284850725";
	std::string	strSNS_IMAGE_URL			= "http://file.ran-world.com/sns/ko/";
	std::string	strSNS_ACTION_LINK			= "http://www.ran-world.com/";
	std::string	strSNS_LOGOUT_NEXT_URL		= "http://file.ran-world.com/sns/ko/fb_logout_next.htm";
	std::string	strSNS_LOGIN_CANCEL_URL		= "http://file.ran-world.com/sns/ko/fb_login_cancel.htm";

	std::string	strSNS_IMAGE_LOGO			= "logo.gif";
	std::string	strSNS_IMAGE_WELCOME		= "welcome.gif";
	std::string	strSNS_IMAGE_LEVELUP		= "levelup.gif";
	std::string	strSNS_IMAGE_ENCHANTED		= "enchanted.gif";

	std::string  strSNS_FB_APP_ID;
	std::string  strSNS_FB_API_KEY;
	std::string  strSNS_FB_APP_SECRET;

	std::string  strSNS_TW_CONSUMER_KEY;
	std::string  strSNS_TW_CONSUMER_SECRET;

	DWORD		dw_REGISTER_LEVEL_UP		= 1;
	DWORD		dw_REGISTER_GRINDING_LEVEL	= 7;

	BOOL LOADFILE(const std::string& FileName, bool bToolMode, BOOL bServer)
	{
		if( strlen(FileName.c_str()) == 0 )	return FALSE;

		//	Note : 
		std::string strPath;
		strPath = GLOGIC::GetPath();
		strPath += FileName;

		CIniLoader cFILE(",", bToolMode);

		if( GLOGIC::bGLOGIC_ZIPFILE )
			cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

		if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
		{
            std::string ErrorMsg(
                sc::string::format(
                    "GLCONST_SNS::LOADFILE(), File Open %1%", FileName));
			sc::writeLogError(ErrorMsg);
			return false;
		}

		int nFlag=0;

        cFILE.getflag( "sns", "bANIMATION_FLAG", 0, 1, nFlag );
        bANIMATION_FLAG = nFlag ? true : false;

		cFILE.getflag( "url", "strSNS_FB_HREF",					0, 1, strSNS_FB_HREF );
		cFILE.getflag( "url", "strSNS_IMAGE_URL",				0, 1, strSNS_IMAGE_URL );
		cFILE.getflag( "url", "strSNS_ACTION_LINK",				0, 1, strSNS_ACTION_LINK );
		cFILE.getflag( "url", "strSNS_LOGOUT_NEXT_URL",			0, 1, strSNS_LOGOUT_NEXT_URL );
		cFILE.getflag( "url", "strSNS_LOGIN_CANCEL_URL",		0, 1, strSNS_LOGIN_CANCEL_URL );

		cFILE.getflag( "path", "strSNS_IMAGE_LOGO",				0, 1, strSNS_IMAGE_LOGO );
		cFILE.getflag( "path", "strSNS_IMAGE_WELCOME",			0, 1, strSNS_IMAGE_WELCOME );
		cFILE.getflag( "path", "strSNS_IMAGE_LEVELUP",			0, 1, strSNS_IMAGE_LEVELUP );
		cFILE.getflag( "path", "strSNS_IMAGE_ENCHANTED",		0, 1, strSNS_IMAGE_ENCHANTED );

		cFILE.getflag( "key", "strSNS_FB_APP_ID",				0, 1, strSNS_FB_APP_ID );
		cFILE.getflag( "key", "strSNS_FB_API_KEY",				0, 1, strSNS_FB_API_KEY );
		cFILE.getflag( "key", "strSNS_FB_APP_SECRET",			0, 1, strSNS_FB_APP_SECRET );

		cFILE.getflag( "key", "strSNS_TW_CONSUMER_KEY",			0, 1, strSNS_TW_CONSUMER_KEY );
		cFILE.getflag( "key", "strSNS_TW_CONSUMER_SECRET",		0, 1, strSNS_TW_CONSUMER_SECRET );

		cFILE.getflag( "const", "dw_REGISTER_LEVEL_UP",			0, 1, dw_REGISTER_LEVEL_UP );
		cFILE.getflag( "const", "dw_REGISTER_GRINDING_LEVEL",	0, 1, dw_REGISTER_GRINDING_LEVEL );

		//-------------------------------------------------------------------------------------------------//

		return TRUE;
	}
};
