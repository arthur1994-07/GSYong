#pragma once

#include "../../RanGfxUI/GfxDef.h"
#include "../../RanLogic/GLChatLinkDef.h"

#include "./Chat/GLChatMan.h"
#include "./Crow/GLCrowRenList.h"
#include "./Widget/GLWidgetScript.h"

// �������� ��� (������ WidgetDef.lua ���ϵ� ���� ����)
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

	// ��Ʈ ���
	EMGFX_FONT_LANG m_emFontLang;

	int m_nStageBG;
	int m_nPrevStageBG;

	bool m_bMouseHitUI;			// ���콺 �Է��� UI�� ��Ʈ �ߴ��� Ȯ��
	bool m_bIsDragging;			// ���콺 �巡�� ������ Ȯ��
	bool m_bIsFocusInTextInput;	// �ؽ�Ʈ ��ǲ�� ��Ŀ�� ���ֳ�?

	bool m_bDisplayOn;
	bool m_bDisplayCountryMark;	// �̸� ���÷��̿� ���� �̹��� ���?

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
	// ���� ���������� ���ӽ� �ʱ�ȭ ��ų�͵� �ʱ�ȭ��Ų��;
	virtual void InitGameStage(void) = 0;

public :
	// Debug Scene Toggle
	virtual void ToggleDebugScene() = 0;
	// D3DPRESENT_PARAMETERS ����
	virtual void StorePresentParameters( D3DPRESENT_PARAMETERS pp ) = 0;
	// �̸� ó���� ������ �޽��� ó��
	virtual void PreProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) = 0;
	// ������ �޽��� ó��, true ��ȯ�� ������ �⺻ ���Ͻ��� ��ŵ
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

	// ���� UI ��Ŀ���� �ؽ�Ʈ ��ǲ�� �ִ°�?
	void SetFocusInTextInput( bool bValue ) { m_bIsFocusInTextInput = bValue; }
	bool GetFocusInTextInput() { return m_bIsFocusInTextInput; }

	GLGaeaClient* GetGaeaClient();

	HWND GetHWND() { return m_hWnd; }

	// ��Ʈ ��� ����
	virtual void SetFontLanguage( EMGFX_FONT_LANG emFontLang ) = 0;
	EMGFX_FONT_LANG GetFontLanguage() { return m_emFontLang; }
	virtual const D3DPRESENT_PARAMETERS& GetPresentParams() = 0;

	virtual void GetMousePos( int& nMouseX, int& nMouseY ) = 0;

	// Ű���� �Է� �������� ����
	virtual void SetConversionModeEn() = 0;

	// ��Ƽ�ٸ���� on/off
	virtual void SetEdgeAAMode( int nMode ) = 0;

	// ��� ������ �̸� Ŭ������ �ݱ� ����
	virtual void SetInteractDropItem( bool bEnable ) = 0;

	virtual std::string GetIMECompositionString(void) = 0;

public :
	virtual void Reload() = 0;

