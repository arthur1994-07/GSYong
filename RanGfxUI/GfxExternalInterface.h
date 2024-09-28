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

	// ä�� �α� �ڵ鷯
	VEC_GFXVALUE m_vecChatLogHandlers;

	// �ݹ� �Լ� ������ ����
	MAP_CALLBACK_FUNC m_mapCallbackFunc;

	// AS �ھ� �ʱ�ȭ�� �Ϸ�Ǿ���?
	bool m_bCreateCompleteGameMovie;

	// �Է� ���
	CString m_strConversionMode;

public :
	// �������� �ݹ� �޼ҵ�
	virtual void Callback( Movie* pMovie, const char* strMethodName, const GFx::Value* args, unsigned argCount );

public :
	// AS -> Client �ݹ� �Լ� ���
	void InitCallbackFunc();

	// External Call
public :
	// UI ����
	void OnWidgetOpen( const GfxCallbackFuncValue& value );
	// UI Ŭ����
	void OnWidgetClose( const GfxCallbackFuncValue& value );
	// UI ��ε�
	void OnWidgetUnload( const GfxCallbackFuncValue& value );
	// ���� ������ ������ �ʱ�ȭ
	void OnDataAdapterInit( const GfxCallbackFuncValue& value );
	// AS �ھ� �ʱ�ȭ �Ϸ�
	void OnCreateCoreComplete( const GfxCallbackFuncValue& value );
	// Ŀ���� �̺�Ʈ
	void OnCustomEvent( const GfxCallbackFuncValue& value );
	// ä�� ��ũ Ŭ��
	void OnChatLinkClick( const GfxCallbackFuncValue& value );
	// ä�� �̸� ��������
	void OnChatReferName( const GfxCallbackFuncValue& value );
	// ä�� �ӼӸ�
	void OnChatWhisper( const GfxCallbackFuncValue& value );
	// ����� ���� �޽��� ���
	void OnPrintDebug( const GfxCallbackFuncValue& value );
	// ���λ��� �˾� Ŭ��
	void OnPrivateMarketClick( const GfxCallbackFuncValue& value );
	// ������ �̸� Ŭ��
	void OnItemNameClick( const GfxCallbackFuncValue& value );
	// �̹��� ������
	void OnImageRelease( const GfxCallbackFuncValue& value );
	// �ػ� �����.
	void OnChangeResolution( const GfxCallbackFuncValue& value );
	// ����� �� �ʱ�ȭ �Ϸ�
	void OnDebugSceneComplete( const GfxCallbackFuncValue& value );
	// ����� �� ���
	void OnToggleDebugScene( const GfxCallbackFuncValue& value );
	// �ؽ�Ʈ ī��
	void OnTextCopy( const GfxCallbackFuncValue& value );

	// Data Adapter Call
public :
	// �巡�� ����
	static void OnNotifyHoldStart( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// �巡�� ��
	static void OnNotifyHoldEnd( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� ���� �巡�� ��
	static void OnNotifyHoldEndForce( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// UI ���� �ۿ��� �巡�� ��
	static void OnNotifyHoldEndOutside( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� ���� ���콺 Ŭ�� (�巡�� ���� �Ӽ��� �������� ��츸 ����)
	static void OnNotifySlotLeftMouseClick( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� ������ ���콺 Ŭ��
	static void OnNotifySlotRightMouseClick( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// ���� ����
	static void OnGetSlotToolTip( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� ���� ����
	static void OnGetSlotSubTooltip( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� �߰� ���� ����
	static void OnGetAddSlotCount( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� �߰� ���� ���ε� ID
	static void OnGetAddSlotBinding( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� ��ȯ ���� ����
	static void OnGetTradeSlotCount( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� ��ȯ ���� ���ε� ID
	static void OnGetTradeSlotBinding( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� ����
	static void OnShowTooltipSlot_bySlot( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	static void OnShowTooltipSlot_byID( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� ���̵�
	static void OnHideTooltipSlot( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� Width �ִ밪
	static void OnGetMaxTooltipWidth( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� Width �ּҰ�
	static void OnGetMinTooltipWidth( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// ä�� �α� ���
	static void OnRegisterMessageLogListener( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ä�� �α� ��� ����
	static void OnUnregisterMessageLogListener( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// ���� �ؽ�ó ���
	static void OnRegisterRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// ���� �ؽ�ó ��� ����
	static void OnUnregisterRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	// Ŭ�� ������ �����ؽ�ó
	static void OnClubIconRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// HTML View Visible
	static void OnHtmlViewSetVisible( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// TextInput Conversion Mode
	static void OnSaveConversionMode( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	static void OnSetConversionMode( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// �����
	static void OnIsGameRun( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// TextInput
	static void OnGetCompositionString( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
	static void OnSetFocusInTextInput( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// ������Ƽ ����
	static void OnSaveProp( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );

	// �������̽�
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
