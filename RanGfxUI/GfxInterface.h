#pragma once

#include "../RanLogicClient/GfxInterfaceBase.h"
#include "./OldUI/OldInterface.h"
#include "./GfxDef.h"

class GfxCore;
class GfxGameScene;
class GfxLoadingScene;
class GfxLoginScene;
class GfxDebugScene;

typedef std::vector< std::string >	VEC_STRING;
typedef VEC_STRING::iterator		VEC_STRING_ITER;

namespace Scaleform
{
	namespace GFx
	{
		class Value;
	}
}

GfxCore* GetCore();

class GfxInterface : public GfxInterfaceBase
{
protected :
	GfxInterface(void);
	GfxInterface( const GfxInterface& value );
	virtual ~GfxInterface(void);

public :
	static GfxInterface* GetInstance();

private :
	static const char* SPLIT_STRING_INST;
	static const char* SPLIT_STRING_ETC;

	// gc 호출 주기
	static const float fGC_CallTime;

private :
	struct SUpdateClubMark
	{
		int nWidgetID;
		int nSubID;
		std::string strParam;
		std::string strInst;

		SUpdateClubMark()
			: nWidgetID( -1 )
			, nSubID( 0 )
		{
		}
	};

public :
	enum EMAS_Interface
	{
		AS_OpenWidget = 0,			// widget 열기
		AS_CloseWidget,				// widget 닫기
		AS_CloseAllWidget,			// 전체 widget 닫기
		AS_UnloadWidget,			// widget 언로드
		AS_IsOpenWidget,			// widget 열려있나?
		AS_IsLoadWidget,			// widget 로드된 상태인가?
		AS_GetMultiWidgetCount,		// 멀티 widget 열린 개수
		AS_EscWidgetClose,			// ESC키로 닫을 수 있는 UI 하나씩 닫기
		AS_SetStageEvent,			// 스테이지 이벤트 설정
		AS_SetProperty,				// 프로퍼티 설정
		AS_GetProperty,				// 프로퍼티 얻기
		AS_SetEvent,				// 이벤트 심기
		AS_SetDataProvider,			// 데이터 프로바이더 설정
		AS_RemoveDataProvider,		// 데이터 프로바이더 제거
		AS_GetDataProvider,			// 데이터 프로바이더 얻기
		AS_SetDataProviderProp,		// 데이터 프로바이더 프로퍼티 설정
		AS_SetListItemPropArray,	// 리스트 아이템 프로퍼티 설정
		AS_SetListItemProp,			// 리스트 아이템 프로퍼티 설정
		AS_CallListItemFunc,		// 리스트 아이템 함수 호출
		AS_CallFunction,			// 함수 호출
		AS_SetFocus,				// 포커스 설정
		AS_SetTextFormat,			// 텍스트 포맷 설정
		AS_SetTween,				// 트윈 설정
		AS_HitTestEnable,			// 마우스 히트 테스트 설정
		AS_GetSlotTooltipPosSize,	// 슬롯 툴팁 위치/사이즈
		AS_SlotDragStop,			// 슬롯 드래그 스탑
		AS_GetDragSlotBinding,		// 현재 드래그 중인 슬롯 바인딩 ID
		AS_GetWidgetInstance,		// 인스턴스 얻기
		AS_ResetWidgetPosition,		// UI 위치 초기화
		AS_IsOpenTooltip,			// 툴팁 열려있나?
		AS_GetDataProviderArray,	
		AS_SetSpeechBubble,			// 말풍선 심기;
		AS_DelSpeechBubble,			// 말풍선 제거;
		AS_SetFakeModal,			// 페이크 모달 설정
		AS_SetDataProvider_Terms,	// 데이터 프로바이더 설정; 존재하는 데이터 프로바이더를 찾고 해당 데이터 값갱신;
		AS_SnapCheck,				// 스냅되는 좌표 얻기
		AS_SetUIAlpha,				// 전체 UI 알파값 설정
		AS_GC,						// 가비지 컬렉션
		AS_RemoveTween,
		AS_RegShowTooltipEnableState,
		AS_UnregShowTooltipEnableState,
		AS_UpdateDraggingSlot,		// 드래그 중인 슬롯 업데이트
		AS_SetFoward,
		AS_SetHeadData,				// 트리 헤드 데이터 설정
		AS_SetTreeData,				// 트리 데이터 설정
		AS_GetHeadData,				// 트리 헤드 데이터 얻기
		AS_GetTreeData,				// 트리 데이터 얻기
		AS_ChangeHeadData,			// 트리 헤드 데이터 변경
		AS_ChangeTreeData,			// 트리 데이터 변경
		AS_SetTextFormatSize = 49,		// 텍스트 포맷 크기 설정
		AS_Interface_Size,
	};

private :
	LPDIRECT3DDEVICE9	m_pDevice;

