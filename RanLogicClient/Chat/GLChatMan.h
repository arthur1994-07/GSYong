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
	// 채팅 보관용 리스트
	LIST_CHAT m_listChat;

	// 링크 데이터 보관용 리스트
	MAP_LINK_DATA_INDEX			m_mapLinkData;	
	MAP_ITEM_LINK_DATA_INDEX	m_mapItemLinkData;
	MAP_PARTY_LINK_DATA_INDEX	m_mapPartyLinkData;

	// 링크 데이터 인덱스
	int	m_dwLinkDataIndex;
	int	m_dwItemLinkDataIndex;
	int	m_dwPartyLinkDataIndex;

	bool	m_bChatLog;			// 채팅 로그 사용?
	DWORD	m_dwChatLogType;	// 채팅 로그 타입

	bool	m_bPrisonBlock;
	bool	m_bGMBlock;					// GM 채팅 블락
	bool	m_bPaperingBlock;			// 도매 채팅 블락
	int		m_nPaperingCount;			// 연속 채팅 개수
	float	m_fPaperingBlockStartTime;	// 도배 채팅 블락 시작 시간
	CString	m_strLastChat;				// 마지막 채팅 메시지

public :
	void MsgProc( LPVOID msgBuffer );

	void MsgChatLinkMessage( LPVOID msgBuffer );
	void MsgSystemItemLinkMessage( LPVOID msgBuffer );

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    void MsgSystemItemAndReferLinkMessage( LPVOID msgBuffer );

	void MsgChatGlobalMessage( LPVOID msgBuffer );
	void MsgChatPrivateFail( LPVOID msgBuffer );
	void MsgChatBlock( LPVOID msgBuffer );

	// 아이템 링크 메세지
	void MsgChatItemLinkMessage( const CString& str, SITEMCUSTOM& sItemCustom );

private :
	void SaveChatLog( const CString& strText, DWORD dwType );

public :
	void ChatLog( bool bChatLog, DWORD dwChatLogType );

public :
	// 채팅 링크 추가
	void AddChatLink( const SLINK_DATA_BASIC& sLinkDataBasic );
	// 채팅 메시지 수신, 루아에 알려줌.
	void OnNotifyReceiveMsg( DWORD dwType, const char* strName, const char* strMsg, 
		bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData = NULL );

	// 시스템 아이템 링크 메세지 루아에 알려줌 
	void OnSystemItemLinkMsg( DWORD dwType, const char* strName, const char* strMsg, 
		bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData = NULL );

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
	// 시스템 아이템, Refer 링크 메세지 루아에 알려줌 
	void OnSystemItemAndReferLinkMsg( DWORD dwType, const char* strName, const char* strMsg, 
		bool bSelf, bool bGM, const VEC_LINK_TEXT_DATA* pvecLinkTextData = NULL );

	// 채팅 링크 클릭, 루아에 알려줌.
	void OnNotifyClickLink( int nLinkIndex );

	// 링크 데이터 얻기
	bool GetLinkData( const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor );
	// 링크 컬러 얻기
	DWORD GetLinkColor( int nLinkIndex );
	// 링크 데이터 인덱스 
	SLINK_DATA_INDEX* GetLinkBasicData( int nLinkIndex );

public :
	// 서버 채팅 타입 -> 클라이언트 채팅 타입
	DWORD ConvertChatType( int nType, bool& bGM );
	// 클라이언트 링크 데이터 -> 서버 링크 데이터
	void ConvertLinkData( const VEC_LINK_TEXT_DATA_EX& vecLinkDataIN, VEC_LINK_TEXT_DATA& vecLinkDataOut );

	// 링크 데이터 등록
	int AddLinkDataToList_Client( const SLINK_DATA_BASIC& sLinkDataBasic, const SITEMCUSTOM* pItemCustom );
	int AddLinkDataToList( const std::string& strCharName, const SLINK_TYPE& sLinkType,
						   const SLINK_DATA_POS& sLinkPos, void* pBuffer, DWORD dwSize );
	int	AddLinkDataToTypeList( const std::string& strCharName, const SLINK_TYPE& sLinkType, void* pBuffer, DWORD dwSize );

	// 링크 데이터 삭제
	void DeleteLinkData( const SLINK_DATA_INDEX& sLinkIndex );

public :
	// 채팅 메시지 추가
	void AddChatMessage( int nLogID, DWORD dwType, const std::string& strMsg, DWORD dwColor, 
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName );
	// 링크 메시지 추가
	void AddLinkMessage( int nLogID, DWORD dwType, const std::string& strMsg, DWORD dwColor,
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		const VEC_LINK_TEXT_DATA& vecLinkIndex,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName );

	// 채팅 메시지 클리어
	void ClearMessage( int nLogID );

	// 채팅 리스트 얻기
	void GetChatList( int nLogID, LIST_CHAT& listChat );
	void GetChatList( int nLogID, DWORD dwType, LIST_CHAT& listChat );

public :
	// 채팅 매크로 텍스트
	void SetChatMacro( WORD wIndex, const char* strText );
	std::string GetChatMacro( WORD wIndex );

	// 파티 링크 데이터
	LuaTable CreatePartyLink();
	// 파티 링크 가입 요청 완료 설정
	void SetPartyLinkOnce( int nLinkIndex );

	// 채팅 블락 확인
	bool CheckBlock( const CString& strMsg = "" );
	// GM 채팅 블락 설정
	void SetGMChatBlock( bool bBlock ) { m_bGMBlock = bBlock; }
	// 채팅 블락
	LuaTable GetChatBlock();
};
