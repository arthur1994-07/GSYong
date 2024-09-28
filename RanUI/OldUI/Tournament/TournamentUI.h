#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/GLogicData.h"

#include "../../InnerInterface.h"
#include "../Competition/UINumberTimer.h"

//-----------------------------------------------------------------------------------------------//

#include <string>
#include <list>

//-----------------------------------------------------------------------------------------------//

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicLineBoxSmart;
class CSwapImage;

class TournamentUI : public CUIGroup
{
public:
	TournamentUI(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~TournamentUI ();

public:
	void                        CreateSubControl ();

public:
	virtual	void	            Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

protected:
	virtual void                SetVisibleSingle( BOOL bVisible );

public:
	void                        Show ( const UI::EMTOURNAMENTDISPLAY_TYPE emType, const std::string& strText );
	void                        Begin ();
	void                        Hide ();
	void                        Update ();

	void						SetTime(int nSec, bool bBlank, bool bShow);

private:
	void                        ResizeControl ( const UI::EMTOURNAMENTDISPLAY_TYPE emType );

private:

public:
	void                        Begin_Tournament_Win();
	void                        Begin_Tournament_Lose();
private:
	

	void                        Update_CTF ( float fElapsedTime );

private:
	
	// CTF 
	struct CTF
	{
		CSwapImage*  pBack;
		CSwapImage*  pSchool;

		CTF () 
			: pBack(NULL)
			, pSchool(NULL)
		{
		}

		void SetVisibleSingle( BOOL bVisible );

	} m_CTF;



private:

private:
	std::string                 m_strPosition;
private:
	float                       m_fAniTime;

protected:
	CInnerInterface*            m_pInterface;
	GLGaeaClient*               m_pGaeaClient;
};

//----------------------------------------------------------------------------------------------------//


class MyTournamentUI : public ITournamentWindowUI, private TournamentUI
{
public:
	MyTournamentUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyTournamentUI() {};

public:
	virtual void OnCreate();
	virtual void OnClose() {};
	virtual void OnVisible(bool visible);
	virtual void OnUpdate();
	virtual bool GetVisible() {return false;}
	virtual void OnFrameMove() {};
	virtual void WinLoseScoreVisible(bool visible);
	virtual const UIRECT& GetGlobalPos(); 
};
