#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

#include <vector>

class CInnerInterface;
class GLGaeaClient;

class CStudentRecordNotifyButton : public CUIGroup
{
private:
	enum
	{
		NOTIFY_BUTTON = NO_ID + 1,
	};

public:
    typedef std::vector<std::string> VEC_STRING;
    typedef VEC_STRING::iterator     ITER_VEC_STRING;

public:
	CStudentRecordNotifyButton(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CStudentRecordNotifyButton ();

public:
	void	CreateSubControl ();

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
    void            Update ();

private:
    CUIControl*     m_pAlarm;

private:
    bool            m_bAlarm;
    VEC_STRING      m_vecInfo;

protected:
    CInnerInterface* m_pInterface;
    GLGaeaClient*    m_pGaeaClient;
};