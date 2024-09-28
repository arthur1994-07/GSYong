#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/GLCompetition.h"

#include <boost/function.hpp>

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;

class RnCDMBattle : public CUIGroup
{
private:
	enum
	{
		RNCDM_BATTLE_NONE = NO_ID + 1,
		RNCDM_BATTLE_BUTTON,
	};

public:
	RnCDMBattle(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~RnCDMBattle ();

public:
	void CreateSubControl ();

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void            Update ();

public:
	void			SetCTFType( const PVP::ENUM_CTF_TYPE emType ) { m_emCTFType = emType; }

private:
	void			EnableJoinButton( const std::string& strLabel, const bool bEnable );

private:
	void            ReqJoin();
	void            ReqCancel();
	void            ReqExit();

private:
	CBasicTextBox*  CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign );

private:
	CBasicTextBox*	  m_pTimeStart;
	CBasicTextBox*	  m_pTimeEnd;
	CBasicTextBox*	  m_pTimeProgress;
	CBasicTextBox*	  m_pStatus;
	CBasicTextButton* m_pJoinButton;
public:
	int m_nStartTime;
	int m_nEndTime;

	void SetBattelState( int nStartTime, int nEndTime );

private:
	boost::function<void(void)> m_fnCallbackButton;
	PVP::ENUM_CTF_TYPE		m_emCTFType;

protected:
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

};
