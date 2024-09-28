#pragma once

#include "../../RanGfxUI/GfxDef.h"
#include "../../RanLogic/GLChatLinkDef.h"

#include "./Chat/GLChatMan.h"
#include "./Crow/GLCrowRenList.h"
#include "./Widget/GLWidgetScript.h"

// 스테이지 배경 (수정시 WidgetDef.lua 파일도 같이 수정)
enum EMSTAGE_BG
{
	EMSTAGE_BG_NONE = -1,

	EMSTAGE_BG_LOGIN = 0,
	EMSTAGE_BG_SELECT_CHAR,
	EMSTAGE_BG_CREATE_CHAR,
	EMSTAGE_BG_GAME,
};

class GLGaeaClient;
class GfxSlotBase;


class SF_PROFILING
{
public:
	typedef std::map<std::string, DWORD> SFMAP_KEYFUNC_DATACNT;
	typedef std::map<DWORD, SFMAP_KEYFUNC_DATACNT> SFMAP_KEYUI_DATAFUNCCNT;
	typedef SFMAP_KEYFUNC_DATACNT::iterator SFMAP_KEYFUNC_DATACNT_ITR;
	typedef SFMAP_KEYUI_DATAFUNCCNT::iterator SFMAP_KEYUI_DATAFUNCCNT_ITR;
	typedef SFMAP_KEYFUNC_DATACNT::value_type SFMAP_KEYFUNC_DATACNT_VT;
	typedef SFMAP_KEYUI_DATAFUNCCNT::value_type SFMAP_KEYUI_DATAFUNCCNT_VT;

public:
	SFMAP_KEYUI_DATAFUNCCNT m_mapProfiling;

public:
	SF_PROFILING(void) {}
	~SF_PROFILING(void){}

public:
	void record(DWORD dwUINum, std::string& strFunction)
	{
		SFMAP_KEYUI_DATAFUNCCNT_ITR mapData_itr = m_mapProfiling.find(dwUINum);
		if (mapData_itr == m_mapProfiling.end())
		{
			SFMAP_KEYFUNC_DATACNT mapData_vt;
			m_mapProfiling.insert( std::make_pair(dwUINum, mapData_vt) );
		}

		SFMAP_KEYFUNC_DATACNT& mapFuncCnt = m_mapProfiling[dwUINum];
		SFMAP_KEYFUNC_DATACNT_ITR mapFuncCnt_itr = mapFuncCnt.find(strFunction);		
		
		
		if ( mapFuncCnt_itr == mapFuncCnt.end() )
			mapFuncCnt[strFunction] = 1;
		else
			mapFuncCnt[strFunction] = mapFuncCnt[strFunction] + 1;
	}
};

class GfxInterfaceBase
{
protected :
	GfxInterfaceBase(void);
	GfxInterfaceBase( const GfxInterfaceBase& value );
	virtual ~GfxInterfaceBase(void);

public :
	static GfxInterfaceBase* GetInstance();

protected :
	HWND m_hWnd;
	GLGaeaClient* m_pGaeaClient;

	// 폰트 언어
	EMGFX_FONT_LANG m_emFontLang;

	int m_nStageBG;
	int m_nPrevStageBG;

	bool m_bMouseHitUI;			// 마우스 입력이 UI에 히트 했는지 확인
	bool m_bIsDragging;			// 마우스 드래그 중인지 확인
	bool m_bIsFocusInTextInput;	// 텍스트 인풋에 포커스 들어가있나?

	bool m_bDisplayOn;
	bool m_bDisplayCountryMark;	// 이름 디스플레이에 국가 이미지 출력?

	bool m_bCallProfiling;
	SF_PROFILING m_sfProfiling;

public:
	void SetProfiling(bool bSet);
	void SaveProfile(void);
	bool isProfiling(){return m_bCallProfiling;}
	SF_PROFILING& getProfilingData() { return m_sfProfiling; }
	void ProfilingRecord(INT nUINum, std::string& strLog) { m_sfProfiling.record(nUINum, strLog); }

