#pragma once

#include "../../SigmaCore/Lua/lua_tinker.h"
#include "../../SigmaCore/Lua/MinLua.h"

extern "C"
{
#include "../../=LuaPlus/src/LuaPlus/src/lua.h"
#include "../../=LuaPlus/src/LuaPlus/src/lualib.h"
#include "../../=LuaPlus/src/LuaPlus/src/lauxlib.h"
}

#include "./GLWidgetScriptDef.h"
#include "../../RanLogic/Land/GLMapAxisInfo.h"

typedef lua_tinker::table LuaTable;

class SUBPATH;
class GLGaeaClient;
class GfxInterfaceBase;
class GLLevelFileClient;

#define INVALID_PARAM( condition, log )				{ if( true == condition ) { PrintDebug( log ); return; } }
#define INVALID_PARAM_VAL( condition, log, val )	{ if( true == condition ) { PrintDebug( log ); return val; } }

#define NULL_INTERFACE_RETURN						{ if( NULL == GLWidgetScript::GetInstance().m_pInterface ) return; }
#define NULL_INTERFACE_RETURN_VALUE( val )			{ if( NULL == GLWidgetScript::GetInstance().m_pInterface ) return val; }

#define SET_LUA_DEF( name, val )					lua_tinker::set( m_pLuaState, name, val );
#define GET_LUA_DEF( type, name )					lua_tinker::get< type >( m_pLuaState, name );

#define REG_LUA_FUNC( funcName, fun )				{ lua_tinker::def( m_pLuaState, funcName, fun ); }

#define REG_LUA_CLASS_NAME( name, type )			{ lua_tinker::class_add< type >( m_pLuaState, name ); }
#define REG_LUA_CLASS_FUNC( type, funcName, fun )	{ lua_tinker::class_def< type >( m_pLuaState, funcName, fun ); }
#define REG_LUA_CLASS_VAL( type, valName, val )		{ lua_tinker::class_mem< type >( m_pLuaState, valName, val ); }
#define REG_LUA_CLASS( name, class )				{ lua_tinker::set( m_pLuaState, name, class ); }

#define IS_LUATABLE( tb )							true == lua_istable( tb.m_obj->m_L, -1 )
#define IS_LUATABLEP( tb )							true == lua_istable( tb->m_obj->m_L, -1 )

namespace NSWIDGET_MAP
{
	void GetLevelClient( GLLevelFileClient** pLevelFile, DWORD dwMapID, bool& bNewLevel );

	// ���� ��ǥ�� �̴ϸ� ��ǥ�� ��ȯ
	void ConvertWorldToMap( const GLMapAxisInfo& sMapAxisInfo,
		const D3DXVECTOR3& vPosition, int& nPosX, int& nPosY );

	// �̴ϸ� �̹��� ��ġ�� ���� ��ǥ�� ��ȯ
	void ConvertImgMapToWorld( const GLMapAxisInfo& sMapAxisInfo,
		int nPosX, int nPosY, D3DXVECTOR3& vPosition );

	// �̴ϸ� �̹��� ��ġ�� �̴ϸ� ��ǥ�� ��ȯ
	void ConvertImgToMapPos( const GLMapAxisInfo& sMapAxisInfo,
		int nPosX, int nPosY, int& nMapPosX, int& nMapPosY );
}

class GLWidgetCallFuncValue
{
public :
	GLWidgetCallFuncValue()
		: pTbValue( NULL )
	{
	}
	~GLWidgetCallFuncValue()
	{
	}

public :
	enum EMVALUE_TYPE
	{
		EMTYPE_NULL,
		EMTYPE_INT,
		EMTYPE_UINT,
		EMTYPE_DWORD,
		EMTYPE_DOUBLE,
		EMTYPE_BOOL,
		EMTYPE_STRING,
		EMTYPE_TABLE,
	};

public :
	EMVALUE_TYPE		emType;