public :
	// UI �����ε�
	virtual void PreloadWidget() = 0;

	// UI ��ε�
	virtual void UnloadWidget( int nWidgetID, int nSubID ) = 0;

	// ���� ID �Ҵ�
	virtual void RegSlot( WORD wType, const char* strSlotID ) = 0;
	// ���� ID ����
	virtual void UnregSlot( const std::string& strSlotID ) = 0;
	// HTML View ID �Ҵ�
	virtual void RegHtmlWebView( int nViewID ) = 0;
	// HTML View ID
	virtual int GetHtmlWebID( int nViewID ) = 0;
	// HTML View �̵�
	virtual void MoveHtmlWebView( int nViewID, bool bVisible, int nX, int nY, int nWidth, int nHeight ) = 0;
	
	// UI ����
	virtual void OpenWidget( int nWidgetID, int nSubID = 0, bool bPreload = false ) = 0;
	// UI �ݱ�
	virtual void CloseWidget( int nWidgetID, int nSubID = 0 ) = 0;
	// ��ü UI �ݱ�
	virtual void CloseAllWidget( bool bDelete ) = 0;
	// UI �����ֳ�?
	virtual bool IsOpenWidget( int nWidgetID, int nSubID = 0 ) = 0;
	// UI �ε�� �����ΰ�?
	virtual bool IsLoadWidget( int nWidgetID, int nSubID = 0 ) = 0;
	// ��Ƽ widget ���� ����
	virtual int GetMultiWidgetCount( int nWidgetID ) = 0;
	// ESC Ű�� ���� �� �ִ� UI �ϳ��� �ݱ�, ������ ������ false
	virtual bool EscWidgetClose() = 0;
	// ���� �����ֳ�?
	virtual bool IsOpenTooltip() = 0;

	// �������� �̺�Ʈ ����
	virtual void SetStageEvent( const char* strEvent, const char* strCallBack, const char* strRetParams ) = 0;

	// ������Ƽ ����
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int nValue ) = 0;
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int nValue ) = 0;
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double dValue ) = 0;
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool bValue ) = 0;
	virtual void SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue ) = 0;
		virtual void SetProperty_WString( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue ) = 0;
	// virtual void SetProperty_Array( int nWidgetID, int nSubID, const char* strProp, const char* strInstanceArray, const char* strValueArray ) = 0;
	virtual void SetProperty_Array( int nWidgetID, int nSubID, LuaTable tbProp, LuaTable tbInstance, LuaTable tbValue, LuaTable tbValueType) = 0;

	// ������Ƽ ���
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int& nValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int& nValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double& dValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool& bValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, std::string& strValue ) = 0;
	virtual bool GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, LuaTable& tb ) = 0;

	// �̺�Ʈ �ɱ�
	virtual void SetEvent( int nWidgetID, int nSubID, const char* strEvent, const char* strCallBack, 
						   const char* strRetProp, const char* strDelivery, const char* strInstance ) = 0;

	// ������ ���ι��̴� ����
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

	// ������ ���ι��̴� ����
	virtual void RemoveDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strInstance, bool bAll ) = 0;

	// ������ ���ι��̴� ���
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int& nValue ) = 0;
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int& nValue ) = 0;
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double& dValue ) = 0;
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool& bValue ) = 0;
	virtual bool GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, std::string& strValue ) = 0;
	virtual bool GetDataProvider_Array( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, LuaTable& tbResult ) = 0;
	
	// ������ ���ι��̴� ������Ƽ ����
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int nValue ) = 0;
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int nValue ) = 0;
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double dValue ) = 0;
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool bValue ) = 0;
	virtual void SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, const char* strValue ) = 0;

	virtual void SetListItemProp_Array( int nWidgetID, int nSubID, char* strIndexArr, const char* strInstNameArr, const char* strPropArr,
		const char* strValueArr, const char* strTypeArr, const char* strInstance ) = 0;
	// ����Ʈ ������ ������Ƽ ����
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

	// ����Ʈ ������ �Լ� ȣ��
	virtual void CallListItemFunc( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strFunc,
		const char* strParams, const char* strInstance ) = 0;

	// �Լ� ȣ��
	virtual void CallFunction( int nWidgetID, int nSubID, const char* strName, const char* strParams, const char* strInstance ) = 0;
	// ��Ŀ�� ����
	virtual void SetFocus( int nWidgetID, int nSubID, const char* strInstance ) = 0;
	// �ؽ�Ʈ �÷� ����
	virtual void SetTextFormat( int nWidgetID, int nSubID, DWORD dwColor, bool bUnderline,
		int nBeginIndex, int nEndIndex, const char* strInstance ) = 0;
	// �ؽ�Ʈ �÷� ����
	virtual void SetTextFormatSize( int nWidgetID, int nSubID, DWORD dwSize, const char* strInstance ) = 0;
	// Ʈ�� ����
	virtual void SetTween( int nWidgetID, int nSubID, int nTime, int nDelay, int nEase, bool bLoop, const char* strComplete,
		const char* strProp, const char* strValue, const char* strInstance ) = 0;

	virtual void RemoveTween( int nWidgetID, int nSubID, const char* strInstance ) = 0;
	
	// ���콺 ��Ʈ �׽�Ʈ ����
	virtual void HitTestEnable( int nWidgetID, int nSubID, bool bEnable, const char* strInstance ) = 0;

	// ���� ���� ��ġ/������
	virtual void GetSlotTooltipPosSize( int& nX, int& nY, int& nWidth, int& nHeight ) = 0;

	// UI ��ġ �ʱ�ȭ
	virtual void ResetWidgetPosition() = 0;

	// ��ǳ�� �߰�
	virtual void SetSpeechBubble(int nWidgetID, int nSubID, const char* strInstance, const char* strText) = 0;
	// ��ǳ�� ����
	virtual void DelSpeechBubble(int nWidgetID, int nSubID, const char* strInstance) = 0;
	// ����ũ ��� ����
	virtual void SetFakeModal( bool bValue ) = 0;
	// ���� �Ǵ� ��ǥ
	virtual void SnapCheck( int nWidgetID, int nSubID, int nX, int nY, int nWidth, int nHeight, int& nRetX, int& nRetY ) = 0;
	// ��ü UI ���İ�
	virtual void SetUIAlpha( float fAlpha ) = 0;
	// Ŭ�� ��ũ ������Ʈ
	virtual void UpdateClubMark( int nWidgetID, int nSubID, const char* szParam, const char* szInst ) = 0;
	// Ȧ�� ���� ������Ʈ
	virtual void UpdateHoldSlot() = 0;
	// UI ���� �Ǿ�����
	virtual void SetFoward( int nWidgetID, int nSubID ) = 0;

	// Ʈ�� ��� ������
	virtual void SetHeadData( int nWidgetID, int nSubID, const char* strObjectID,
		LuaTable tbData, const char* strInstance ) = 0;
	virtual void GetHeadData( int nWidgetID, int nIndex, const char* szInst, int nSubID, LuaTable& tb ) = 0;
	virtual void ChangeHeadData( int nWidgetID, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID ) = 0;

	// Ʈ�� ������
	virtual void SetTreeData( int nWidgetID, int nSubID, LuaTable tbKey, const char* strObjectID, 
		LuaTable tbData, const char* strInstance ) = 0;
	virtual void GetTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szInst, int nSubID, LuaTable& tb ) = 0;
	virtual void ChangeTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID ) = 0;

