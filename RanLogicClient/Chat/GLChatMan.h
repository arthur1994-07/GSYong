#pragma once

#include "./GLChatDef.h"
#include "../../RanLogic/GLChatLinkDef.h"
#include "../Widget/GLWidgetScript.h"

class GLChat
{
public :
	GLChat( int nLogID, DWORD dwType, const std::string& strMsg, DWORD dwColor )
		: m_nLogID( nLogID )
		, m_dwType( dwType )
		, m_strMsg( strMsg )
		, m_dwColor( dwColor )
	{
	}
	~GLChat()
	{
		m_vecLinkTextData.clear();
	}

public :
	int					m_nLogID;
	DWORD				m_dwType;
	std::string			m_strMsg;
	DWORD				m_dwColor;

	GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR	m_vecCustomColor;
	VEC_LINK_TEXT_DATA					m_vecLinkTextData;
	GLCHAT_DEF::SCHAT_REFERCHAR_NAME	m_sReferName;
};
typedef std::list< GLChat >		LIST_CHAT;
typedef LIST_CHAT::iterator		LIST_CHAT_ITER;

class GLChatMan
{
private :
	GLChatMan(void);
	GLChatMan( const GLChatMan& value );
	~GLChatMan(void);

public :
	static GLChatMan& GetInstance();

private :
	// ä�� ������ ����Ʈ
	LIST_CHAT m_listChat;

	// ��ũ ������ ������ ����Ʈ
	MAP_LINK_DATA_INDEX			m_mapLinkData;	
	MAP_ITEM_LINK_DATA_INDEX	m_mapItemLinkData;
	MAP_PARTY_LINK_DATA_INDEX	m_mapPartyLinkData;

	// ��ũ ������ �ε���
	int	m_dwLinkDataIndex;
	int	m_dwItemLinkDataIndex;
	int	m_dwPartyLinkDataIndex;

	bool	m_bChatLog;			// ä�� �α� ���?
	DWORD	m_dwChatLogType;	// ä�� �α� Ÿ��

	bool	m_bPrisonBlock;
	bool	m_bGMBlock;					// GM ä�� ���
	bool	m_bPaperingBlock;			// ���� ä�� ���
	int		m_nPaperingCount;			// ���� ä�� ����
	float	m_fPaperingBlockStartTime;	// ���� ä�� ��� ���� �ð�
	CString	m_strLastChat;				// ������ ä�� �޽���

public :
	void MsgProc( LPVOID msgBuffer );

	void MsgChatLinkMessage( LPVOID msgBuffer );
	void MsgSystemItemLinkMessage( LPVOID msgBuffer );

    //! Ȯ��/������ ����Ʈ ���� ��� : sckim, Redmine #4544
    void MsgSystemItemAndReferLinkMessage( LPVOID msgBuffer );

	void MsgChatGlobalMessage( LPVOID msgBuffer );
	void MsgChatPrivateFail( LPVOID msgBuffer );
	void MsgChatBlock( LPVOID msgBuffer );

	// ������ ��ũ �޼���
	void MsgChatItemLinkMessage( const CString& str, SITEMCUSTOM& sItemCustom );

private :
	void SaveChatLog( const CString& strText, DWORD dwType );

public :
	void ChatLog( bool bChatLog, DWORD dwChatLogType );

public :
	// ä�� ��ũ �߰�
	void AddChatLink( const SLINK_DATA_BASIC& sLinkDataBasic );
	// ä�� �޽��� ����, ��ƿ� �˷���.
	void OnNotifyReceiveMsg( DWORD dwType, const char* strName, const char* strMsg, 
		bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData = NULL );

	// �ý��� ������ ��ũ �޼��� ��ƿ� �˷��� 
	void OnSystemItemLinkMsg( DWORD dwType, const char* strName, const char* strMsg, 
		bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData = NULL );

    //! Ȯ��/������ ����Ʈ ���� ��� : sckim, Redmine #4544
	// �ý��� ������, Refer ��ũ �޼��� ��ƿ� �˷��� 
	void OnSystemItemAndReferLinkMsg( DWORD dwType, const char* strName, const char* strMsg, 
		bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData = NULL );

	// ä�� ��ũ Ŭ��, ��ƿ� �˷���.
	void OnNotifyClickLink( int nLinkIndex );

	// ��ũ ������ ���
	bool GetLinkData( const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor );
	// ��ũ �÷� ���
	DWORD GetLinkColor( int nLinkIndex );
	// ��ũ ������ �ε��� 
	SLINK_DATA_INDEX* GetLinkBasicData( int nLinkIndex );

public :
	// ���� ä�� Ÿ�� -> Ŭ���̾�Ʈ ä�� Ÿ��
	DWORD ConvertChatType( int nType, bool& bGM );
	// Ŭ���̾�Ʈ ��ũ ������ -> ���� ��ũ ������
	void ConvertLinkData( const VEC_LINK_TEXT_DATA_EX& vecLinkDataIN, VEC_LINK_TEXT_DATA& vecLinkDataOut );

	// ��ũ ������ ���
	int AddLinkDataToList_Client( const SLINK_DATA_BASIC& sLinkDataBasic, const SITEMCUSTOM* pItemCustom );
	int AddLinkDataToList( const std::string& strCharName, const SLINK_TYPE& sLinkType,
						   const SLINK_DATA_POS& sLinkPos, void* pBuffer, DWORD dwSize );
	int	AddLinkDataToTypeList( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize );

	// ��ũ ������ ����
	void DeleteLinkData( const SLINK_DATA_INDEX& sLinkIndex );

public :
	// ä�� �޽��� �߰�
	void AddChatMessage( int nLogID, DWORD dwType, const std::string& strMsg, DWORD dwColor, 
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName );
	// ��ũ �޽��� �߰�
	void AddLinkMessage( int nLogID, DWORD dwType, const std::string& strMsg, DWORD dwColor,
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		const VEC_LINK_TEXT_DATA& vecLinkIndex,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName );

	// ä�� �޽��� Ŭ����
	void ClearMessage( int nLogID );

	// ä�� ����Ʈ ���
	void GetChatList( int nLogID, LIST_CHAT& listChat );
	void GetChatList( int nLogID, DWORD dwType, LIST_CHAT& listChat );

public :
	// ä�� ��ũ�� �ؽ�Ʈ
	void SetChatMacro( WORD wIndex, const char* strText );
	std::string GetChatMacro( WORD wIndex );

	// ��Ƽ ��ũ ������
	LuaTable CreatePartyLink();
	// ��Ƽ ��ũ ���� ��û �Ϸ� ����
	void SetPartyLinkOnce( int nLinkIndex );

	// ä�� ��� Ȯ��
	bool CheckBlock( const CString& strMsg = "" );
	// GM ä�� ��� ����
	void SetGMChatBlock( bool bBlock ) { m_bGMBlock = bBlock; }
	// ä�� ���
	LuaTable GetChatBlock();
};
