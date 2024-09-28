#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

#include <vector>

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicLineBoxSmart;

class CCompetitionNotifyButton : public CUIGroup
{
private:
    enum
    {
        NOTIFY_BUTTON = NO_ID + 1,
    };

public:
    typedef std::vector<std::string> VEC_STRING;
    typedef VEC_STRING::iterator     ITER_VEC_STRING;

    typedef std::pair<DWORD, std::string> MAP_STRING_VALUE;
    typedef std::map<DWORD, std::string> MAP_STRING;
    typedef MAP_STRING::iterator     ITER_MAP_STRING;

public:
    CCompetitionNotifyButton(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CCompetitionNotifyButton ();

public:
    void	CreateSubControl ();

public:
	virtual	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
    void            Update();
	void			UpdateAlarm();
	void			SetWaitJoin( int nType );
    void			ClearAlarm();

public:
    void            SetLock(const bool _bLock);
    void            SetAlarm(const DWORD _dwMatchingNum, const char* szText);  // 전장 버튼위에 조그맣게 뜨는 알람 메시지 (ex. 점령전 신청가능);
    void            SetTooltip(const DWORD _dwMatchingNum, const char* szText);  // 전장 버튼을 마우스 오버시 나오는 전장의 현 상태 (ex. 2시간후 점령전이 시작됩니다.);

private:
    void            UpdateCTF();    // 점령전은 아직 컨버팅 되지않은 전장 컨텐츠이므로 예외적으로 Update처리를 해준다.;

private:
    MAP_STRING      m_mapAlarm;
    MAP_STRING      m_mapTootip;
    bool            m_bChangeAlarm;

private:
	CBasicButton*   m_pButtonImage;
	CUIControl*		m_pAlarm_Arrow;
	CBasicTextBox*	m_pAlarm_Text;
	CUIControl*     m_pLock;
	CBasicLineBoxSmart* m_pAlarm_LineBox;

private:
    bool            m_bAlarm;
	VEC_STRING      m_vecAlarm;
	std::vector<int> m_vecWaitJoin;

private:
    float           m_fMousePosX;
    float           m_fMousePosY;
    float           m_fUpdateTime;
    UINT            m_nPrevCTFState;

private:
    static const float s_fAutoUpdateTime;

protected:
    CInnerInterface* m_pInterface;
    GLGaeaClient*    m_pGaeaClient;
};

class MyCompetitionNotifyButton : public ICompetitionNotifyButton, private CCompetitionNotifyButton
{
public:
    MyCompetitionNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyCompetitionNotifyButton()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
    virtual void Update();
    virtual void SetLock(const bool _bLock);
    virtual void ClearAlarm();
    virtual void SetAlarm(const DWORD _dwMatchingNum, const char* szText);  // 전장 버튼위에 조그맣게 뜨는 알람 메시지 (ex. 점령전 신청가능);
    virtual void SetTooltip(const DWORD _dwMatchingNum, const char* szText);  // 전장 버튼을 마우스 오버시 나오는 전장의 현 상태 (ex. 2시간후 점령전이 시작됩니다.);
};