	GfxCore*			m_pCore;
	GfxGameScene*		m_pGameScene;
	//GfxLoadingScene*	m_pLoadingScene;
	GfxLoginScene*		m_pLoginScene;
	GfxDebugScene*		m_pDebugScene;

	// 슬롯 ID 보관
	VEC_STRING				m_vecRegSlotID;
	// HTML 뷰 ID
	std::map< int, int >	m_mapHtmlWebViewID;
	
	bool m_bUpdatedClubIconRT;
	// 업데이트 할 클럽 마크 정보
	std::queue< SUpdateClubMark > m_queUpdateClubMark;

	bool m_bReload;
	bool m_bShowDebugScene;

	float	m_fTimerGC;		// gc 호출 타이머
	bool	m_bCallGC;		// gc 호출 해야하는가?

	OldInterface* m_pOldInterface;

public :
	class SASInterface* m_pASInterface;

public:
	virtual OldInterface* GetOldInterface() { return m_pOldInterface; }

public :
	virtual void OnCreateDevice( HWND hWnd, LPDIRECT3DDEVICE9 pDevice );
	virtual void OnResetDevice( LPDIRECT3DDEVICE9 pDevice );
	virtual void OnLostDevice();
	virtual void OnDestroyDevice();
	virtual void OnFrameMove( float fElapsedTime );
	virtual void OnFrameRender( float fElapsedTime );

	virtual void InitOldInterface();

	virtual void SetLoadingData( const SNATIVEID& sMapID, const std::string& strLoadingImgFile, bool bShowGrade );
	virtual void OnInitGame();
	virtual void OnInitLoading();
	virtual void OnBeginLoading( ELoadingSceneType eType );
	virtual void OnEndLoading();

public:
	// 게임 스테이지로 접속시 초기화 시킬것들 초기화시킨다;
	virtual void InitGameStage(void);

public :
	// Debug Scene Toggle
	virtual void ToggleDebugScene();
	// D3DPRESENT_PARAMETERS 저장
	virtual void StorePresentParameters( D3DPRESENT_PARAMETERS pp );
	// 미리 처리할 윈도우 메시지 처리
	virtual void PreProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	// 윈도우 메시지 처리, true 반환시 윈도우 기본 프록시저 스킵
	virtual bool ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public :
	virtual void SetDisplayCountryMark( bool bValue );

	// 폰트 언어 설정
	virtual void SetFontLanguage( EMGFX_FONT_LANG emFontLang );

	virtual bool IsMouseInUI();

	virtual const D3DPRESENT_PARAMETERS& GetPresentParams();
	virtual void GetMousePos( int& nMouseX, int& nMouseY );

	// 키보드 입력 영문으로 변경
	virtual void SetConversionModeEn();
	// 현재 입력 언어
	CString GetConversionMode();

	// 안티앨리어싱 on/off
	virtual void SetEdgeAAMode( int nMode );

	// 드랍 아이템 이름 클릭으로 줍기 설정
	virtual void SetInteractDropItem( bool bEnable );

	virtual std::string GetIMECompositionString(void);

public :
	virtual void Reload();

	// 닫힌 UI 일정 시간 지나면 언로드
	void CheckDeleteWidget( float fElapsedTime );

public :
	Scaleform::GFx::Value* FindInstance( WORD wWidgetID, WORD wSubID, std::string strInst );

private :
	// AS 함수 호출
	bool Invoke( EMAS_Interface emInterface, const Scaleform::GFx::Value* pArgs = NULL,
		const int nArgsCount = 0, Scaleform::GFx::Value* pResult = NULL );

	// 스트링 배열 생성을 위해 구분자 단위로 잘라줌.
	void SplitString( VEC_STRING& vecSplitStr, const char* str, const std::string& strSeparator );
	// 스트링 배열 생성
	void CreateStringArray( const char* str, Scaleform::GFx::Value* pValue, const std::string& strSeparator );
	
