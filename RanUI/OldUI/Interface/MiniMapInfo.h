#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CInnerInterface; 
class	CBasicTextBox;
class GLGaeaClient;
class CSwapImage;

class CMiniMapInfo : public CUIGroup
{
protected:
	enum
	{
		MINIMAP_CLUBMARK = NO_ID + 1,
        MINIMAP_CTFMARK,
		MINIMAP_CTIMARK,
	};

public:
	CMiniMapInfo(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CMiniMapInfo(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl();

public:
	void SetMapPos( float fCurX, float fCurY, int nMiniX, int nMiniY );
	void UpdatePostion();

	CString & GetMapName() { return m_strMapName; }
	void SetMapName( const CString & strMapName ) { m_strMapName = strMapName; } 

public:
	// 렌더상태를 설정하고 뷰포트를 지우고 장면을 그리는 역할
	virtual HRESULT Render(LPDIRECT3DDEVICEQ pD3DDevice);

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	int m_nMiniPosX, m_nMiniPosY;	// 미니멥 좌표.

private:
	// 클럽 마크 표시를 위한 정보
	DWORD	m_dwVer;
	int		m_nID;
	int		m_nServer;
	CString	m_strClubName;

private:
	CString			m_strMapName;	// 맵 이름

	CBasicTextBox*	m_pChannelTextBox;
	CBasicTextBox*	m_pMinimapTextBox;
	CUIControl*		m_pClubMark;
    CSwapImage*     m_pCTFMark;
	CSwapImage*		m_pCTIMark;

protected:
	CInnerInterface* m_pInterface;

};