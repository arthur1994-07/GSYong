#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicButton;
class GLMapAxisInfo;
class CBasicProgressBar;
class CMiniMapInfo;
class GLGaeaClient;

const DWORD UIMSG_MOUSEIN_FULLSCREENBUTTON = UIMSG_USER1;

class CMiniMap : public CUIGroup
{
private:
static	const	int	nTIME_MINUTE;

protected:
	enum
	{
		MINIMAP_FULLSCREEN = NO_ID + 1,
	};

public:
	CMiniMap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CMiniMap(void);

protected:
    GLGaeaClient* m_pGaeaClient;

protected:
	void	RotateMap();
	void	SetMapProperty();						// 맵 좌표 속성 줌 스텝
	void	UpdateMapTexturePos();	

public:
	void	CreateSubControl();

private:
	void	CreateMap( char *MapPos);
	void	CreateUserMark(char *keyvalue);
	void	CreateZoomButton(char* InKeyvalue, char *InFlipvalue, char *OutKeyvalue, char*OutFlipvalue);
	void	CreateTextBox(char *x_keyvalue, char *y_keyvalue);

protected:
	//	맵 정보 조절 함수
	void	SetMapData ( const char *szMiniMapTex,UIRECT tPos );
	void	SetMapSize ( int size_x, int size_y, int start_x, int start_y );

public:
	BOOL	LoadMapInfo(char *szMapInfo);
	void	SetMapAxisInfo(const GLMapAxisInfo &sInfo, const CString& strMapName );

	void	CalcMapPos( int cur_x, int cur_y, int& MapX, int& MapY );

	void	SetMapInfo( int size_x, int size_y, const char *szMapName, int start_x, int start_y,UIRECT tMapPos );

	void	StartClubGuidBattleTime();
    void    SetClubGuidBattleName( const std::vector<std::string> &vecName );
    void	UpdateClubGuidBattleTime( const std::vector<float> &vecTime );

    void	StartClubDeathMatchTime();
    void    UpdateClubDeathMatchTime( const float fTime );

    void	EndClubBattleTime();

	const UIRECT& GetMinimapBackGlobalPos();

public:
	virtual HRESULT Render(LPDIRECT3DDEVICEQ pD3DDevice); // 렌더상태를 설정하고 뷰포트를 지우고 장면을 그리는 역할	

public:
	void UpdateTime( float fElapsedTime );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );


private:
	CUIControl*	m_pMap;
	CUIControl* m_pUserMark;
    CBasicTextBox*	m_pXPos;
	CBasicTextBox*	m_pYPos;
	CBasicTextBox*	m_pMiniMapName;
	CBasicButton*	m_pZoomOut;
	CBasicButton*	m_pZoomIn;

	CUIControl*		m_pTime;
	CBasicTextBox*	m_pTimeText;
	CBasicTextBox*	m_pServerTimeText;
	CUIControl*		m_pDirection;
	
	CUIControl*		m_pArrowCountBack;
	CBasicTextBox*	m_pArrowCountText;
	CUIControl*		m_pBack; // by luxes.

private:
	int			m_nMap_X, m_nMap_Y;					//	전체 맵 사이즈
	int			m_nMapSize;							//	맵사이즈
	int			m_nStart_X, m_nStart_Y;				//	맵 시작 좌표

	int			m_nMin_X, m_nMin_Y;					//	보여지는 맵 좌표


	UIRECT		m_rcTexture_Pos;					//	텍스쳐 좌표

private:
	CBasicProgressBar* m_pPK_ATTACK;
	CBasicProgressBar* m_pPK_DEFENSE;

	CUIControl*		m_pFullScreenButton;
	CUIControl*		m_pFullScreenButtonDummy;
	CMiniMapInfo*	m_pMiniMapInfoDummy;
	CMiniMapInfo*	m_pMiniMapInfoLeftDummy;

	CMiniMapInfo * m_pMimiMapInfo; // 맵 정보 컨트롤 포인터

private:
	bool	m_bTEST;

private:
	WORD            m_wAmmoCount;
	CBasicTextBox*	m_pAmmoText;
	CBasicTextBox*	m_pClubBattleTitle_Text;        // 데스매치인지 선도전인지 나타내는 타이틀
    CBasicTextBox*	m_pClubBattleTime_Text;         // 데스매치 or 선도전 종료까지 남은 시간
    CBasicTextBox*	m_pClubBattleName_Text;         // 선도전 이름 (ex 성문,현암)
	CUIControl*     m_pCenterPoint;

protected:
	CInnerInterface* m_pInterface;

};

class MyMiniMap : public IMiniMap, private CMiniMap
{
public:
    MyMiniMap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyMiniMap()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetMapAxisInfo(const GLMapAxisInfo &sInfo, const UI::String &strMapName );
    
    virtual void StartClubGuidBattleTime();
    virtual void SetClubGuidBattleName( const std::vector<std::string> &vecName );
    virtual void UpdateClubGuidBattleTime( const std::vector<float> &vecTime );

    virtual void StartClubDeathMatchTime();
    virtual void UpdateClubDeathMatchTime( const float fTime );
    
    virtual void EndClubBattleTime();

    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual const UIRECT&	GetMinimapBackGlobalPos();
};