	// 프로퍼티 설정 함수 공통 부분
	void PreSetProperty( Scaleform::GFx::Value* pArgs, 
		int nWidgetID, int nSubID, const char* strProp, const char* strInstance );
	// 프로퍼티 얻기 함수 공통 부분
	EMGFX_OBJECT_TYPE PreGetProperty( Scaleform::GFx::Value* pGfxValue, int nWidgetID, int nSubID,
		const char* strProp, const char* strInstance );
	// 데이터 프로바이더 얻기 공통 부분
	EMGFX_OBJECT_TYPE PreGetDataProvider( Scaleform::GFx::Value* pGfxValue, int nWidgetID, int nSubID, int nIndex,
		const char* strName, const char* strInstance );
	// 데이터 프로바이더 프로퍼티 설정 공통 부분
	void PreSetDataProviderProp( Scaleform::GFx::Value* pArgs, int nWidgetID, int nSubID, int nIndex,
		const char* strName, const char* strInstance );
	// 리스트 아이템 프로퍼티 설정 함수 공통 부분
	Scaleform::GFx::Value* PreSetListItemProp( Scaleform::GFx::Value* pArgs,int nWidgetID, int nSubID, int nIndex,
		const char* strInstName, const char* strProp, const char* strInstance );

	// 트리 데이터 삽입
	void InsertTreeData( Scaleform::GFx::Value* pParentArray, int& nArrayIndex, 
		const VEC_STRING& vecObjectID, LuaTable tbData );

public :
	// UI 프리로드
	virtual void PreloadWidget();

	// UI 언로드
	virtual void UnloadWidget( int nWidgetID, int nSubID );

	// 슬롯 ID 할당
	virtual void RegSlot( WORD wType, const char* strSlotID );
	// 슬롯 ID 제거
	virtual void UnregSlot( const std::string& strSlotID );
	// HTML View ID 할당
	virtual void RegHtmlWebView( int nViewID );
	// HTML View ID
	virtual int GetHtmlWebID( int nViewID );
	// HTML View 이동
	virtual void MoveHtmlWebView( int nViewID, bool bVisible, int nX, int nY, int nWidth, int nHeight );

	// UI 열기
	virtual void OpenWidget( int nWidgetID, int nSubID = 0, bool bPreload = false );
	// UI 닫기
	virtual void CloseWidget( int nWidgetID, int nSubID = 0 );
	// 전체 UI 닫기
	virtual void CloseAllWidget( bool bDelete );
	// UI 열려있나?
	virtual bool IsOpenWidget( int nWidgetID, int nSubID = 0 );
	// UI 로드된 상태인가?
	virtual bool IsLoadWidget( int nWidgetID, int nSubID = 0 );
	// 멀티 widget 열린 개수
	virtual int GetMultiWidgetCount( int nWidgetID );
	// ESC 키로 닫을 수 있는 UI 하나씩 닫기, 닫을게 없으면 false
	virtual bool EscWidgetClose();
	// 툴팁 열려있나?
	virtual bool IsOpenTooltip();

	// 스테이지 이벤트 설정
	virtual void SetStageEvent( const char* strEvent, const char* strCallBack, const char* strRetParams );

