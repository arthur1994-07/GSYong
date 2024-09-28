#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../RanLogic/GLCompetition.h"

#include <boost/function.hpp>

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;

class TournamentComboBox : public CUIGroup
{
private:
	enum
	{
		CONTROL_ID_NONE = NO_ID + 1,
		CONTROL_ID_COMBO_BUTTON,
		CONTROL_ID_COMBO_ROLLOVER,
	};

public:
	TournamentComboBox(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~TournamentComboBox ();

public:
	void CreateSubControl ();
public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
public:
	int AddText(const CString& str, bool bVisibleScroll = FALSE, bool bOneLine = FALSE, const D3DCOLOR& TextColor = NS_UITEXTCOLOR::WHITE );
	void ClearText();
	//CString	GetSelectText ( int nIndex );

	void SetSelectText(int nIndex);
	int GetSelectIndex() { return m_nCurrentFilter; }
	int GetMaxIndex() { return m_MaxSize; }


private:
	CUIControl* CreateLineBox( const std::string& strControl, const std::string& strTexInfo ,UIGUID id = NO_ID);

	boost::function<void(void)> m_fnCallbackButton;
private:
	CBasicTextBox* m_pComboBoxText;
	CBasicComboBoxRollOver* m_pComboBoxRollOver;
	int m_nCurrentFilter;
	int m_MaxSize ;


protected:
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

};