	std::wstring EncodeText( const std::string& str );

public :
	virtual void OnCreateDevice( HWND hWnd, LPDIRECT3DDEVICE9 pDevice ) = 0;
	virtual void OnResetDevice( LPDIRECT3DDEVICE9 pDevice ) = 0;
	virtual void OnLostDevice() = 0;
	virtual void OnDestroyDevice() = 0;
	virtual void OnFrameMove( float fElapsedTime ) = 0;
	virtual void OnFrameRender( float fElapsedTime ) = 0;

	virtual void InitOldInterface() = 0;

	virtual void SetLoadingData( const SNATIVEID& sMapID, const std::string& strLoadingImgFile, bool bShowGrade ) = 0;
	virtual void OnInitGame() = 0;
	virtual void OnInitLoading() = 0;
	virtual void OnBeginLoading( ELoadingSceneType eType ) = 0;
	virtual void OnEndLoading() = 0;

public:
	// 게임 스테이지로 접속시 초기화 시킬것들 초기화시킨다;
	virtual void InitGameStage(void) = 0;

public :
	// Debug Scene Toggle
	virtual void ToggleDebugScene() = 0;
	// D3DPRESENT_PARAMETERS 저장
	virtual void StorePresentParameters( D3DPRESENT_PARAMETERS pp ) = 0;
	// 미리 처리할 윈도우 메시지 처리
	virtual void PreProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) = 0;
	// 윈도우 메시지 처리, true 반환시 윈도우 기본 프록시저 스킵
	virtual bool ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) = 0;

public :
	void SetGaeaClient( GLGaeaClient* pClient );
	void SetStageBG( int nStageBG ) { m_nPrevStageBG = m_nStageBG; m_nStageBG = nStageBG; }
	int GetStageBG() { return m_nStageBG; }
	int GetPrevStageBG() { return m_nPrevStageBG; }

	bool IsMouseHitUI() { return m_bMouseHitUI; }
	bool IsDragging() { return m_bIsDragging; }
	virtual bool IsMouseInUI() = 0;

	void SetDisplayOn( bool bValue ) { m_bDisplayOn = bValue; }
	bool GetDisplayOn() { return m_bDisplayOn; }

	virtual void SetDisplayCountryMark( bool bValue ) { m_bDisplayCountryMark = bValue; }
	bool GetDisplayCountryMark() { return m_bDisplayCountryMark; }

	// 현재 UI 포커스가 텍스트 인풋에 있는가?
	void SetFocusInTextInput( bool bValue ) { m_bIsFocusInTextInput = bValue; }
	bool GetFocusInTextInput() { return m_bIsFocusInTextInput; }

	GLGaeaClient* GetGaeaClient();

	HWND GetHWND() { return m_hWnd; }

	// 폰트 언어 설정
	virtual void SetFontLanguage( EMGFX_FONT_LANG emFontLang ) = 0;
	EMGFX_FONT_LANG GetFontLanguage() { return m_emFontLang; }
	virtual const D3DPRESENT_PARAMETERS& GetPresentParams() = 0;

	virtual void GetMousePos( int& nMouseX, int& nMouseY ) = 0;

	// 키보드 입력 영문으로 변경
	virtual void SetConversionModeEn() = 0;

	// 안티앨리어싱 on/off
	virtual void SetEdgeAAMode( int nMode ) = 0;

	// 드랍 아이템 이름 클릭으로 줍기 설정
	virtual void SetInteractDropItem( bool bEnable ) = 0;

	virtual std::string GetIMECompositionString(void) = 0;

public :
	virtual void Reload() = 0;