	int					nValue;
	unsigned int		unValue;
	DWORD				dwValue;
	double				dValue;
	bool				bValue;
	std::string			strValue;
	lua_tinker::table*	pTbValue;

public :
	void SetNull(void)			{ emType = EMTYPE_NULL; }
	void SetInt( int val )					 { emType = EMTYPE_INT; nValue = val; }
	void SetUInt( unsigned int val )		 { emType = EMTYPE_UINT; unValue = val; }
	void SetDWORD( DWORD val )				 { emType = EMTYPE_DWORD; dwValue = val; }
	void SetDouble( double val )			 { emType = EMTYPE_DOUBLE; dValue = val; }
	void SetBool( bool val )				 { emType = EMTYPE_BOOL; bValue = val; }
	void SetString( const std::string& val ) { emType = EMTYPE_STRING; strValue = val; }
	void SetTable( lua_tinker::table* pVal ) { emType = EMTYPE_TABLE; pTbValue = pVal; }
};
typedef std::vector< GLWidgetCallFuncValue >		VEC_WIDGET_CALLFUNCVALUE;
typedef VEC_WIDGET_CALLFUNCVALUE::const_iterator	VEC_WIDGET_CALLFUNCVALUE_CITER;

class GLWidgetScript
{
private :
	GLWidgetScript(void);
	GLWidgetScript( const GLWidgetScript& value );
	~GLWidgetScript(void);

public :
	static GLWidgetScript& GetInstance();

public :
	GLGaeaClient*		m_pGaeaClient;
	GfxInterfaceBase*	m_pInterface;

private :
	SUBPATH*			m_pPath;

	std::string			m_strScriptPath;
	bool				m_bReload;

	bool				m_bInitialize;

private :
	lua_State* m_pLuaState;

public :
	void Reload();

	HRESULT Init( GLGaeaClient* pClient, GfxInterfaceBase* pInterface, SUBPATH* pPath );
	void OnFrameMove( float fElapsedTime );

private :
	// ���� ����Ʈ, ��ũ��Ʈ �ε�
	HRESULT LoadFile( const SUBPATH* pPath );

	// wsf ������ �������� WidgetScript ���� �ε��Ѵ�.
	// �Ѿ�� pZipFile �� �� �Լ��ȿ��� ������ fclose()�� �ȴ�.
	// pZipFile			: Zip ����
	// strPathWSF_ZIP	: Zip ������ Full Path.
	BOOL LoadMemory( FILE* pZipFile, const std::string& strPathWSF_ZIP );

	// LoadMemory ������ ȣ��ȴ�.
	BOOL LoadFileInZip( const std::string& strPathWSF_ZIP );

	// ��� ������ �� �ε�
	void LoadDefine();

	// ��� ���� ���� �ʱ�ȭ
	void InitGlobal();
	// ��ƿ��� ����� �Լ� ���
	void RegFunc();

public :
	// ��� �Լ� ȣ��
	// fmt (-n : int) (-dw : DWORD) (-l : LONGLONG) (-f : double) (-s : char*) (-b : bool) 
	template< typename T >
	T LuaCallFunc( const char* szName, const char* fmt = "", ... );
	template< typename T >
	T LuaCallFunc( const char* szName, const VEC_WIDGET_CALLFUNCVALUE& vecValue );
	template< typename T >
	T LuaCallFunc( const char* szName, const GLWidgetCallFuncValue* pArgs, int nArgCount );

	// ��� ��� ����
	void LuaDoCommand( const char* strCommand );

private:
	void recordPf(const char* szName);

public :
	lua_State* GetLuaState() { return m_pLuaState; }

public :
	// ����� ������ ���� ���
	static void RegSlot( WORD wType, const char* strSlotID );
	// HTML View ID �Ҵ�
	static void RegHtmlWebView( int nViewID );
	// HTML View �̵�
	static void MoveHtmlWebView( int nViewID, bool bVisible, int nX, int nY, int nWidth, int nHeight );

