//mjeon.groupchat

#pragma once

enum
{
	HOW_MANY_CHAT_MEMBERS_PER_ONE_SYNC = 25,	//40 * 25 = 1000 bytes
	HOW_MANY_ITEM_LINK_PER_CHAT_MSG = 2,			//약 100 bytes * 3 = 300 // 아이템 구조 변경으로 우선 잠시2로 변경함 기존 메이크넘 삭제하면서 다시 3으로 늘릴 예정
	MAX_LENGTH_OF_CHAT_MESSAGE = 512,
};

struct CHAT_MEMBER
{
	CHAT_MEMBER(DWORD _dwChaNum, std::string _strName)
		:dwChaNum(_dwChaNum)
	{
		StringCchCopy(szChaName, sizeof(szChaName), _strName.c_str());
	}

	CHAT_MEMBER(DWORD _dwChaNum, char *_szName)
		:dwChaNum(_dwChaNum)
	{
		StringCchCopy(szChaName, sizeof(szChaName), _szName);
	}

	CHAT_MEMBER()
		:dwChaNum(INVALID_CHANUM)
	{
		ZeroMemory(szChaName, sizeof(szChaName));
	}

	DWORD				dwChaNum;
	char				szChaName[CHR_ID_LENGTH];
};