public :
	// UI 프리로드
	virtual void PreloadWidget() = 0;

	// UI 언로드
	virtual void UnloadWidget( int nWidgetID, int nSubID ) = 0;

	// 슬롯 ID 할당
	virtual void RegSlot( WORD wType, const char* strSlotID ) = 0;
	// 슬롯 ID 제거
	virtual void UnregSlot( const std::string& strSlotID ) = 0;
	// HTML View ID 할당
	virtual void RegHtmlWebView( int nViewID ) = 0;
	// HTML View ID
	virtual int GetHtmlWebID( int nViewID ) = 0;
	// HTML View 이동
	virtual void MoveHtmlWebView( int nViewID, bool bVisible, int nX, int nY, int nWidth, int nHeight ) = 0;
	
	// UI 열기
	virtual void OpenWidget( int nWidgetID, int nSubID = 0, bool bPreload = false ) = 0;
	// UI 닫기
	virtual void CloseWidget( int nWidgetID, int nSubID = 0 ) = 0;
	// 전체 UI 닫기
	virtual void CloseAllWidget( bool bDelete ) = 0;
	// UI 열려있나?
	virtual bool IsOpenWidget( int nWidgetID, int nSubID = 0 ) = 0;
	// UI 로드된 상태인가?
	virtual bool IsLoadWidget( int nWidgetID, int nSubID = 0 ) = 0;
	// 멀티 widget 열린 개수
	virtual int GetMultiWidgetCount( int nWidgetID ) = 0;
	// ESC 키로 닫을 수 있는 UI 하나씩 닫기, 닫을게 없으면 false
	virtual bool EscWidgetClose() = 0;
	// 툴팁 열려있나?
	virtual bool IsOpenTooltip() = 0;

	// 스테이지 이벤트 설정
	virtual void SetStageEvent( const char* strEvent, const char* strCallBack, const char* strRetParams ) = 0;

	// 프로퍼티 설정
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int nValue ) = 0;
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int nValue ) = 0;
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double dValue ) = 0;
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool bValue ) = 0;
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue ) = 0;
		virtual void SetProperty_WString( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue ) = 0;
	// virtual void SetProperty_Array( int nWidgetID, int nSubID, const char* strProp, const char* strInstanceArray, const char* strValueArray ) = 0;
	virtual void SetProperty_Array( int nWidgetID, int nSubID, LuaTable tbProp, LuaTable tbInstance, LuaTable tbValue, LuaTable tbValueType) = 0;

	// 프로퍼티 얻기
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int& nValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int& nValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double& dValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool& bValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, std::string& strValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, LuaTable& tb ) = 0;

	// 이벤트 심기
	virtual void SetEvent( int nWidgetID, int nSubID, const char* strEvent, const char* strCallBack, 
						   const char* strRetProp, const char* strDelivery, const char* strInstance ) = 0;

	// 데이터 프로바이더 설정
	virtual void SetDataProvider( int nWidgetID, int nSubID, const char* strDataProvider, 
								  const char* strInstance, bool bAdd ) = 0;
	virtual void SetDataProviderEx( int nWidgetID, int nSubID, const char* strObjectID, const char* strDataProvider,
									const char* strInstance, bool bAdd ) = 0;
	virtual void SetDataProviderComplex( int nWidgetID, int nSubID, const char* strObjectID, LuaTable tbData,
									const char* strInstance, bool bAdd ) = 0;
	virtual void SetDataProvider_Terms(int nWidgetID, int nSubID
		, const char* strInstName
		, LuaTable tbWhereProvName, LuaTable tbWhereProvData, LuaTable tbWhereProvType
		, LuaTable tbNewProvName, LuaTable tbNewProvData, LuaTable tbNewProvType) = 0;

	// 데이터 프로바이더 제거
	virtual void RemoveDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strInstance, bool bAll ) = 0;

	// 데이터 프로바이더 얻기
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int& nValue ) = 0;
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int& nValue ) = 0;
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double& dValue ) = 0;
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool& bValue ) = 0;
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, std::string& strValue ) = 0;
	virtual bool GetDataProvider_Array( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, LuaTable& tbResult ) = 0;
	
	// 데이터 프로바이더 프로퍼티 설정
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int nValue ) = 0;
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int nValue ) = 0;
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double dValue ) = 0;
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool bValue ) = 0;
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, const char* strValue ) = 0;

	virtual void SetListItemProp_Array( int nWidgetID, int nSubID, char* strIndexArr, const char* strInstNameArr, const char* strPropArr,
		const char* strValueArr, const char* strTypeArr, const char* strInstance ) = 0;
	// 리스트 아이템 프로퍼티 설정
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp,
		int nValue, const char* strInstance ) = 0;
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp,
		unsigned int nValue, const char* strInstance ) = 0;
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp, 
		double dValue, const char* strInstance ) = 0;
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp, 
		bool bValue, const char* strInstance ) = 0;
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp, 
		const char* strValue, const char* strInstance ) = 0;

	// 리스트 아이템 함수 호출
	virtual void CallListItemFunc( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strFunc,
		const char* strParams, const char* strInstance ) = 0;

	// 함수 호출
	virtual void CallFunction( int nWidgetID, int nSubID, const char* strName, const char* strParams, const char* strInstance ) = 0;
	// 포커스 설정
	virtual void SetFocus( int nWidgetID, int nSubID, const char* strInstance ) = 0;
	// 텍스트 컬러 설정
	virtual void SetTextFormat( int nWidgetID, int nSubID, DWORD dwColor, bool bUnderline,
		int nBeginIndex, int nEndIndex, const char* strInstance ) = 0;
	// 텍스트 컬러 설정
	virtual void SetTextFormatSize( int nWidgetID, int nSubID, DWORD dwSize, const char* strInstance ) = 0;
	// 트윈 설정
	virtual void SetTween( int nWidgetID, int nSubID, int nTime, int nDelay, int nEase, bool bLoop, const char* strComplete,
		const char* strProp, const char* strValue, const char* strInstance ) = 0;

	virtual void RemoveTween( int nWidgetID, int nSubID, const char* strInstance ) = 0;
	
	// 마우스 히트 테스트 설정
	virtual void HitTestEnable( int nWidgetID, int nSubID, bool bEnable, const char* strInstance ) = 0;

	// 슬롯 툴팁 위치/사이즈
	virtual void GetSlotTooltipPosSize( int& nX, int& nY, int& nWidth, int& nHeight ) = 0;

	// UI 위치 초기화
	virtual void ResetWidgetPosition() = 0;

	// 말풍선 추가
	virtual void SetSpeechBubble(int nWidgetID, int nSubID, const char* strInstance, const char* strText) = 0;
	// 말풍선 삭제
	virtual void DelSpeechBubble(int nWidgetID, int nSubID, const char* strInstance) = 0;
	// 페이크 모달 설정
	virtual void SetFakeModal( bool bValue ) = 0;
	// 스냅 되는 좌표
	virtual void SnapCheck( int nWidgetID, int nSubID, int nX, int nY, int nWidth, int nHeight, int& nRetX, int& nRetY ) = 0;
	// 전체 UI 알파값
	virtual void SetUIAlpha( float fAlpha ) = 0;
	// 클럽 마크 업데이트
	virtual void UpdateClubMark( int nWidgetID, int nSubID, const char* szParam, const char* szInst ) = 0;
	// 홀드 슬롯 업데이트
	virtual void UpdateHoldSlot() = 0;
	// UI 순서 맨앞으로
	virtual void SetFoward( int nWidgetID, int nSubID ) = 0;

	// 트리 헤드 데이터
	virtual void SetHeadData( int nWidgetID, int nSubID, const char* strObjectID,
		LuaTable tbData, const char* strInstance ) = 0;
	virtual void GetHeadData( int nWidgetID, int nIndex, const char* szInst, int nSubID, LuaTable& tb ) = 0;
	virtual void ChangeHeadData( int nWidgetID, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID ) = 0;

	// 트리 데이터
	virtual void SetTreeData( int nWidgetID, int nSubID, LuaTable tbKey, const char* strObjectID, 
		LuaTable tbData, const char* strInstance ) = 0;
	virtual void GetTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szInst, int nSubID, LuaTable& tb ) = 0;
	virtual void ChangeTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID ) = 0;