	// widget ����
	static void OpenWidget( int nWidgetID, int nSubID = 0 );
	// widget �ݱ�
	static void CloseWidget( int nWidgetID, int nSubID = 0 );
	// ��ü widget �ݱ�
	static void CloseAllWidget( bool bDelete );
	// Widget �����ֳ�?
	static bool IsOpenWidget( int nWidgetID, int nSubID = 0 );
	// Widget �ε�� �����ΰ�?
	static bool IsLoadWidget( int nWidgetID, int nSubID = 0 );
	// ��Ƽ Widget ���� ����
	static int GetMultiWidgetCount( int nWidgetID );
	// ESCŰ�� ���� �� �ִ� UI �ϳ��� �ݱ�, ������ ������ false
	static bool EscWidgetClose();
	// ���� �����ֳ�?
	static bool IsOpenTooltip();

	// �������� �̺�Ʈ ����
	static void SetStageEvent( const char* strEvent, const char* strCallBack, const char* strRetParams );

	// ��Ģ����
	static __int64 Calculate_Plus( __int64 nVal1, __int64 nVal2 );
	static __int64 Calculate_Minus( __int64 nVal1, __int64 nVal2 );
	static __int64 Calculate_Multiply( __int64 nVal1, __int64 nVal2 );
	static __int64 Calculate_Divide( __int64 nVal1, __int64 nVal2 );

	// ������Ƽ ����
	static void SetInt( int nWidgetID, const char* strProp, int nValue, const char* strInstance, int nSubID = 0 );
	static void SetUInt( int nWidgetID, const char* strProp, unsigned int nValue, const char* strInstance, int nSubID = 0 );
	static void SetDouble( int nWidgetID, const char* strProp, double dValue, const char* strInstance, int nSubID = 0 );
	static void SetBool( int nWidgetID, const char* strProp, bool bValue, const char* strInstance, int nSubID = 0 );
	static void SetString( int nWidgetID, const char* strProp, const char* strValue, const char* strInstance, int nSubID = 0 );
	static void SetWString( int nWidgetID, const char* strProp, const char* strValue, const char* strInstance, int nSubID = 0 );
	// static void SetProperty_array( int nWidgetID, const char* strProp, const char* strValue, const char* strInstance, int nSubID = 0 );
	static void SetArray( int nWidgetID, LuaTable tbProp, LuaTable tbValue, LuaTable tbInstance, LuaTable tbValueType, int nSubID = 0 );

