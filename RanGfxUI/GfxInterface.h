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

	// gc ȣ�� �ֱ�
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
		AS_OpenWidget = 0,			// widget ����
		AS_CloseWidget,				// widget �ݱ�
		AS_CloseAllWidget,			// ��ü widget �ݱ�
		AS_UnloadWidget,			// widget ��ε�
		AS_IsOpenWidget,			// widget �����ֳ�?
		AS_IsLoadWidget,			// widget �ε�� �����ΰ�?
		AS_GetMultiWidgetCount,		// ��Ƽ widget ���� ����
		AS_EscWidgetClose,			// ESCŰ�� ���� �� �ִ� UI �ϳ��� �ݱ�
		AS_SetStageEvent,			// �������� �̺�Ʈ ����
		AS_SetProperty,				// ������Ƽ ����
		AS_GetProperty,				// ������Ƽ ���
		AS_SetEvent,				// �̺�Ʈ �ɱ�
		AS_SetDataProvider,			// ������ ���ι��̴� ����
		AS_RemoveDataProvider,		// ������ ���ι��̴� ����
		AS_GetDataProvider,			// ������ ���ι��̴� ���
		AS_SetDataProviderProp,		// ������ ���ι��̴� ������Ƽ ����
		AS_SetListItemPropArray,	// ����Ʈ ������ ������Ƽ ����
		AS_SetListItemProp,			// ����Ʈ ������ ������Ƽ ����
		AS_CallListItemFunc,		// ����Ʈ ������ �Լ� ȣ��
		AS_CallFunction,			// �Լ� ȣ��
		AS_SetFocus,				// ��Ŀ�� ����
		AS_SetTextFormat,			// �ؽ�Ʈ ���� ����
		AS_SetTween,				// Ʈ�� ����
		AS_HitTestEnable,			// ���콺 ��Ʈ �׽�Ʈ ����
		AS_GetSlotTooltipPosSize,	// ���� ���� ��ġ/������
		AS_SlotDragStop,			// ���� �巡�� ��ž
		AS_GetDragSlotBinding,		// ���� �巡�� ���� ���� ���ε� ID
		AS_GetWidgetInstance,		// �ν��Ͻ� ���
		AS_ResetWidgetPosition,		// UI ��ġ �ʱ�ȭ
		AS_IsOpenTooltip,			// ���� �����ֳ�?
		AS_GetDataProviderArray,	
		AS_SetSpeechBubble,			// ��ǳ�� �ɱ�;
		AS_DelSpeechBubble,			// ��ǳ�� ����;
		AS_SetFakeModal,			// ����ũ ��� ����
		AS_SetDataProvider_Terms,	// ������ ���ι��̴� ����; �����ϴ� ������ ���ι��̴��� ã�� �ش� ������ ������;
		AS_SnapCheck,				// �����Ǵ� ��ǥ ���
		AS_SetUIAlpha,				// ��ü UI ���İ� ����
		AS_GC,						// ������ �÷���
		AS_RemoveTween,
		AS_RegShowTooltipEnableState,
		AS_UnregShowTooltipEnableState,
		AS_UpdateDraggingSlot,		// �巡�� ���� ���� ������Ʈ
		AS_SetFoward,
		AS_SetHeadData,				// Ʈ�� ��� ������ ����
		AS_SetTreeData,				// Ʈ�� ������ ����
		AS_GetHeadData,				// Ʈ�� ��� ������ ���
		AS_GetTreeData,				// Ʈ�� ������ ���
		AS_ChangeHeadData,			// Ʈ�� ��� ������ ����
		AS_ChangeTreeData,			// Ʈ�� ������ ����
		AS_SetTextFormatSize = 49,		// �ؽ�Ʈ ���� ũ�� ����
		AS_Interface_Size,
	};

private :
	LPDIRECT3DDEVICE9	m_pDevice;

	GfxCore*			m_pCore;
	GfxGameScene*		m_pGameScene;
	//GfxLoadingScene*	m_pLoadingScene;
	GfxLoginScene*		m_pLoginScene;
	GfxDebugScene*		m_pDebugScene;

	// ���� ID ����
	VEC_STRING				m_vecRegSlotID;
	// HTML �� ID
	std::map< int, int >	m_mapHtmlWebViewID;
	
	bool m_bUpdatedClubIconRT;
	// ������Ʈ �� Ŭ�� ��ũ ����
	std::queue< SUpdateClubMark > m_queUpdateClubMark;

	bool m_bReload;
	bool m_bShowDebugScene;

	float	m_fTimerGC;		// gc ȣ�� Ÿ�̸�
	bool	m_bCallGC;		// gc ȣ�� �ؾ��ϴ°�?

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
	// ���� ���������� ���ӽ� �ʱ�ȭ ��ų�͵� �ʱ�ȭ��Ų��;
	virtual void InitGameStage(void);