public :
	// ä�� �޽��� �߰�
	virtual void AddChatMessage( int nLogID, const std::string& strMsg, DWORD dwColor, 
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName ) = 0;
	// ��ũ �޽��� �߰�
	virtual void AddLinkMessage( int nLogID, const std::string& strMsg, DWORD dwColor,
		const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
		VEC_LINK_TEXT_DATA& vecLinkTextData,
		const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName ) = 0;

public :
	// ���� �巡�� ��ž
	virtual void SlotDragStop() = 0;

	// ���� ������
	virtual bool GetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem ) = 0;
	virtual void SetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem ) = 0;

	// ���� ������ ����
	virtual void ResetSlot( const char* strSlotID ) = 0;
	// ������ ���� ������ ����
	virtual void SetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID ) = 0;
	virtual void SetItemSlotData_NonStrengthen( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID ) = 0;
	virtual void SetItemSlotData_BORNTIME( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, DWORD dwBornT ) = 0;
	// ��ų ���� ������ ����
	virtual void SetSkillSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nLevel = -1 ) = 0;
	// �κ��丮 ���� ������ ����
	virtual void SetInventorySlotData( const char* strSlotID, int nPosX, int nPosY ) = 0;
	// �� ���� ���� ������ ����
	virtual void SetRebuySlotData( const char* strSlotID, int nRebuyID, WORD wMID, WORD wSID ) = 0;
	// ��ȯ�� ������ ����;
	virtual void SetSummonStateSlotData( const char* strSlotID, const char* strIconName, INT nIconIdxX, INT nIconIdxY ) = 0;
	// ���� ���� ������ ����
	virtual void SetBankSlotData( const char* strSlotID, int nPosX, int nPosY ) = 0;
	// ���ø���Ʈ ���� ������ ����
	virtual void SetWishlistSlotData( const char* strSlotID, int nPosX, int nPosY ) = 0;
	// ĳ���� ���� ���� ������ ����
	virtual void SetCharInfoSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, int nSlot ) = 0;
	// ��ų ������ ����
	virtual void SetSkillQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex ) = 0;
	// ������ ������ ����
	virtual void SetItemQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex ) = 0;
	// �̴ϰ��� : �ֻ��� ���� ���� ����;
	virtual void SetItemMiniGameDiceRewardSlotData( const char* strSlotID, DWORD dwID, int nIndex ) = 0;
	// NPC ��ȭ ���� ���� ����
	virtual void SetNpcMarketSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY ) = 0;
	// NPC ��ȭ ���� īƮ ���� ����
	virtual void SetNpcMarketCartSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, WORD wIndex ) = 0;
	// �ŷ�â ���� ����
	virtual void SetTradeSlotData(const char* strSlotID, int nPosX, int nPosY, bool _MyBox) = 0;
	// PointShop ���� ����
	virtual void SetPointShopSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY ) = 0;
	// PointShop īƮ ���� ����
	virtual void SetPointShopCartSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nIndex ) = 0;
	// ��������â ���� ������ ����
	virtual void SetReferSlotData( const char* strSlotID, int nSlot ) = 0;
	// ���λ����˻� ����Ʈ�� ������ ��� ����
	virtual void SetPMSPointTradeSlotData( const char* strSlotID, WORD wPosX, WORD wPosY ) = 0;
	// ���λ����˻� ����Ʈ�� �ε��� ����
	virtual void SetPMSPointTradeSlotData_Index( const char* strSlotID, DWORD dwIndex, DWORD dwCount ) = 0;
	// ���λ��� ����
	virtual void SetPrivateMarketSlotData( const char* strSlotID, int nPosX, int nPosY, bool bSelf ) = 0;
	// ���λ����˻� ����
	virtual void SetPrivateMarketSearchSlotData( const char* strSlotID, DWORD dwIndex, WORD wCount, bool bSearchPage ) = 0;
	// ���� ����
	virtual void SetReceivePostSlotData( const char* strSlotID, DWORD dwIndex, DWORD dwCount ) = 0;	
	// PostBoxSub_PreviewPageOpen
	virtual void SetSlot_PostBoxSub_PreviewPageOpen( const char* strSlotID ) = 0;
	// �� ������ ���� ���� ����
	virtual void SetPetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD nSlot ) = 0;
	// Ŭ��â�� â ���� ����
	virtual void SetClubStorageSlotData( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY) = 0;
	virtual void SetClubStorageItem(const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY) = 0;
	// ��Ŀ ���� ����
	virtual void SetLockerSlot( const char* strSlotID, int nIndex, int nPosX, int nPosY ) = 0;
	// ���� ī�� ���� ����
	virtual void SetRebuildCardSlot( const char* strSlotID, int nPosX, int nPosY ) = 0;
	// ��Ƽ �й� ���� ����;
	virtual void SetItemSlotData_PartyDistribution( const char* strSlotID, DWORD dwItemID, WORD wSlotIndex) = 0;
	// ������ ���� ���� ����
	virtual void SetItemRepairSlotData( const char* strSlotID, int nSlot ) = 0;
	// �ζ� ��÷ ������ ���� ���� ����;
	virtual void SetLottoWinItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID ) = 0;
	// �ζ� ���� ��÷�� ����Ʈ ��÷ ������ ��� ���� ����;
	virtual void SetLottoPreWinListItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID, int nRanking ) = 0;
	// �� ����� ���� ���� ����
	virtual void SetRanMobileRewardItemSlotData( const char* strSlotID, int nIndex ) = 0;

