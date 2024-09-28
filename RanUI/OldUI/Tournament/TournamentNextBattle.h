#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/GLCompetition.h"
#include "../../../RanLogic/Msg/GLContrlMatching.h"

#include <boost/function.hpp>

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;

class TournamentNextBattle : public CUIGroup
{
private:
	enum
	{
		CTF_BATTLE_NONE = NO_ID + 1,
		CTF_BATTLE_BUTTON,
	};

public:
	TournamentNextBattle(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~TournamentNextBattle ();

public:
	void CreateSubControl ();

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void            Update ();
	void			OnSetDateTime(DWORD _time);
	void			OnSetStartTime(DWORD _timeH,DWORD _timeM);
	void			OnSetEndTime(DWORD _timeH,DWORD _timeM);
	void			OnSetProgressTime(DWORD _timeH,DWORD _timeM);
	void			OnSetStatus(MatchingSystem::MATCHING_CONTENTS_STATUS _Status);
	void			OnSetView(bool isCanObserve);
	void			OnSetBetting(bool _IsCanTOTO);

public:
	void			SetCTFType( const PVP::ENUM_CTF_TYPE emType ) { m_emCTFType = emType; }

private:
	void			EnableJoinButton( const std::string& strLabel, const bool bEnable );

private:
	CBasicTextBox*  CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign );

private:
	CBasicTextBox*	  m_pTimeStart;
	CBasicTextBox*	  m_pTimeEnd;
	CBasicTextBox*	  m_pTimeProgress;
	CBasicTextBox*	  m_pStatus;
	CBasicTextButton* m_pJoinButton;

	CBasicTextBox*	pTextBox_SUB[7];

private:
	boost::function<void(void)> m_fnCallbackButton;
	PVP::ENUM_CTF_TYPE		m_emCTFType;

protected:
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

};