public :
	// Debug Scene Toggle
	virtual void ToggleDebugScene();
	// D3DPRESENT_PARAMETERS ����
	virtual void StorePresentParameters( D3DPRESENT_PARAMETERS pp );
	// �̸� ó���� ������ �޽��� ó��
	virtual void PreProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	// ������ �޽��� ó��, true ��ȯ�� ������ �⺻ ���Ͻ��� ��ŵ
	virtual bool ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public :
	virtual void SetDisplayCountryMark( bool bValue );

	// ��Ʈ ��� ����
	virtual void SetFontLanguage( EMGFX_FONT_LANG emFontLang );

	virtual bool IsMouseInUI();

	virtual const D3DPRESENT_PARAMETERS& GetPresentParams();
	virtual void GetMousePos( int& nMouseX, int& nMouseY );

	// Ű���� �Է� �������� ����
	virtual void SetConversionModeEn();
	// ���� �Է� ���
	CString GetConversionMode();

	// ��Ƽ�ٸ���� on/off
	virtual void SetEdgeAAMode( int nMode );

	// ��� ������ �̸� Ŭ������ �ݱ� ����
	virtual void SetInteractDropItem( bool bEnable );

	virtual std::string GetIMECompositionString(void);

public :
	virtual void Reload();

	// ���� UI ���� �ð� ������ ��ε�
	void CheckDeleteWidget( float fElapsedTime );

public :
	Scaleform::GFx::Value* FindInstance( WORD wWidgetID, WORD wSubID, std::string strInst );

private :
	// AS �Լ� ȣ��
	bool Invoke( EMAS_Interface emInterface, const Scaleform::GFx::Value* pArgs = NULL,
		const int nArgsCount = 0, Scaleform::GFx::Value* pResult = NULL );

	// ��Ʈ�� �迭 ������ ���� ������ ������ �߶���.
	void SplitString( VEC_STRING& vecSplitStr, const char* str, const std::string& strSeparator );
	// ��Ʈ�� �迭 ����
	void CreateStringArray( const char* str, Scaleform::GFx::Value* pValue, const std::string& strSeparator );
	
	// ������Ƽ ���� �Լ� ���� �κ�
	void PreSetProperty( Scaleform::GFx::Value* pArgs, 
		int nWidgetID, int nSubID, const char* strProp, const char* strInstance );
	// ������Ƽ ��� �Լ� ���� �κ�
	EMGFX_OBJECT_TYPE PreGetProperty( Scaleform::GFx::Value* pGfxValue, int nWidgetID, int nSubID,
		const char* strProp, const char* strInstance );
	// ������ ���ι��̴� ��� ���� �κ�
	EMGFX_OBJECT_TYPE PreGetDataProvider( Scaleform::GFx::Value* pGfxValue, int nWidgetID, int nSubID, int nIndex,
		const char* strName, const char* strInstance );
	// ������ ���ι��̴� ������Ƽ ���� ���� �κ�
	void PreSetDataProviderProp( Scaleform::GFx::Value* pArgs, int nWidgetID, int nSubID, int nIndex,
		const char* strName, const char* strInstance );
	// ����Ʈ ������ ������Ƽ ���� �Լ� ���� �κ�
	Scaleform::GFx::Value* PreSetListItemProp( Scaleform::GFx::Value* pArgs,int nWidgetID, int nSubID, int nIndex,
		const char* strInstName, const char* strProp, const char* strInstance );

	// Ʈ�� ������ ����
	void InsertTreeData( Scaleform::GFx::Value* pParentArray, int& nArrayIndex, 
		const VEC_STRING& vecObjectID, LuaTable tbData );

