//mjeon.groupchat

#pragma once

enum
{
	HOW_MANY_CHAT_MEMBERS_PER_ONE_SYNC = 25,	//40 * 25 = 1000 bytes
	HOW_MANY_ITEM_LINK_PER_CHAT_MSG = 2,			//�� 100 bytes * 3 = 300 // ������ ���� �������� �켱 ���2�� ������ ���� ����ũ�� �����ϸ鼭 �ٽ� 3���� �ø� ����
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