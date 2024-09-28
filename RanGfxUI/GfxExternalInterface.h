#pragma once

#include "../../RanThirdParty/GFx/Src/GFx/GFx_Player.h"
#include "./GfxCallFunc.h"

class CLIKDataBindingAdapter;

class GfxExternalInterface : public ExternalInterface
{
public :
	GfxExternalInterface(void);
	virtual ~GfxExternalInterface(void);

private :
	typedef std::vector< GFx::Value >		VEC_GFXVALUE;
	typedef VEC_GFXVALUE::iterator			VEC_GFXVALUE_ITER;

private :
	CLIKDataBindingAdapter* m_pGameDataBinder;

	// 채팅 로그 핸들러
	VEC_GFXVALUE m_vecChatLogHandlers;

	// 콜백 함수 포인터 보관
	MAP_CALLBACK_FUNC m_mapCallbackFunc;

	// AS 코어 초기화가 완료되었나?
	bool m_bCreateCompleteGameMovie;

	// 입력 언어
	CString m_strConversionMode;

public :
	// 스케일폼 콜백 메소드
	virtual void Callback( Movie* pMovie, const char* strMethodName, const GFx::Value* args, unsigned argCount );

public :
	// AS -> Client 콜백 함수 등록
	void InitCallbackFunc();

	// External Call
public :
	// UI 오픈
	void OnWidgetOpen( const GfxCallbackFuncValue& value );
	// UI 클로즈
	void OnWidgetClose( const GfxCallbackFuncValue& value );
	// UI 언로드
	void OnWidgetUnload( const GfxCallbackFuncValue& value );
	// 게임 데이터 어탭터 초기화
	void OnDataAdapterInit( const GfxCallbackFuncValue& value );
	// AS 코어 초기화 완료
	void OnCreateCoreComplete( const GfxCallbackFuncValue& value );
	// 커스텀 이벤트
	void OnCustomEvent( const GfxCallbackFuncValue& value );
	// 채팅 링크 클릭
	void OnChatLinkClick( const GfxCallbackFuncValue& value );
	// 채팅 이름 정보보기
	void OnChatReferName( const GfxCallbackFuncValue& value );
	// 채팅 귓속말
	void OnChatWhisper( const GfxCallbackFuncValue& value );
	// 디버깅 에러 메시지 출력
	void OnPrintDebug( const GfxCallbackFuncValue& value );
	// 개인상점 팝업 클릭
	void OnPrivateMarketClick( const GfxCallbackFuncValue& value );
	// 아이템 이름 클릭
	void OnItemNameClick( const GfxCallbackFuncValue& value );
	// 이미지 릴리즈
	void OnImageRelease( const GfxCallbackFuncValue& value );
	// 해상도 변경됨.
	void OnChangeResolution( const GfxCallbackFuncValue& value );
	// 디버그 씬 초기화 완료
	void OnDebugSceneComplete( const GfxCallbackFuncValue& value );
	// 디버그 씬 토글
	void OnToggleDebugScene( const GfxCallbackFuncValue& value );
	// 텍스트 카피
	void OnTextCopy( const GfxCallbackFuncValue& value );

	// Data Adapter Call
public :
	// 드래그 시작
	static void OnNotifyHoldStart( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 드래그 끝
	static void OnNotifyHoldEnd( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 슬롯 강제 드래그 끝
	static void OnNotifyHoldEndForce( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// UI 영역 밖에서 드래그 끝
	static void OnNotifyHoldEndOutside( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 슬롯 왼쪽 마우스 클릭 (드래그 가능 속성이 꺼져있을 경우만 동작)
	static void OnNotifySlotLeftMouseClick( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 슬롯 오른쪽 마우스 클릭
	static void OnNotifySlotRightMouseClick( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// 슬롯 툴팁
	static void OnGetSlotToolTip( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 슬롯 서브 툴팁
	static void OnGetSlotSubTooltip( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 툴팁 추가 슬롯 개수
	static void OnGetAddSlotCount( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 툴팁 추가 슬롯 바인딩 ID
	static void OnGetAddSlotBinding( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 툴팁 교환 슬롯 개수
	static void OnGetTradeSlotCount( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 툴팁 교환 슬롯 바인딩 ID
	static void OnGetTradeSlotBinding( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 툴팁 슬롯
	static void OnShowTooltipSlot_bySlot( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	static void OnShowTooltipSlot_byID( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 툴팁 하이드
	static void OnHideTooltipSlot( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 툴팁 Width 최대값
	static void OnGetMaxTooltipWidth( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 툴팁 Width 최소값
	static void OnGetMinTooltipWidth( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// 채팅 로그 등록
	static void OnRegisterMessageLogListener( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 채팅 로그 등록 해제
	static void OnUnregisterMessageLogListener( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// 렌더 텍스처 등록
	static void OnRegisterRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 렌더 텍스처 등록 해제
	static void OnUnregisterRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// 클럽 아이콘 렌더텍스처
	static void OnClubIconRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// HTML View Visible
	static void OnHtmlViewSetVisible( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// TextInput Conversion Mode
	static void OnSaveConversionMode( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	static void OnSetConversionMode( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// 디버그
	static void OnIsGameRun( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// TextInput
	static void OnGetCompositionString( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	static void OnSetFocusInTextInput( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// 프로퍼티 저장
	static void OnSaveProp( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// 인터페이스
	static void OnRegisterInterface( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	static void OnRegisterDisplayInterface( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	static void OnGetLargeMapIconTooltip(GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params);
	static void OnChatLogRefresh(GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params);
public :
	bool IsCraeteCompleteGameMovie() { return m_bCreateCompleteGameMovie; }
	void ResetGameMovie() { m_bCreateCompleteGameMovie = false; }

public :
	void SetGameDataBinder( CLIKDataBindingAdapter* pDataBinder ) { m_pGameDataBinder = pDataBinder; }
	CLIKDataBindingAdapter* GetGameDataBinder() { return m_pGameDataBinder; }

	VEC_GFXVALUE& GetChatLogHandler() { return m_vecChatLogHandlers; }
};