	// 프로퍼티 설정
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int nValue );
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int nValue );
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double dValue );
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool bValue );
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue );
	virtual void SetProperty_WString( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue );

	// 프로퍼티 설정; 
	// 하나의 속송에 다수의 개체가 여러번 콜하여 부하를 일으키는 것을 방지하기 위함임;
	// virtual void SetProperty_Array( int nWidgetID, int nSubID, const char* strProp, const char* strInstanceArray, const char* strValueArray );
	virtual void SetProperty_Array( int nWidgetID, int nSubID, LuaTable tbProp, LuaTable tbInstance, LuaTable tbValue, LuaTable tbValueType );

	// 프로퍼티 얻기
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int& nValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int& nValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double& dValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool& bValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, std::string& strValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, LuaTable& tb );

	// 이벤트 심기
	virtual void SetEvent( int nWidgetID, int nSubID, const char* strEvent, const char* strCallBack,
						   const char* strRetProp, const char* strDelivery, const char* strInstance );

	// 데이터 프로바이더 설정
	virtual void SetDataProvider( int nWidgetID, int nSubID, const char* strDataProvider,const char* strInstance, bool bAdd );
	virtual void SetDataProviderEx( int nWidgetID, int nSubID, const char* strObjectID, const char* strDataProvider,
									const char* strInstance, bool bAdd );
	virtual void SetDataProviderComplex( int nWidgetID, int nSubID, const char* strObjectID, LuaTable tbData,
									const char* strInstance, bool bAdd );
	virtual void SetDataProvider_Terms(int nWidgetID, int nSubID
		, const char* strInstName
		, LuaTable tbWhereProvName, LuaTable tbWhereProvData, LuaTable tbWhereProvType
		, LuaTable tbNewProvName, LuaTable tbNewProvData, LuaTable tbNewProvType);

	// 데이터 프로바이더 제거
	virtual void RemoveDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strInstance, bool bAll );

	// 데이터 프로바이더 얻기
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int& nValue );
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int& nValue );
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double& dValue );
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool& bValue );
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, std::string& strValue );

	// 데이터 프로바이더 얻기;
	// 하나의 컴포넌트에 접근하는건데 여러번 콜되어 부하를 일으키는 것을 방지하기 위함임;
	// 요청 순서대로 테이블에 담겨서 반환됨;
	virtual bool GetDataProvider_Array( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, LuaTable& tbResult );

	// 데이터 프로바이더 프로퍼티 설정
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int nValue );
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int nValue );
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double dValue );
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool bValue );
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, const char* strValue );

	virtual void SetListItemProp_Array( int nWidgetID, int nSubID, char* strIndexArr, const char* strInstNameArr, const char* strPropArr,
		const char* strValueArr, const char* strTypeArr, const char* strInstance );
	// 리스트 아이템 프로퍼티 설정
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strProp, const char* strInstName,
		int nValue, const char* strInstance );
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp,
		unsigned int nValue, const char* strInstance );
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strProp, const char* strInstName,
		double dValue, const char* strInstance );
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strProp, const char* strInstName,
		bool bValue, const char* strInstance );
	virtual void SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strProp, const char* strInstName,
		const char* strValue, const char* strInstance );

	// 리스트 아이템 함수 호출
	virtual void CallListItemFunc( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strFunc,
		const char* strParams, const char* strInstance );

	// 함수 호출
	virtual void CallFunction( int nWidgetID, int nSubID, const char* strName, const char* strParams, const char* strInstance );
	// 포커스 설정
	virtual void SetFocus( int nWidgetID, int nSubID, const char* strInstance );
	// 텍스트 컬러 설정
	virtual void SetTextFormat( int nWidgetID, int nSubID, DWORD dwColor, bool bUnderline,
		int nBeginIndex, int nEndIndex, const char* strInstance );
	// 텍스트 사이즈 설정
	virtual void SetTextFormatSize( int nWidgetID, int nSubID, DWORD dwSize, const char* strInstance );
	// 트윈 설정
	virtual void SetTween( int nWidgetID, int nSubID, int nTime, int nDelay, int nEase, bool bLoop, const char* strComplete,
		const char* strProp, const char* strValue, const char* strInstance );

	virtual void RemoveTween( int nWidgetID, int nSubID, const char* strInstance);

	// 마우스 히트 테스트 설정
	virtual void HitTestEnable( int nWidgetID, int nSubID, bool bEnable, const char* strInstance );

	// 슬롯 툴팁 위치/사이즈
	virtual void GetSlotTooltipPosSize( int& nX, int& nY, int& nWidth, int& nHeight );

	// UI 위치 초기화
	virtual void ResetWidgetPosition();

	// 말풍선 추가
	virtual void SetSpeechBubble(int nWidgetID, int nSubID, const char* strInstance, const char* strText);
	// 말풍선 삭제
	virtual void DelSpeechBubble(int nWidgetID, int nSubID, const char* strInstance);
	// 페이크 모달 설정
	virtual void SetFakeModal( bool bValue );
	// 스냅 되는 좌표
	virtual void SnapCheck( int nWidgetID, int nSubID, int nX, int nY, int nWidth, int nHeight, int& nRetX, int& nRetY );
	// 전체 UI 알파값
	virtual void SetUIAlpha( float fAlpha );
	// 클럽 마크 업데이트
	virtual void UpdateClubMark( int nWidgetID, int nSubID, const char* szParam, const char* szInst );
	// 홀드 슬롯 업데이트
	virtual void UpdateHoldSlot();
	// UI 순서 맨앞으로
	virtual void SetFoward( int nWidgetID, int nSubID );

	// 트리 헤드 데이터
	virtual void SetHeadData( int nWidgetID, int nSubID, const char* strObjectID,
		LuaTable tbData, const char* strInstance );
	virtual void GetHeadData( int nWidgetID, int nIndex, const char* szInst, int nSubID, LuaTable& tb );
	virtual void ChangeHeadData( int nWidgetID, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID );

	// 트리 데이터
	virtual void SetTreeData( int nWidgetID, int nSubID, LuaTable tbKey, const char* strObjectID, 
		LuaTable tbData, const char* strInstance );
	virtual void GetTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szInst, int nSubID, LuaTable& tb );
	virtual void ChangeTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID );

public :
	// 채팅 메시지 추가
	virtual void AddChatMessage( int nLogID, const std::string& strMsg, DWORD dwColor, 
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName );
	// 링크 메시지 추가
	virtual void AddLinkMessage( int nLogID, const std::string& strMsg, DWORD dwColor,
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		VEC_LINK_TEXT_DATA& vecLinkTextData,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName );

public :
	// 슬롯 드래그 스탑
	virtual void SlotDragStop();

	// 슬롯 아이템
	virtual bool GetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem );
	virtual void SetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem );

	// 슬롯 데이터 리셋
	virtual void ResetSlot( const char* strSlotID );
	// 아이템 슬롯 데이터 설정
	virtual void SetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID );
	virtual void SetItemSlotData_NonStrengthen( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID );
	virtual void SetItemSlotData_BORNTIME( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, DWORD dwBornT );
	// 스킬 슬롯 데이터 설정
	virtual void SetSkillSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nLevel = -1 );
	// 인벤토리 슬롯 데이터 설정
	virtual void SetInventorySlotData( const char* strSlotID, int nPosX, int nPosY );
	// 재 구입 슬롯 데이터 설정
	virtual void SetRebuySlotData( const char* strSlotID, int nRebuyID, WORD wMID, WORD wSID );
	// 소환수 데이터 설정;
	virtual void SetSummonStateSlotData( const char* strSlotID, const char* strIconName, INT nIconIdxX, INT nIconIdxY );
	// 은행 슬롯 데이터 설정
	virtual void SetBankSlotData( const char* strSlotID, int nPosX, int nPosY );
	// 위시리스트 슬롯 데이터 설정
	virtual void SetWishlistSlotData( const char* strSlotID, int nPosX, int nPosY );
	// 캐릭터 정보 슬롯 데이터 설정
	virtual void SetCharInfoSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, int nSlot );
	// 스킬 퀵슬롯 설정
	virtual void SetSkillQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex );
	// 아이템 퀵슬롯 설정
	virtual void SetItemQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex );
	// 미니게임 : 주사위 보상 슬롯 설정;
	virtual void SetItemMiniGameDiceRewardSlotData( const char* strSlotID, DWORD dwID, int nIndex );
	// NPC 대화 마켓 슬롯 설정
	virtual void SetNpcMarketSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY );
	// NPC 대화 마켓 카트 슬롯 설정
	virtual void SetNpcMarketCartSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, WORD wIndex );
	// 거래창 슬롯 설정
	virtual void SetTradeSlotData(const char* strSlotID, int nPosX, int nPosY, bool _MyBox);
	// PointShop 슬롯 설정
	virtual void SetPointShopSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY );
	// PointShop 카트 슬롯 설정
	virtual void SetPointShopCartSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nIndex );
	//정보보기창 슬롯 설정
	virtual void SetReferSlotData( const char* strSlotID, int nSlot );
	// 개인상점검색 포인트샵 아이템 등록 슬롯
	virtual void SetPMSPointTradeSlotData( const char* strSlotID, WORD wPosX, WORD wPosY );
	// 개인상점검색 포인트샵 인덱스 슬롯
	virtual void SetPMSPointTradeSlotData_Index( const char* strSlotID, DWORD dwIndex, DWORD dwCount );
	// 개인상점 아이템 등록 슬롯
	virtual void SetPrivateMarketSlotData( const char* strSlotID, int nPosX, int nPosY, bool bSelf );
	// 개인상점검색 슬롯
	virtual void SetPrivateMarketSearchSlotData( const char* strSlotID, DWORD dwIndex, WORD wCount, bool bSearchPage );
	// 우편 슬롯
	virtual void SetReceivePostSlotData( const char* strSlotID, DWORD dwIndex, DWORD dwCount );
	// PostBoxSub_PreviewPageOpen
	virtual void SetSlot_PostBoxSub_PreviewPageOpen( const char* strSlotID );
	// 펫 아이템 장착 슬롯 설정
	virtual void SetPetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD nSlot );
	// 클럽 창고 슬롯 설정
	virtual void SetClubStorageSlotData( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY);
	// 클럽 창고 아이템 넣어주기
	virtual void SetClubStorageItem(const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY);
	// 락커 슬롯 설정
	virtual void SetLockerSlot( const char* strSlotID, int nIndex, int nPosX, int nPosY );
	// 개조 카드 슬롯 설정
	virtual void SetRebuildCardSlot( const char* strSlotID, int nPosX, int nPosY );
	// 파티 분배 슬롯 설정;
	virtual void SetItemSlotData_PartyDistribution( const char* strSlotID, DWORD dwItemID, WORD wSlotIndex);
	// 아이템 수리 슬롯 설정
	virtual void SetItemRepairSlotData( const char* strSlotID, int nSlot );
	// 로또 당첨 아이템 보기 슬롯 설정;
	virtual void SetLottoWinItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID );
	// 로또 이전 당첨자 리스트 당첨 아이템 목록 슬롯 설정;
	virtual void SetLottoPreWinListItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID, int nRanking );
	// 란 모바일 보상 슬롯 설정
	virtual void SetRanMobileRewardItemSlotData( const char* strSlotID, int nIndex );