public :
	// UI �����ε�
	virtual void PreloadWidget();

	// UI ��ε�
	virtual void UnloadWidget( int nWidgetID, int nSubID );

	// ���� ID �Ҵ�
	virtual void RegSlot( WORD wType, const char* strSlotID );
	// ���� ID ����
	virtual void UnregSlot( const std::string& strSlotID );
	// HTML View ID �Ҵ�
	virtual void RegHtmlWebView( int nViewID );
	// HTML View ID
	virtual int GetHtmlWebID( int nViewID );
	// HTML View �̵�
	virtual void MoveHtmlWebView( int nViewID, bool bVisible, int nX, int nY, int nWidth, int nHeight );

	// UI ����
	virtual void OpenWidget( int nWidgetID, int nSubID = 0, bool bPreload = false );
	// UI �ݱ�
	virtual void CloseWidget( int nWidgetID, int nSubID = 0 );
	// ��ü UI �ݱ�
	virtual void CloseAllWidget( bool bDelete );
	// UI �����ֳ�?
	virtual bool IsOpenWidget( int nWidgetID, int nSubID = 0 );
	// UI �ε�� �����ΰ�?
	virtual bool IsLoadWidget( int nWidgetID, int nSubID = 0 );
	// ��Ƽ widget ���� ����
	virtual int GetMultiWidgetCount( int nWidgetID );
	// ESC Ű�� ���� �� �ִ� UI �ϳ��� �ݱ�, ������ ������ false
	virtual bool EscWidgetClose();
	// ���� �����ֳ�?
	virtual bool IsOpenTooltip();

	// �������� �̺�Ʈ ����
	virtual void SetStageEvent( const char* strEvent, const char* strCallBack, const char* strRetParams );

	// ������Ƽ ����
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int nValue );
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int nValue );
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double dValue );
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool bValue );
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue );
	virtual void SetProperty_WString( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue );

	// ������Ƽ ����; 
	// �ϳ��� �Ӽۿ� �ټ��� ��ü�� ������ ���Ͽ� ���ϸ� ����Ű�� ���� �����ϱ� ������;
	// virtual void SetProperty_Array( int nWidgetID, int nSubID, const char* strProp, const char* strInstanceArray, const char* strValueArray );
	virtual void SetProperty_Array( int nWidgetID, int nSubID, LuaTable tbProp, LuaTable tbInstance, LuaTable tbValue, LuaTable tbValueType );

	// ������Ƽ ���
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int& nValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int& nValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double& dValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool& bValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, std::string& strValue );
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, LuaTable& tb );

	// �̺�Ʈ �ɱ�
	virtual void SetEvent( int nWidgetID, int nSubID, const char* strEvent, const char* strCallBack,
						   const char* strRetProp, const char* strDelivery, const char* strInstance );

	// ������ ���ι��̴� ����
	virtual void SetDataProvider( int nWidgetID, int nSubID, const char* strDataProvider,const char* strInstance, bool bAdd );
	virtual void SetDataProviderEx( int nWidgetID, int nSubID, const char* strObjectID, const char* strDataProvider,
									const char* strInstance, bool bAdd );
	virtual void SetDataProviderComplex( int nWidgetID, int nSubID, const char* strObjectID, LuaTable tbData,
									const char* strInstance, bool bAdd );
	virtual void SetDataProvider_Terms(int nWidgetID, int nSubID
		, const char* strInstName
		, LuaTable tbWhereProvName, LuaTable tbWhereProvData, LuaTable tbWhereProvType
		, LuaTable tbNewProvName, LuaTable tbNewProvData, LuaTable tbNewProvType);

	// ������ ���ι��̴� ����
	virtual void RemoveDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strInstance, bool bAll );

	// ������ ���ι��̴� ���
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int& nValue );
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int& nValue );
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double& dValue );
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool& bValue );
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, std::string& strValue );

	// ������ ���ι��̴� ���;
	// �ϳ��� ������Ʈ�� �����ϴ°ǵ� ������ �ݵǾ� ���ϸ� ����Ű�� ���� �����ϱ� ������;
	// ��û ������� ���̺� ��ܼ� ��ȯ��;
	virtual bool GetDataProvider_Array( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, LuaTable& tbResult );

	// ������ ���ι��̴� ������Ƽ ����
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int nValue );
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int nValue );
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double dValue );
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool bValue );
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, const char* strValue );

	virtual void SetListItemProp_Array( int nWidgetID, int nSubID, char* strIndexArr, const char* strInstNameArr, const char* strPropArr,
		const char* strValueArr, const char* strTypeArr, const char* strInstance );
	// ����Ʈ ������ ������Ƽ ����
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

	// ����Ʈ ������ �Լ� ȣ��
	virtual void CallListItemFunc( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strFunc,
		const char* strParams, const char* strInstance );

	// �Լ� ȣ��
	virtual void CallFunction( int nWidgetID, int nSubID, const char* strName, const char* strParams, const char* strInstance );
	// ��Ŀ�� ����
	virtual void SetFocus( int nWidgetID, int nSubID, const char* strInstance );
	// �ؽ�Ʈ �÷� ����
	virtual void SetTextFormat( int nWidgetID, int nSubID, DWORD dwColor, bool bUnderline,
		int nBeginIndex, int nEndIndex, const char* strInstance );
	// �ؽ�Ʈ ������ ����
	virtual void SetTextFormatSize( int nWidgetID, int nSubID, DWORD dwSize, const char* strInstance );
	// Ʈ�� ����
	virtual void SetTween( int nWidgetID, int nSubID, int nTime, int nDelay, int nEase, bool bLoop, const char* strComplete,
		const char* strProp, const char* strValue, const char* strInstance );

	virtual void RemoveTween( int nWidgetID, int nSubID, const char* strInstance);

	// ���콺 ��Ʈ �׽�Ʈ ����
	virtual void HitTestEnable( int nWidgetID, int nSubID, bool bEnable, const char* strInstance );

	// ���� ���� ��ġ/������
	virtual void GetSlotTooltipPosSize( int& nX, int& nY, int& nWidth, int& nHeight );

	// UI ��ġ �ʱ�ȭ
	virtual void ResetWidgetPosition();

	// ��ǳ�� �߰�
	virtual void SetSpeechBubble(int nWidgetID, int nSubID, const char* strInstance, const char* strText);
	// ��ǳ�� ����
	virtual void DelSpeechBubble(int nWidgetID, int nSubID, const char* strInstance);
	// ����ũ ��� ����
	virtual void SetFakeModal( bool bValue );
	// ���� �Ǵ� ��ǥ
	virtual void SnapCheck( int nWidgetID, int nSubID, int nX, int nY, int nWidth, int nHeight, int& nRetX, int& nRetY );
	// ��ü UI ���İ�
	virtual void SetUIAlpha( float fAlpha );
	// Ŭ�� ��ũ ������Ʈ
	virtual void UpdateClubMark( int nWidgetID, int nSubID, const char* szParam, const char* szInst );
	// Ȧ�� ���� ������Ʈ
	virtual void UpdateHoldSlot();
	// UI ���� �Ǿ�����
	virtual void SetFoward( int nWidgetID, int nSubID );

	// Ʈ�� ��� ������
	virtual void SetHeadData( int nWidgetID, int nSubID, const char* strObjectID,
		LuaTable tbData, const char* strInstance );
	virtual void GetHeadData( int nWidgetID, int nIndex, const char* szInst, int nSubID, LuaTable& tb );
	virtual void ChangeHeadData( int nWidgetID, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID );

	// Ʈ�� ������
	virtual void SetTreeData( int nWidgetID, int nSubID, LuaTable tbKey, const char* strObjectID, 
		LuaTable tbData, const char* strInstance );
	virtual void GetTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szInst, int nSubID, LuaTable& tb );
	virtual void ChangeTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID );

