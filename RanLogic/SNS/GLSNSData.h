// bjju.sns

#pragma once

#include <string>

class GLSNS
{
public:
	GLSNS ();
	virtual ~GLSNS();

};

namespace COMMENT
{

};

namespace GLCONST_SNS
{
	extern bool			bANIMATION_FLAG;

	extern std::string	strSNS_FB_HREF;
	extern std::string	strSNS_IMAGE_URL;
	extern std::string	strSNS_ACTION_LINK;
	extern std::string	strSNS_LOGOUT_NEXT_URL;
	extern std::string	strSNS_LOGIN_CANCEL_URL;

	extern std::string	strSNS_IMAGE_LOGO;
	extern std::string	strSNS_IMAGE_WELCOME;
	extern std::string	strSNS_IMAGE_LEVELUP;
	extern std::string	strSNS_IMAGE_ENCHANTED;

	extern std::string  strSNS_FB_APP_ID;
	extern std::string  strSNS_FB_API_KEY;
	extern std::string  strSNS_FB_APP_SECRET;

	extern std::string  strSNS_TW_CONSUMER_KEY;
	extern std::string  strSNS_TW_CONSUMER_SECRET;

	extern DWORD		dw_REGISTER_LEVEL_UP;
	extern DWORD		dw_REGISTER_GRINDING_LEVEL;  // �ּ� ���� X�ܰ� �̻���� SNS�� ���� ��ϵ˴ϴ�.

    BOOL LOADFILE(const std::string& FileName, bool bToolMode, BOOL bServer/*=TRUE*/);
};