public :
	// 현재 드래그중인 스킬 슬롯의 스킬 ID
	virtual SNATIVEID GetDragSlotSkillID();

	// 스킬 슬롯 쿨다운
	virtual void SetSkillSlotCoolDown( const SNATIVEID& sID );
	// 아이템 슬롯 쿨다운
	virtual void SetItemSlotCoolDown( const SNATIVEID& sID );

public :
	// 이름 디스플레이 추가
	virtual void AddNameDisplay( CROWREN& sDispName, const char* szOwnerName = NULL );
	// 이름 디스플레이 클리어
	virtual void ClearDisplay();

	// 개인상점 디스플레이 제거
	virtual void DeletePrivateMarketDisplay( DWORD dwID );

	// 헤드쳇 디스플레이
	virtual void AddHeadChatDisplay( const std::string& strName, DWORD dwNameColor,
		const std::string& strChat, DWORD dwColor, const D3DXVECTOR2& vPos ); 
	// 데미지 디스플레이
	virtual void AddDamageDisplay( const D3DXVECTOR3& vPos, int nDamage, 
		DWORD dwDamageFlag, BOOL bAttack, const STARGETID& sCauser, const STARGETID& sTarget );
	// 개인상점 디스플레이
	virtual void AddPrivateMarketDisplay( const CROWREN& sDispName );

	// 개인상점 디스플레이 토글 상태 끄기
	virtual void ToggleOffPrivateMarketDisplay( DWORD dwID );

	// 이름 디스플레이 클럽 아이콘 변경
	virtual void ChangeClubIcon( DWORD dwClubID );

	virtual bool GetCapslockKeyState();
	virtual void SetCapslockKeyState(bool bDown);

	virtual bool RegShowTooltipEnableState( int nWidgetID, int nSubID, const char* strInstance, WORD wMID, WORD wSID, WORD wLevel, bool bUpdate );
	virtual bool UnregShowTooltipEnableState( int nWidgetID, int nSubID, const char* strInstance );

public :

	// OLD UI
	virtual void SetTargetInfoSelect( STARGETID sTargetID );
	virtual void SetTargetInfoMouseOver( STARGETID sTargetID );
	virtual void ResetTargetInfoSelect();

	virtual void ClearNameList() { m_pOldInterface->ClearNameList(); }

	virtual void GfxInterface::AddChatMsg( const CString& strName, const D3DCOLOR& dwIDColor, 
		const CString& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos ) ;

	virtual void NameDisplaySetCount( DWORD	Count );

	virtual void UiFrameMove(LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime);
	virtual void UiRender(LPDIRECT3DDEVICEQ pd3dDevice );

	virtual void ADD_MARKET( const CROWREN &sDISP );


public :
	GfxCore* GetCore() { return m_pCore; }
	GfxGameScene* GetGameScene() { return m_pGameScene; }
	GfxDebugScene* GetDebugScene() { return m_pDebugScene; }
	LPDIRECT3DDEVICE9 GetDevice() { return m_pDevice; }

	bool GetUpdatedClubRT() { return m_bUpdatedClubIconRT; }
	void SetUpdatedClubRT( bool bValue ) { m_bUpdatedClubIconRT = bValue; }
};