public :
	// ä�� �޽��� �߰�
	virtual void AddChatMessage( int nLogID, const std::string& strMsg, DWORD dwColor, 
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName );
	// ��ũ �޽��� �߰�
	virtual void AddLinkMessage( int nLogID, const std::string& strMsg, DWORD dwColor,
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		VEC_LINK_TEXT_DATA& vecLinkTextData,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName );

public :
	// ���� �巡�� ��ž
	virtual void SlotDragStop();

	// ���� ������
	virtual bool GetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem );
	virtual void SetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem );

	// ���� ������ ����
	virtual void ResetSlot( const char* strSlotID );
	// ������ ���� ������ ����
	virtual void SetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID );
	virtual void SetItemSlotData_NonStrengthen( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID );
	virtual void SetItemSlotData_BORNTIME( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, DWORD dwBornT );
	// ��ų ���� ������ ����
	virtual void SetSkillSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nLevel = -1 );
	// �κ��丮 ���� ������ ����
	virtual void SetInventorySlotData( const char* strSlotID, int nPosX, int nPosY );
	// �� ���� ���� ������ ����
	virtual void SetRebuySlotData( const char* strSlotID, int nRebuyID, WORD wMID, WORD wSID );
	// ��ȯ�� ������ ����;
	virtual void SetSummonStateSlotData( const char* strSlotID, const char* strIconName, INT nIconIdxX, INT nIconIdxY );
	// ���� ���� ������ ����
	virtual void SetBankSlotData( const char* strSlotID, int nPosX, int nPosY );
	// ���ø���Ʈ ���� ������ ����
	virtual void SetWishlistSlotData( const char* strSlotID, int nPosX, int nPosY );
	// ĳ���� ���� ���� ������ ����
	virtual void SetCharInfoSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, int nSlot );
	// ��ų ������ ����
	virtual void SetSkillQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex );
	// ������ ������ ����
	virtual void SetItemQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex );
	// �̴ϰ��� : �ֻ��� ���� ���� ����;
	virtual void SetItemMiniGameDiceRewardSlotData( const char* strSlotID, DWORD dwID, int nIndex );
	// NPC ��ȭ ���� ���� ����
	virtual void SetNpcMarketSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY );
	// NPC ��ȭ ���� īƮ ���� ����
	virtual void SetNpcMarketCartSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, WORD wIndex );
	// �ŷ�â ���� ����
	virtual void SetTradeSlotData(const char* strSlotID, int nPosX, int nPosY, bool _MyBox);
	// PointShop ���� ����
	virtual void SetPointShopSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY );
	// PointShop īƮ ���� ����
	virtual void SetPointShopCartSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nIndex );
	//��������â ���� ����
	virtual void SetReferSlotData( const char* strSlotID, int nSlot );
	// ���λ����˻� ����Ʈ�� ������ ��� ����
	virtual void SetPMSPointTradeSlotData( const char* strSlotID, WORD wPosX, WORD wPosY );
	// ���λ����˻� ����Ʈ�� �ε��� ����
	virtual void SetPMSPointTradeSlotData_Index( const char* strSlotID, DWORD dwIndex, DWORD dwCount );
	// ���λ��� ������ ��� ����
	virtual void SetPrivateMarketSlotData( const char* strSlotID, int nPosX, int nPosY, bool bSelf );
	// ���λ����˻� ����
	virtual void SetPrivateMarketSearchSlotData( const char* strSlotID, DWORD dwIndex, WORD wCount, bool bSearchPage );
	// ���� ����
	virtual void SetReceivePostSlotData( const char* strSlotID, DWORD dwIndex, DWORD dwCount );
	// PostBoxSub_PreviewPageOpen
	virtual void SetSlot_PostBoxSub_PreviewPageOpen( const char* strSlotID );
	// �� ������ ���� ���� ����
	virtual void SetPetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD nSlot );
	// Ŭ�� â�� ���� ����
	virtual void SetClubStorageSlotData( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY);
	// Ŭ�� â�� ������ �־��ֱ�
	virtual void SetClubStorageItem(const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY);
	// ��Ŀ ���� ����
	virtual void SetLockerSlot( const char* strSlotID, int nIndex, int nPosX, int nPosY );
	// ���� ī�� ���� ����
	virtual void SetRebuildCardSlot( const char* strSlotID, int nPosX, int nPosY );
	// ��Ƽ �й� ���� ����;
	virtual void SetItemSlotData_PartyDistribution( const char* strSlotID, DWORD dwItemID, WORD wSlotIndex);
	// ������ ���� ���� ����
	virtual void SetItemRepairSlotData( const char* strSlotID, int nSlot );
	// �ζ� ��÷ ������ ���� ���� ����;
	virtual void SetLottoWinItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID );
	// �ζ� ���� ��÷�� ����Ʈ ��÷ ������ ��� ���� ����;
	virtual void SetLottoPreWinListItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID, int nRanking );
	// �� ����� ���� ���� ����
	virtual void SetRanMobileRewardItemSlotData( const char* strSlotID, int nIndex );