public :
	// 채팅 메시지 추가
	virtual void AddChatMessage( int nLogID, const std::string& strMsg, DWORD dwColor, 
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName ) = 0;
	// 링크 메시지 추가
	virtual void AddLinkMessage( int nLogID, const std::string& strMsg, DWORD dwColor,
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		VEC_LINK_TEXT_DATA& vecLinkTextData,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName ) = 0;

public :
	// 슬롯 드래그 스탑
	virtual void SlotDragStop() = 0;

	// 슬롯 아이템
	virtual bool GetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem ) = 0;
	virtual void SetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem ) = 0;

	// 슬롯 데이터 리셋
	virtual void ResetSlot( const char* strSlotID ) = 0;
	// 아이템 슬롯 데이터 설정
	virtual void SetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID ) = 0;
	virtual void SetItemSlotData_NonStrengthen( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID ) = 0;
	virtual void SetItemSlotData_BORNTIME( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, DWORD dwBornT ) = 0;
	// 스킬 슬롯 데이터 설정
	virtual void SetSkillSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nLevel = -1 ) = 0;
	// 인벤토리 슬롯 데이터 설정
	virtual void SetInventorySlotData( const char* strSlotID, int nPosX, int nPosY ) = 0;
	// 재 구입 슬롯 데이터 설정
	virtual void SetRebuySlotData( const char* strSlotID, int nRebuyID, WORD wMID, WORD wSID ) = 0;
	// 소환수 데이터 설정;
	virtual void SetSummonStateSlotData( const char* strSlotID, const char* strIconName, INT nIconIdxX, INT nIconIdxY ) = 0;
	// 은행 슬롯 데이터 설정
	virtual void SetBankSlotData( const char* strSlotID, int nPosX, int nPosY ) = 0;
	// 위시리스트 슬롯 데이터 설정
	virtual void SetWishlistSlotData( const char* strSlotID, int nPosX, int nPosY ) = 0;
	// 캐릭터 정보 슬롯 데이터 설정
	virtual void SetCharInfoSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, int nSlot ) = 0;
	// 스킬 퀵슬롯 설정
	virtual void SetSkillQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex ) = 0;
	// 아이템 퀵슬롯 설정
	virtual void SetItemQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex ) = 0;
	// 미니게임 : 주사위 보상 슬롯 설정;
	virtual void SetItemMiniGameDiceRewardSlotData( const char* strSlotID, DWORD dwID, int nIndex ) = 0;
	// NPC 대화 마켓 슬롯 설정
	virtual void SetNpcMarketSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY ) = 0;
	// NPC 대화 마켓 카트 슬롯 설정
	virtual void SetNpcMarketCartSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, WORD wIndex ) = 0;
	// 거래창 슬롯 설정
	virtual void SetTradeSlotData(const char* strSlotID, int nPosX, int nPosY, bool _MyBox) = 0;
	// PointShop 슬롯 설정
	virtual void SetPointShopSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY ) = 0;
	// PointShop 카트 슬롯 설정
	virtual void SetPointShopCartSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nIndex ) = 0;
	// 정보보기창 슬롯 데이터 설정
	virtual void SetReferSlotData( const char* strSlotID, int nSlot ) = 0;
	// 개인상점검색 포인트샵 아이템 등록 슬롯
	virtual void SetPMSPointTradeSlotData( const char* strSlotID, WORD wPosX, WORD wPosY ) = 0;
	// 개인상점검색 포인트샵 인덱스 슬롯
	virtual void SetPMSPointTradeSlotData_Index( const char* strSlotID, DWORD dwIndex, DWORD dwCount ) = 0;
	// 개인상점 슬롯
	virtual void SetPrivateMarketSlotData( const char* strSlotID, int nPosX, int nPosY, bool bSelf ) = 0;
	// 개인상점검색 슬롯
	virtual void SetPrivateMarketSearchSlotData( const char* strSlotID, DWORD dwIndex, WORD wCount, bool bSearchPage ) = 0;
	// 우편 슬롯
	virtual void SetReceivePostSlotData( const char* strSlotID, DWORD dwIndex, DWORD dwCount ) = 0;	
	// PostBoxSub_PreviewPageOpen
	virtual void SetSlot_PostBoxSub_PreviewPageOpen( const char* strSlotID ) = 0;
	// 펫 아이템 장착 슬롯 설정
	virtual void SetPetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD nSlot ) = 0;
	// 클럽창고 창 슬롯 설정
	virtual void SetClubStorageSlotData( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY) = 0;
	virtual void SetClubStorageItem(const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY) = 0;
	// 락커 슬롯 설정
	virtual void SetLockerSlot( const char* strSlotID, int nIndex, int nPosX, int nPosY ) = 0;
	// 개조 카드 슬롯 설정
	virtual void SetRebuildCardSlot( const char* strSlotID, int nPosX, int nPosY ) = 0;
	// 파티 분배 슬롯 설정;
	virtual void SetItemSlotData_PartyDistribution( const char* strSlotID, DWORD dwItemID, WORD wSlotIndex) = 0;
	// 아이템 수리 슬롯 설정
	virtual void SetItemRepairSlotData( const char* strSlotID, int nSlot ) = 0;
	// 로또 당첨 아이템 보기 슬롯 설정;
	virtual void SetLottoWinItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID ) = 0;
	// 로또 이전 당첨자 리스트 당첨 아이템 목록 슬롯 설정;
	virtual void SetLottoPreWinListItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID, int nRanking ) = 0;
	// 란 모바일 보상 슬롯 설정
	virtual void SetRanMobileRewardItemSlotData( const char* strSlotID, int nIndex ) = 0;