	// ������Ƽ ���
	static int GetInt( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static unsigned int GetUInt( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static double GetDouble( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static bool GetBool( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static std::string GetString( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static LuaTable GetArray( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );

	// �̺�Ʈ �ɱ�
	static void SetEvent( int nWidgetID, const char* strEvent, const char* strCallBack, 
		const char* strRetProp, const char* strInstance, const char* strDelivery = NULL, int nSubID = 0 );

	// ������ ���ι��̴� ����
	static void SetData( int nWidgetID, const char* strDataProvider, const char* strInstance, bool bAdd, int nSubID = 0 );
	static void SetDataEx( int nWidgetID, const char* strObjectID, LuaTable tbData,
								   const char* strInstance, bool bAdd, int nSubID = 0 );
	static void SetDataComplex( int nWidgetID, const char* strObjectID, LuaTable tbData,
										const char* strInstance, bool bAdd, int nSubID = 0 );
	static void SetDataTerms(int nWidgetID, const char* strInstName
		, LuaTable tbWhereProvName, LuaTable tbWhereProvData, LuaTable tbWhereProvType
		, LuaTable tbNewProvName, LuaTable tbNewProvData, LuaTable tbNewProvType);//, int nSubID = 0);
	// ������ ���ι��̴� ����
	static void RemoveData( int nWidgetID, int nIndex, const char* strInstance, bool bAll, int nSubID = 0 );

	// ������ ���ι��̴� ���
	static int GetDataInt( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static unsigned int GetDataUInt( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static double GetDataDouble( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static bool GetDataBool( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static std::string GetDataString( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static LuaTable GetDataArray( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	
	// ������ ���ι��̴� ������Ƽ ����
	static void SetIndexDataInt( int nWidgetID, int nIndex, const char* strName, int nValue, const char* strInstnace, int nSubID = 0 );
	static void SetIndexDataUInt( int nWidgetID, int nIndex, const char* strName, unsigned int nValue, const char* strInstnace, int nSubID = 0 );
	static void SetIndexDataDouble( int nWidgetID, int nIndex, const char* strName, double dValue, const char* strInstnace, int nSubID = 0 );
	static void SetIndexDataBool( int nWidgetID, int nIndex, const char* strName, bool bValue, const char* strInstnace, int nSubID = 0 );
	static void SetIndexDataString( int nWidgetID, int nIndex, const char* strName, const char* strValue, const char* strInstnace, int nSubID = 0 );

	// ����Ʈ ������ ������Ƽ ����
	static void SetListItemInt( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
		int nValue, const char* strInstance, int nSubID = 0 );
	static void SetListItemUInt( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
		unsigned int nValue, const char* strInstance, int nSubID = 0 );
	static void SetListItemDouble( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
		double dValue, const char* strInstance, int nSubID = 0 );
	static void SetListItemBool( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
		bool bValue, const char* strInstance, int nSubID = 0 );
	static void SetListItemString( int nWidgetID, int nIndex, const char* strInstName, const char* strProp, 
		const char* strValue, const char* strInstance, int nSubID = 0 );
	static void SetListItemArray(int nWidgetID, char* strIndexArr, const char* strInstName, const char* strProp, 
		const char* strValue, const char* strTypeArr, const char* strInstance, int nSubID = 0 );

	// ����Ʈ ������ �Լ� ȣ��
	static void CallListItemFunc( int nWidgetID, int nIndex, const char* strInstName, const char* strFunc,
		const char* strParams, const char* strInstance, int nSubID = 0 );

	// �Լ� ȣ��
	static void CallFunction( int nWidgetID, const char* strName, const char* strParams, const char* strInstance, int nSubID = 0 );
	// ��Ŀ�� ����
	static void SetFocus( int nWidgetID, const char* strInstance, int nSubID = 0 );
	// �ؽ�Ʈ ���� ����
	static void SetTextFormat( int nWidgetID, DWORD dwColor, bool bUnderline,
		int nBeginIndex, int nEndIndex, const char* strInstance, int nSubID = 0 );
	// �ؽ�Ʈ ���� ������ ���� // ���Ŀ� SetTextFormat ������� �� ���� ������ ������.. 
	static void SetTextFormatSize( int nWidgetID, DWORD dwSize, const char* strInstance, int nSubID = 0 );

	// Ʈ�� ����
	static void SetTween( int nWidgetID, LuaTable tbQuickSet,
		const char* strProp, const char* strValue, const char* strInstance, int nSubID = 0 );
	static void RemoveTween( int nWidgetID, const char* strInstance, int nSubID = 0 );

	// ���콺 ��Ʈ �׽�Ʈ ����
	static void HitTestEnable( int nWidgetID, bool bEnable, const char* strInstance, int nSubID = 0 );

	// ���� ���� ��ġ/������
	static LuaTable GetSlotTooltipPosSize();

	// UI ��ġ �ʱ�ȭ
	static void ResetWidgetPosition();

	// ��ǳ�� �߰�
	static void SetSpeechBubble(int nWidgetID, const char* strInstance, const char* strText, int nSubID = 0);
	// ��ǳ�� ����
	static void DelSpeechBubble(int nWidgetID, const char* strInstance, int nSubID = 0);
	// ����ũ ��� ����
	static void SetFakeModal( bool bValue );
	// ���� �Ǵ� ��ǥ
	static LuaTable SnapCheck( int nWidgetID, int nX, int nY, int nWidth = 0, int nHeight = 0, int nSubID = 0 );
	// ��ü UI ���İ�
	static void SetUIAlpha( float fAlpha );
	// ���� ����
	static void ReleaseSlot();
	// Ŭ�� ��ũ ������Ʈ
	static void UpdateClubMark( int nWidgetID, const char* szParam, const char* szInst, int nSubID = 0 );
	// UI ���� �� ������
	static void SetFoward( int nWidgetID, int nSubID = 0 );

	// Ʈ�� ��� ������
	static void SetHeadData( int nWidgetID, const char* szObjectID,
		LuaTable tbData, const char* szInst, int nSubID = 0 );
	static LuaTable GetHeadData( int nWidgetID, int nIndex, const char* szInst, int nSubID = 0 );
	static void ChangeHeadData( int nWidgetID, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID = 0 );

	// Ʈ�� ������
	static void SetTreeData( int nWidgetID, LuaTable tbKey, const char* strObjectID, 
		LuaTable tbData, const char* szInst, int nSubID = 0 );
	static LuaTable GetTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szInst, int nSubID = 0 );
	static void ChangeTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID = 0 );

	// ����� �ܼ� ���
	static void PrintDebug( const char* strText, WORD wColor = 0 );
	// ����� �� ���
	static void ToggleDebugScene();

// Client
public :
	// �ؽ�ó ���
	static std::string GetUITexturePath();
	// �̴ϸ� �ؽ�ó ���
	static std::string GetMinimapTexturePath();

	// ���� �ػ� ����Ʈ
	static LuaTable GetResolutionList();

	// ȭ�� ���� ������
	static int GetScreenSizeWidth();
	// ȭ�� ���� ������
	static int GetScreenSizeHeight();
	// ���콺 ��ġ
	static lua_tinker::table GetMousePos();
	// ȭ�� ��ȯ : ��ü ȭ��, â���
	static void ChangeScreenMode();
	// ������ ���?
	static bool IsFullScreen();
	// �Է� ��� ����Ű ��ȯ
	static void TransInputEn();
	// ���� ����
	static void CloseGame();

	// External �ؽ�Ʈ ����
	static std::string GetGameExternalText( const char* strKey, int nIndex );
	// Internal �ؽ�Ʈ ����
	static std::string GetGameInternalText( const char* strKey, int nIndex );
	// Word �ؽ�Ʈ ����
	static std::string GetGameWordText( const char* strKey, int nIndex );
	// �������� ��ȯ�ϴ� ���ؽ�Ʈ;
	static std::string GetTipText_RandomIntex(void);

	static std::string AnsiToUTF8( const char* szText );
	static std::string UTF8ToAnsi( const char* szText );

	// Ŭ���� �̸� ��������
	static std::string GetCharClassName( int nClass );
	static std::string GetCharClassIndexName( int nIndex );
	// ĳ���� �÷��׸� ĳ���� �ε����� ��ȯ

	// ���� ���� Ŭ������ ����
	static int GetCharClassToClassIndex( int nClass  );

	static int GetCharClassFlagIndex( int nCharFlag );
	// ĳ���� �÷��׸� ���� ������ �ٸ� ������ ��ȯ 
	static int GetCharClassToSexFlag( int nCharSex );
	// �� Ÿ�� ��������
	static int GetPetType( const unsigned int nPetType );
	// �� �̸� ��������
	static std::string GetPetName( int nPetName );
	// �� �̸� ���
	static std::string GetMapName( DWORD dwID );
	// ������ �̸� ��������
	static std::string GetItemName( WORD wMID, WORD wSID );
	// ��ų �̸� ��������
	static std::string GetSkillName( WORD wMID, WORD wSID );

	// ���� �ð�
	static LuaTable GetDate();

	// ���ڿ� üũ
	static bool CheckString( const char* str );
	// ���� ���ι��̴�
	static int GetServiceProvider();
	// ���� ���� ���ι��̴�
	static int GetRenderServiceProvider();

	// �������� ��� ����
	static void SetStageBG( int nStage );
	static int GetStageBG();
	// ���� �������� �̵�
	static void GameToLobbyStage( bool bWorldBattle );
	// �� ���ͷ� ����
	static void ToCustomerCenter();
	// �� ������ ����
	static void OpenWebPage( const char* szURL );

	// ���� ���� ����
	static void SetSfxVolume( long lVolume );
	static void SetEnvVolume( long lVolume );
	static void SetBGMVolume( long lVolume );
	static long GetSfxVolume( long lVolume );
	static long GetEnvVolume( long lVolume );
	static long GetBGMVolume( long lVolume );
	static void SetMuteSfx( bool bMute );
	static void SetMuteEnv( bool bMute );
	static void SetMuteBGM( bool bMute );
	static bool GetMuteSfx();
	static bool GetMuteEnv();
	static bool GetMuteBGM();

	// ���� �ɼ�
	static void SetGameOption( LuaTable tbOption );
	static LuaTable GetGameOption();

	// ī�޶� ��ġ
	static LuaTable GetCameraPos();
	// ī�޶� ����
	static LuaTable GetCameraDir();

	// �ڵ� ��Ȱ �ɼ�
	static bool GetAutoResurrection();
	static void SetAutoResurrection( bool bValue );

	// ����
	static void StopSound( const char* str );
	static void PlaySound( const char* str );

	static DWORD MakeNativeID( WORD wMID, WORD wSID );
	static LuaTable SplitNativeID( DWORD dwID );

	static std::string MakeMoney( LONGLONG llMoney );
	static WORD ColorDWORDtoWORD( DWORD dwColor );
	static DWORD ColorWORDtoDWORD( WORD wColor );

	// ���� ����
	static void OpenForum();

	static std::string GetCompositionString();
	static LuaTable GetItem_BoxItemlist( WORD wMID, WORD wSID );
	static DWORD GetMoneyColor( LONGLONG lMoney );

// Command
public :
	// ��� ����
	static std::string DoCommand( const char* strCommand );

// ������ ����
public :
	// ���� ������ ����
	static void ResetSlot( const char* strSlotID );
	// ������ ���� ����
	static void SetSlot_Item( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID );
	static void SetSlot_Item_NonStrengthen( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID );
	static void SetSlot_ItemBornT( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, DWORD dwBornT );
	// ������ ���� ����
	static void SetSlot_Item_DWORD( const char* strSlotID, DWORD dwID, WORD wCount, DWORD dwRealID );
	// ��ų ���� ����
	static void SetSlot_Skill( const char* strSlotID, DWORD dwID );
	// ��ų ���� ����
	static void SetSlot_SkillLevel( const char* strSlotID, DWORD dwID, WORD wLevel );
	// �κ��丮 ���� ����
	static void SetSlot_Inventory( const char* strSlotID, int nPosX, int nPosY );
	// �� ���� ���� ����
	static void SetSlot_Rebuy( const char* strSlotID, int nRebuyID, WORD wMID, WORD wSID	);
	// ��ȯ�� ���� ���� ����;
	static void SetSlot_SummonState( const char* strSlotID, const char* strIconName, INT nIconIdxX, INT nIconIdxY );
	// ���� ���� ����
	static void SetSlot_Bank( const char* strSlotID, int nPosX, int nPosY );
	// ���ø���Ʈ ���� ����
	static void SetSlot_Wishlist( const char* strSlotID, int nPosX, int nPosY );
	// ĳ���� ���� ���� ����
	static void SetSlot_CharInfo( const char* strSlotID, int nSlot );
	// ��ų ������ ����
	static void SetSlot_SQS( const char* strSlotID, DWORD dwID, int nIndex );
	// ������ ������ ����
	static void SetSlot_IQS( const char* strSlotID, DWORD dwID, int nIndex );
	// �̴ϰ��� : �ֻ��� ���� ���� ����;
	static void SetSlot_MiniGame_Dice( const char* strSlotID, DWORD dwID, int nIndex );
	// NPC ���� ���� ����
	static void SetSlot_NpcMarket( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY );
	// NPC ���� īƮ ���� ����
	static void SetSlot_NpcMarketCart( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wIndex );
	// �ŷ� â ���� ����
	static void SetSlot_Trade(const char* strSlotID, int nPosX, int nPosY, bool _MyBox);
	// PointShop ���� ����
	static void SetSlot_PointShop( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY );
	// PointShop īƮ ���� ����
	static void SetSlot_PointShopCart( const char* strSlotID, WORD wMID, WORD wSID, int nIndex );
	// ��������â ���� ����
	static void SetSlot_ReferChar( const char* strSlotID, int nSlot );
	// ���� ���� �˻� ����Ʈ �ŷ� ������ ��� ���� ����
	static void SetSlot_PMSPointTrade( const char* strSlotID, WORD wPosX, WORD wPosY );
	// ���λ����˻� ����Ʈ�� �ε��� ����
	static void SetSlot_PMSPointTrade_Index( const char* strSlotID, DWORD dwIndex, DWORD dwCount );
	// ���λ��� ���
	static void SetSlot_PrivateMarket( const char* strSlotID, WORD wPosX, WORD wPosY, bool bSelf );
	// ���λ��� �˻�
	static void SetSlot_PrivateMarketSearch( const char* strSlotID, DWORD dwIndex, WORD wCount, bool bSearchPage );
	// ���� ����
	static void SetSlot_ReceivePost( const char* strSlotID, DWORD dwIndex, DWORD dwCount );
	// PostBoxSub_PreviewPageOpen
	static void SetSlot_PostBoxSub_PreviewPageOpen( const char* strSlotID );
	// �� ������ ����
	static void SetSlot_Pet( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount );
	// Ŭ�� â�� ����
	static void SetSlot_ClubStorage( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY );
	// Ŭ�� â�� ���� ����
	static void SetClubStorage_Item( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY );
	// ��Ŀ ���� ����
	static void SetSlot_Locker( const char* strSlotID, int nIndex, int nPosX, int nPosY );
	// ���� ī�� ���� ����
	static void SetSlot_RebuildCard( const char* strSlotID, int nPosX, int nPosY );
	// ��Ƽ �й� ������ ���� ����;
	static void SetSlot_PartyDistribution(const char* strSlotID, DWORD dwItemID, WORD wSlotIndex);
	// ������ ���� ���� ����
	static void SetSlot_ItemRepair( const char* strSlotID, int nSlot );
	// �ζ� ��÷ ������ ���� ���� ����;
	static void SetSlot_LottoWinItem ( const char* strSlotID, DWORD dwLottoSystemID );
	// �ζ� ���� ��÷�� ����Ʈ ��÷ ������ ��� ���� ����;
	static void SetSlot_PreWinListItem ( const char* strSlotID, DWORD dwLottoSystemID, int nRanking );
	// �� ����� ���� ���� ����
	static void SetSlot_RanMobileReward( const char* strSlotID, int nIndex );
	// ��ų ������ �����Ѵ�, ���Կ��� ��µǴ� ������ ������ UiComponent���� ��½�Ű�� ����;
	// ���� �����͸� �����Ŵ;
	static void RegShowTooltipEnableState ( int nWidgetID, int nSubID, const char* strInstance, WORD wMID, WORD wSID, WORD wLevel, bool bUpdate );
	static void UnregShowTooltipEnableState ( int nWidgetID, int nSubID, const char* strInstance );

// ä��
public :
	// ä�� �޽��� �߰�
	static void AddChatMessage( int nLogID, DWORD dwType, bool bOnlyStore, const char* strMsg, 
		DWORD dwColor, LuaTable tbCustomColor, LuaTable tbNameData );
	// ��ũ �޽��� �߰�
	static void AddLinkMessage( int nLogID, DWORD dwType, bool bOnlyStore, const char* strMsg, 
		DWORD dwColor, LuaTable tbCustomColor, LuaTable tbData, LuaTable tbNameData );

	// ����� ä�� �޽��� �߰�
	static void AddStoredChat( int nLogID );
	static void AddStoredChatType( int nLogID, DWORD dwType );

	// ����� ä�� �޽��� Ŭ����
	static void ClearStoredChat( int nLogID );

	// �� ĳ���� �����
	static void AddHeadChat( const char* strName, const char* strMsg );

// ���� �ؽ�ó
public :
	// ĳ���� ���� �ؽ�ó ������Ʈ
	static void UpdateCharInfoRT();
	// ĳ���� Ż�� ���� �ؽ��� ������Ʈ;
	static void UpdateCharVehicleInfoRT( WORD wMid = SNATIVEID::ID_NULL, WORD wSid = SNATIVEID::ID_NULL );
	// �̴ϰ��� : �ֻ��� �ؽ��� ������Ʈ;
	static void UpdateDiceRT(void);
	// ������ �̸����� ������Ʈ
	static void UpdateItemPreviewRT();

public:
	static bool GetCapslockKeyState();
	static void SetCapslockKeyState(bool bDown);
};

#include "./GLWidgetScript.hpp"