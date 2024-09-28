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

	// 월드 좌표를 미니맵 좌표로 변환
	void ConvertWorldToMap( const GLMapAxisInfo& sMapAxisInfo,
		const D3DXVECTOR3& vPosition, int& nPosX, int& nPosY );

	// 미니맵 이미지 위치를 월드 좌표로 변환
	void ConvertImgMapToWorld( const GLMapAxisInfo& sMapAxisInfo,
		int nPosX, int nPosY, D3DXVECTOR3& vPosition );

	// 미니맵 이미지 위치를 미니맵 좌표로 변환
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
	// 위젯 리스트, 스크립트 로드
	HRESULT LoadFile( const SUBPATH* pPath );

	// wsf 파일을 기준으로 WidgetScript 들을 로딩한다.
	// 넘어온 pZipFile 는 현 함수안에서 무조건 fclose()가 된다.
	// pZipFile			: Zip 파일
	// strPathWSF_ZIP	: Zip 파일의 Full Path.
	BOOL LoadMemory( FILE* pZipFile, const std::string& strPathWSF_ZIP );

	// LoadMemory 다음에 호출된다.
	BOOL LoadFileInZip( const std::string& strPathWSF_ZIP );

	// 루아 디파인 값 로드
	void LoadDefine();

	// 루아 전역 변수 초기화
	void InitGlobal();
	// 루아에서 사용할 함수 등록
	void RegFunc();

public :
	// 루아 함수 호출
	// fmt (-n : int) (-dw : DWORD) (-l : LONGLONG) (-f : double) (-s : char*) (-b : bool) 
	template< typename T >
	T LuaCallFunc( const char* szName, const char* fmt = "", ... );
	template< typename T >
	T LuaCallFunc( const char* szName, const VEC_WIDGET_CALLFUNCVALUE& vecValue );
	template< typename T >
	T LuaCallFunc( const char* szName, const GLWidgetCallFuncValue* pArgs, int nArgCount );

	// 루아 명령 실행
	void LuaDoCommand( const char* strCommand );

private:
	void recordPf(const char* szName);

public :
	lua_State* GetLuaState() { return m_pLuaState; }