public :
	// 현재 드래그중인 스킬 슬롯의 스킬 ID
	virtual SNATIVEID GetDragSlotSkillID() = 0;

	// 스킬 슬롯 쿨다운
	virtual void SetSkillSlotCoolDown( const SNATIVEID& sID ) = 0;
	// 아이템 슬롯 쿨다운
	virtual void SetItemSlotCoolDown( const SNATIVEID& sID ) = 0;

public :
	// 채팅 리스트 UI에 출력
	void PrintChatList( int nLogID, LIST_CHAT& listChat );

public :
	// 이름 디스플레이 추가
	virtual void AddNameDisplay( CROWREN& sDispName, const char* szOwnerName = NULL ) = 0;
	// 이름 디스플레이 클리어
	virtual void ClearDisplay() = 0;

	// 개인상점 디스플레이
	virtual void AddPrivateMarketDisplay( const CROWREN& sDispName ) = 0;
	
	// 개인상점 디스플레이 제거
	virtual void DeletePrivateMarketDisplay( DWORD dwID ) = 0;

	// 헤드쳇 디스플레이
	virtual void AddHeadChatDisplay( const std::string& strName, DWORD dwNameColor,
		const std::string& strChat, DWORD dwColor, const D3DXVECTOR2& vPos ) = 0; 
	// 데미지 디스플레이
	virtual void AddDamageDisplay( const D3DXVECTOR3& vPos, int nDamage, 
		DWORD dwDamageFlag, BOOL bAttack, const STARGETID& sCauser, const STARGETID& sTarget ) = 0;

	// 개인상점 디스플레이 토글 상태 끄기
	virtual void ToggleOffPrivateMarketDisplay( DWORD dwID ) = 0;

	// 이름 디스플레이 클럽 아이콘 변경
	virtual void ChangeClubIcon( DWORD dwClubID ) = 0;

public:

	// OLD UI
	virtual void SetTargetInfoSelect( STARGETID sTargetID ) = 0;
	virtual void SetTargetInfoMouseOver( STARGETID sTargetID ) = 0;
	virtual void ResetTargetInfoSelect() = 0;
	virtual void AddChatMsg( const CString& strName, const D3DCOLOR& dwIDColor, 
		const CString& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos ) = 0;
	virtual void ClearNameList() = 0;
	virtual void NameDisplaySetCount( DWORD Count ) = 0;

	virtual void UiFrameMove(LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime)  = 0;
	virtual void UiRender(LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

	virtual void ADD_MARKET( const CROWREN &sDISP ) = 0;

public:
	virtual bool GetCapslockKeyState() = 0;
	virtual void SetCapslockKeyState(bool bDown) = 0;

	virtual bool RegShowTooltipEnableState( int nWidgetID, int nSubID, const char* strInstance, WORD wMID, WORD wSID, WORD wLevel, bool bUpdate ) = 0;
	virtual bool UnregShowTooltipEnableState( int nWidgetID, int nSubID, const char* strInstance ) = 0;
};