public :
	// ���� �巡������ ��ų ������ ��ų ID
	virtual SNATIVEID GetDragSlotSkillID();

	// ��ų ���� ��ٿ�
	virtual void SetSkillSlotCoolDown( const SNATIVEID& sID );
	// ������ ���� ��ٿ�
	virtual void SetItemSlotCoolDown( const SNATIVEID& sID );

public :
	// �̸� ���÷��� �߰�
	virtual void AddNameDisplay( CROWREN& sDispName, const char* szOwnerName = NULL );
	// �̸� ���÷��� Ŭ����
	virtual void ClearDisplay();

	// ���λ��� ���÷��� ����
	virtual void DeletePrivateMarketDisplay( DWORD dwID );

	// ����� ���÷���
	virtual void AddHeadChatDisplay( const std::string& strName, DWORD dwNameColor,
		const std::string& strChat, DWORD dwColor, const D3DXVECTOR2& vPos ); 
	// ������ ���÷���
	virtual void AddDamageDisplay( const D3DXVECTOR3& vPos, int nDamage, 
		DWORD dwDamageFlag, BOOL bAttack, const STARGETID& sCauser, const STARGETID& sTarget );
	// ���λ��� ���÷���
	virtual void AddPrivateMarketDisplay( const CROWREN& sDispName );

	// ���λ��� ���÷��� ��� ���� ����
	virtual void ToggleOffPrivateMarketDisplay( DWORD dwID );

	// �̸� ���÷��� Ŭ�� ������ ����
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