public :
	// 사용할 아이콘 슬롯 등록
	static void RegSlot( WORD wType, const char* strSlotID );
	// HTML View ID 할당
	static void RegHtmlWebView( int nViewID );
	// HTML View 이동
	static void MoveHtmlWebView( int nViewID, bool bVisible, int nX, int nY, int nWidth, int nHeight );

	// widget 열기
	static void OpenWidget( int nWidgetID, int nSubID = 0 );
	// widget 닫기
	static void CloseWidget( int nWidgetID, int nSubID = 0 );
	// 전체 widget 닫기
	static void CloseAllWidget( bool bDelete );
	// Widget 열려있나?
	static bool IsOpenWidget( int nWidgetID, int nSubID = 0 );
	// Widget 로드된 상태인가?
	static bool IsLoadWidget( int nWidgetID, int nSubID = 0 );
	// 멀티 Widget 열린 개수
	static int GetMultiWidgetCount( int nWidgetID );
	// ESC키로 닫을 수 있는 UI 하나씩 닫기, 닫을게 없으면 false
	static bool EscWidgetClose();
	// 툴팁 열려있나?
	static bool IsOpenTooltip();

	// 스테이지 이벤트 설정
	static void SetStageEvent( const char* strEvent, const char* strCallBack, const char* strRetParams );

	// 사칙연산
	static __int64 Calculate_Plus( __int64 nVal1, __int64 nVal2 );
	static __int64 Calculate_Minus( __int64 nVal1, __int64 nVal2 );
	static __int64 Calculate_Multiply( __int64 nVal1, __int64 nVal2 );
	static __int64 Calculate_Divide( __int64 nVal1, __int64 nVal2 );

	// 프로퍼티 설정
	static void SetInt( int nWidgetID, const char* strProp, int nValue, const char* strInstance, int nSubID = 0 );
	static void SetUInt( int nWidgetID, const char* strProp, unsigned int nValue, const char* strInstance, int nSubID = 0 );
	static void SetDouble( int nWidgetID, const char* strProp, double dValue, const char* strInstance, int nSubID = 0 );
	static void SetBool( int nWidgetID, const char* strProp, bool bValue, const char* strInstance, int nSubID = 0 );
	static void SetString( int nWidgetID, const char* strProp, const char* strValue, const char* strInstance, int nSubID = 0 );
	static void SetWString( int nWidgetID, const char* strProp, const char* strValue, const char* strInstance, int nSubID = 0 );
	// static void SetProperty_array( int nWidgetID, const char* strProp, const char* strValue, const char* strInstance, int nSubID = 0 );
	static void SetArray( int nWidgetID, LuaTable tbProp, LuaTable tbValue, LuaTable tbInstance, LuaTable tbValueType, int nSubID = 0 );

	// 프로퍼티 얻기
	static int GetInt( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static unsigned int GetUInt( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static double GetDouble( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static bool GetBool( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static std::string GetString( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );
	static LuaTable GetArray( int nWidgetID, const char* strProp, const char* strInstance, int nSubID = 0 );

	// 이벤트 심기
	static void SetEvent( int nWidgetID, const char* strEvent, const char* strCallBack, 
		const char* strRetProp, const char* strInstance, const char* strDelivery = NULL, int nSubID = 0 );

	// 데이터 프로바이더 설정
	static void SetData( int nWidgetID, const char* strDataProvider, const char* strInstance, bool bAdd, int nSubID = 0 );
	static void SetDataEx( int nWidgetID, const char* strObjectID, LuaTable tbData,
								   const char* strInstance, bool bAdd, int nSubID = 0 );
	static void SetDataComplex( int nWidgetID, const char* strObjectID, LuaTable tbData,
										const char* strInstance, bool bAdd, int nSubID = 0 );
	static void SetDataTerms(int nWidgetID, const char* strInstName
		, LuaTable tbWhereProvName, LuaTable tbWhereProvData, LuaTable tbWhereProvType
		, LuaTable tbNewProvName, LuaTable tbNewProvData, LuaTable tbNewProvType);//, int nSubID = 0);
	// 데이터 프로바이더 제거
	static void RemoveData( int nWidgetID, int nIndex, const char* strInstance, bool bAll, int nSubID = 0 );

	// 데이터 프로바이더 얻기
	static int GetDataInt( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static unsigned int GetDataUInt( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static double GetDataDouble( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static bool GetDataBool( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static std::string GetDataString( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	static LuaTable GetDataArray( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID = 0 );
	
	// 데이터 프로바이더 프로퍼티 설정
	static void SetIndexDataInt( int nWidgetID, int nIndex, const char* strName, int nValue, const char* strInstnace, int nSubID = 0 );
	static void SetIndexDataUInt( int nWidgetID, int nIndex, const char* strName, unsigned int nValue, const char* strInstnace, int nSubID = 0 );
	static void SetIndexDataDouble( int nWidgetID, int nIndex, const char* strName, double dValue, const char* strInstnace, int nSubID = 0 );
	static void SetIndexDataBool( int nWidgetID, int nIndex, const char* strName, bool bValue, const char* strInstnace, int nSubID = 0 );
	static void SetIndexDataString( int nWidgetID, int nIndex, const char* strName, const char* strValue, const char* strInstnace, int nSubID = 0 );

	// 리스트 아이템 프로퍼티 설정
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

	// 리스트 아이템 함수 호출
	static void CallListItemFunc( int nWidgetID, int nIndex, const char* strInstName, const char* strFunc,
		const char* strParams, const char* strInstance, int nSubID = 0 );

	// 함수 호출
	static void CallFunction( int nWidgetID, const char* strName, const char* strParams, const char* strInstance, int nSubID = 0 );
	// 포커스 설정
	static void SetFocus( int nWidgetID, const char* strInstance, int nSubID = 0 );
	// 텍스트 포맷 설정
	static void SetTextFormat( int nWidgetID, DWORD dwColor, bool bUnderline,
		int nBeginIndex, int nEndIndex, const char* strInstance, int nSubID = 0 );
	// 텍스트 포멧 사이즈 설정 // 추후에 SetTextFormat 기능쪽은 다 합쳐 버리는 쪽으로.. 
	static void SetTextFormatSize( int nWidgetID, DWORD dwSize, const char* strInstance, int nSubID = 0 );

	// 트윈 설정
	static void SetTween( int nWidgetID, LuaTable tbQuickSet,
		const char* strProp, const char* strValue, const char* strInstance, int nSubID = 0 );
	static void RemoveTween( int nWidgetID, const char* strInstance, int nSubID = 0 );

	// 마우스 히트 테스트 설정
	static void HitTestEnable( int nWidgetID, bool bEnable, const char* strInstance, int nSubID = 0 );

	// 슬롯 툴팁 위치/사이즈
	static LuaTable GetSlotTooltipPosSize();

	// UI 위치 초기화
	static void ResetWidgetPosition();

	// 말풍선 추가
	static void SetSpeechBubble(int nWidgetID, const char* strInstance, const char* strText, int nSubID = 0);
	// 말풍선 삭제
	static void DelSpeechBubble(int nWidgetID, const char* strInstance, int nSubID = 0);
	// 페이크 모달 설정
	static void SetFakeModal( bool bValue );
	// 스냅 되는 좌표
	static LuaTable SnapCheck( int nWidgetID, int nX, int nY, int nWidth = 0, int nHeight = 0, int nSubID = 0 );
	// 전체 UI 알파값
	static void SetUIAlpha( float fAlpha );
	// 슬롯 놓기
	static void ReleaseSlot();
	// 클럽 마크 업데이트
	static void UpdateClubMark( int nWidgetID, const char* szParam, const char* szInst, int nSubID = 0 );
	// UI 순서 맨 앞으로
	static void SetFoward( int nWidgetID, int nSubID = 0 );

	// 트리 헤드 데이터
	static void SetHeadData( int nWidgetID, const char* szObjectID,
		LuaTable tbData, const char* szInst, int nSubID = 0 );
	static LuaTable GetHeadData( int nWidgetID, int nIndex, const char* szInst, int nSubID = 0 );
	static void ChangeHeadData( int nWidgetID, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID = 0 );

	// 트리 데이터
	static void SetTreeData( int nWidgetID, LuaTable tbKey, const char* strObjectID, 
		LuaTable tbData, const char* szInst, int nSubID = 0 );
	static LuaTable GetTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szInst, int nSubID = 0 );
	static void ChangeTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szObjectID, 
		LuaTable tbData, const char* szInst, int nSubID = 0 );

	// 디버깅 콘솔 출력
	static void PrintDebug( const char* strText, WORD wColor = 0 );
	// 디버깅 씬 토글
	static void ToggleDebugScene();

// Client
public :
	// 텍스처 경로
	static std::string GetUITexturePath();
	// 미니맵 텍스처 경로
	static std::string GetMinimapTexturePath();

	// 게임 해상도 리스트
	static LuaTable GetResolutionList();

	// 화면 가로 사이즈
	static int GetScreenSizeWidth();
	// 화면 세로 사이즈
	static int GetScreenSizeHeight();
	// 마우스 위치
	static lua_tinker::table GetMousePos();
	// 화면 전환 : 전체 화면, 창모드
	static void ChangeScreenMode();
	// 윈도우 모드?
	static bool IsFullScreen();
	// 입력 모드 영문키 전환
	static void TransInputEn();
	// 게임 종료
	static void CloseGame();

	// External 텍스트 번역
	static std::string GetGameExternalText( const char* strKey, int nIndex );
	// Internal 텍스트 번역
	static std::string GetGameInternalText( const char* strKey, int nIndex );
	// Word 텍스트 번역
	static std::string GetGameWordText( const char* strKey, int nIndex );
	// 랜덤으로 반환하는 팁텍스트;
	static std::string GetTipText_RandomIntex(void);

	static std::string AnsiToUTF8( const char* szText );
	static std::string UTF8ToAnsi( const char* szText );

	// 클래스 이름 가져오기
	static std::string GetCharClassName( int nClass );
	static std::string GetCharClassIndexName( int nIndex );
	// 캐릭터 플래그를 캐릭터 인덱스로 변환

	// 성별 없는 클래스로 변경
	static int GetCharClassToClassIndex( int nClass  );

	static int GetCharClassFlagIndex( int nCharFlag );
	// 캐릭터 플래그를 같은 직업의 다른 성별로 변환 
	static int GetCharClassToSexFlag( int nCharSex );
	// 펫 타입 가져오기
	static int GetPetType( const unsigned int nPetType );
	// 펫 이름 가져오기
	static std::string GetPetName( int nPetName );
	// 맵 이름 얻기
	static std::string GetMapName( DWORD dwID );
	// 아이템 이름 가져오기
	static std::string GetItemName( WORD wMID, WORD wSID );
	// 스킬 이름 가져오기
	static std::string GetSkillName( WORD wMID, WORD wSID );

	// 현재 시간
	static LuaTable GetDate();

	// 문자열 체크
	static bool CheckString( const char* str );
	// 서비스 프로바이더
	static int GetServiceProvider();
	// 렌더 서비스 프로바이더
	static int GetRenderServiceProvider();

	// 스테이지 배경 설정
	static void SetStageBG( int nStage );
	static int GetStageBG();
	// 서버 선택으로 이동
	static void GameToLobbyStage( bool bWorldBattle );
	// 고객 센터로 연결
	static void ToCustomerCenter();
	// 웹 페이지 열기
	static void OpenWebPage( const char* szURL );

	// 게임 볼륨 설정
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

	// 게임 옵션
	static void SetGameOption( LuaTable tbOption );
	static LuaTable GetGameOption();

	// 카메라 위치
	static LuaTable GetCameraPos();
	// 카메라 방향
	static LuaTable GetCameraDir();

	// 자동 부활 옵션
	static bool GetAutoResurrection();
	static void SetAutoResurrection( bool bValue );

	// 사운드
	static void StopSound( const char* str );
	static void PlaySound( const char* str );

	static DWORD MakeNativeID( WORD wMID, WORD wSID );
	static LuaTable SplitNativeID( DWORD dwID );

	static std::string MakeMoney( LONGLONG llMoney );
	static WORD ColorDWORDtoWORD( DWORD dwColor );
	static DWORD ColorWORDtoDWORD( WORD wColor );

	// 포럼 열기
	static void OpenForum();

	static std::string GetCompositionString();
	static LuaTable GetItem_BoxItemlist( WORD wMID, WORD wSID );
	static DWORD GetMoneyColor( LONGLONG lMoney );

// Command
public :
	// 명령 수행
	static std::string DoCommand( const char* strCommand );

// 아이콘 슬롯
public :
	// 슬롯 데이터 리셋
	static void ResetSlot( const char* strSlotID );
	// 아이템 슬롯 설정
	static void SetSlot_Item( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID );
	static void SetSlot_Item_NonStrengthen( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID );
	static void SetSlot_ItemBornT( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, DWORD dwBornT );
	// 아이템 슬롯 설정
	static void SetSlot_Item_DWORD( const char* strSlotID, DWORD dwID, WORD wCount, DWORD dwRealID );
	// 스킬 슬롯 설정
	static void SetSlot_Skill( const char* strSlotID, DWORD dwID );
	// 스킬 슬롯 설정
	static void SetSlot_SkillLevel( const char* strSlotID, DWORD dwID, WORD wLevel );
	// 인벤토리 슬롯 설정
	static void SetSlot_Inventory( const char* strSlotID, int nPosX, int nPosY );
	// 재 구입 슬롯 설정
	static void SetSlot_Rebuy( const char* strSlotID, int nRebuyID, WORD wMID, WORD wSID	);
	// 소환수 정보 슬롯 설정;
	static void SetSlot_SummonState( const char* strSlotID, const char* strIconName, INT nIconIdxX, INT nIconIdxY );
	// 은행 슬롯 설정
	static void SetSlot_Bank( const char* strSlotID, int nPosX, int nPosY );
	// 위시리스트 슬롯 설정
	static void SetSlot_Wishlist( const char* strSlotID, int nPosX, int nPosY );
	// 캐릭터 정보 슬롯 설정
	static void SetSlot_CharInfo( const char* strSlotID, int nSlot );
	// 스킬 퀵슬롯 설정
	static void SetSlot_SQS( const char* strSlotID, DWORD dwID, int nIndex );
	// 아이템 퀵슬롯 설정
	static void SetSlot_IQS( const char* strSlotID, DWORD dwID, int nIndex );
	// 미니게임 : 주사위 보상 슬롯 설정;
	static void SetSlot_MiniGame_Dice( const char* strSlotID, DWORD dwID, int nIndex );
	// NPC 마켓 슬롯 설정
	static void SetSlot_NpcMarket( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY );
	// NPC 마켓 카트 슬롯 설정
	static void SetSlot_NpcMarketCart( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wIndex );
	// 거래 창 슬롯 설정
	static void SetSlot_Trade(const char* strSlotID, int nPosX, int nPosY, bool _MyBox);
	// PointShop 슬롯 설정
	static void SetSlot_PointShop( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY );
	// PointShop 카트 슬롯 설정
	static void SetSlot_PointShopCart( const char* strSlotID, WORD wMID, WORD wSID, int nIndex );
	// 정보보기창 슬롯 설정
	static void SetSlot_ReferChar( const char* strSlotID, int nSlot );
	// 개인 상점 검색 포인트 거래 아이템 등록 슬롯 설정
	static void SetSlot_PMSPointTrade( const char* strSlotID, WORD wPosX, WORD wPosY );
	// 개인상점검색 포인트샵 인덱스 슬롯
	static void SetSlot_PMSPointTrade_Index( const char* strSlotID, DWORD dwIndex, DWORD dwCount );
	// 개인상점 등록
	static void SetSlot_PrivateMarket( const char* strSlotID, WORD wPosX, WORD wPosY, bool bSelf );
	// 개인상점 검색
	static void SetSlot_PrivateMarketSearch( const char* strSlotID, DWORD dwIndex, WORD wCount, bool bSearchPage );
	// 우편 슬롯
	static void SetSlot_ReceivePost( const char* strSlotID, DWORD dwIndex, DWORD dwCount );
	// PostBoxSub_PreviewPageOpen
	static void SetSlot_PostBoxSub_PreviewPageOpen( const char* strSlotID );
	// 펫 아이템 슬롯
	static void SetSlot_Pet( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount );
	// 클럽 창고 슬롯
	static void SetSlot_ClubStorage( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY );
	// 클럽 창고 슬롯 갱신
	static void SetClubStorage_Item( const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY );
	// 락커 슬롯 설정
	static void SetSlot_Locker( const char* strSlotID, int nIndex, int nPosX, int nPosY );
	// 개조 카드 슬롯 설정
	static void SetSlot_RebuildCard( const char* strSlotID, int nPosX, int nPosY );
	// 파티 분배 아이템 슬롯 설정;
	static void SetSlot_PartyDistribution(const char* strSlotID, DWORD dwItemID, WORD wSlotIndex);
	// 아이템 수리 슬롯 설정
	static void SetSlot_ItemRepair( const char* strSlotID, int nSlot );
	// 로또 당첨 아이템 보기 슬롯 설정;
	static void SetSlot_LottoWinItem ( const char* strSlotID, DWORD dwLottoSystemID );
	// 로또 이전 당첨자 리스트 당첨 아이템 목록 슬롯 설정;
	static void SetSlot_PreWinListItem ( const char* strSlotID, DWORD dwLottoSystemID, int nRanking );
	// 란 모바일 보상 슬롯 설정
	static void SetSlot_RanMobileReward( const char* strSlotID, int nIndex );
	// 스킬 툴팁을 저장한다, 슬롯에서 출력되는 툴팁을 제외한 UiComponent에서 출력시키기 위한;
	// 툴팁 데이터를 저장시킴;
	static void RegShowTooltipEnableState ( int nWidgetID, int nSubID, const char* strInstance, WORD wMID, WORD wSID, WORD wLevel, bool bUpdate );
	static void UnregShowTooltipEnableState ( int nWidgetID, int nSubID, const char* strInstance );

// 채팅
public :
	// 채팅 메시지 추가
	static void AddChatMessage( int nLogID, DWORD dwType, bool bOnlyStore, const char* strMsg, 
		DWORD dwColor, LuaTable tbCustomColor, LuaTable tbNameData );
	// 링크 메시지 추가
	static void AddLinkMessage( int nLogID, DWORD dwType, bool bOnlyStore, const char* strMsg, 
		DWORD dwColor, LuaTable tbCustomColor, LuaTable tbData, LuaTable tbNameData );

	// 저장된 채팅 메시지 추가
	static void AddStoredChat( int nLogID );
	static void AddStoredChatType( int nLogID, DWORD dwType );

	// 저장된 채팅 메시지 클리어
	static void ClearStoredChat( int nLogID );

	// 내 캐릭터 헤드쳇
	static void AddHeadChat( const char* strName, const char* strMsg );

// 렌더 텍스처
public :
	// 캐릭터 정보 텍스처 업데이트
	static void UpdateCharInfoRT();
	// 캐릭터 탈것 정보 텍스쳐 업데이트;
	static void UpdateCharVehicleInfoRT( WORD wMid = SNATIVEID::ID_NULL, WORD wSid = SNATIVEID::ID_NULL );
	// 미니게임 : 주사위 텍스쳐 업데이트;
	static void UpdateDiceRT(void);
	// 아이템 미리보기 업데이트
	static void UpdateItemPreviewRT();

public:
	static bool GetCapslockKeyState();
	static void SetCapslockKeyState(bool bDown);
};

#include "./GLWidgetScript.hpp"