public :
	// ���� �巡������ ��ų ������ ��ų ID
	virtual SNATIVEID GetDragSlotSkillID() = 0;

	// ��ų ���� ��ٿ�
	virtual void SetSkillSlotCoolDown( const SNATIVEID& sID ) = 0;
	// ������ ���� ��ٿ�
	virtual void SetItemSlotCoolDown( const SNATIVEID& sID ) = 0;

public :
	// ä�� ����Ʈ UI�� ���
	void PrintChatList( int nLogID, LIST_CHAT& listChat );

public :
	// �̸� ���÷��� �߰�
	virtual void AddNameDisplay( CROWREN& sDispName, const char* szOwnerName = NULL ) = 0;
	// �̸� ���÷��� Ŭ����
	virtual void ClearDisplay() = 0;

	// ���λ��� ���÷���
	virtual void AddPrivateMarketDisplay( const CROWREN& sDispName ) = 0;
	
	// ���λ��� ���÷��� ����
	virtual void DeletePrivateMarketDisplay( DWORD dwID ) = 0;

	// ����� ���÷���
	virtual void AddHeadChatDisplay( const std::string& strName, DWORD dwNameColor,
		const std::string& strChat, DWORD dwColor, const D3DXVECTOR2& vPos ) = 0; 
	// ������ ���÷���
	virtual void AddDamageDisplay( const D3DXVECTOR3& vPos, int nDamage, 
		DWORD dwDamageFlag, BOOL bAttack, const STARGETID& sCauser, const STARGETID& sTarget ) = 0;

	// ���λ��� ���÷��� ��� ���� ����
	virtual void ToggleOffPrivateMarketDisplay( DWORD dwID ) = 0;

	// �̸� ���÷��� Ŭ�� ������ ����
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
