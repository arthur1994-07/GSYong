#pragma once

#include "CompetitionPage.h"
#include "../Util/UIPage.h"
#include "../../../RanLogic/GLCompetition.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

//-----------------------------------------------------------------------------------------------//

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CSwapImage;
class CCTFBattle;
class CCTFHistory;

class CCTFPage : public CCompetitionPage
{

public:
	enum
	{
		CTF_PAGE = PAGE_NEXT,
		CTF_BATTLE,
		CTF_HISTORY,
	};

public:
	CCTFPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CCTFPage();

public:
	void	             CreateSubControl ();

public:
	virtual void         Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void         TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void				 Update();
	void                 UpdateInfo	   ();
	void                 UpdateBattle  ();
	void                 UpdateHistory ();

protected:
	virtual void         BeginSubControl();
	virtual void         EndSubControl();

private:
	CSwapImage*			 m_pMap;
	
	CBasicTextBox*		 m_pText_Title;
	CBasicTextBox*		 m_pText_Info;
	CBasicTextBox*		 m_pText_Award;

	CUIControl*			 m_pComp_SchoolImage[SCHOOL_NUM];

	CCTFBattle*			 m_pBattle;
	CCTFHistory*		 m_pHistory;

private:
	PVP::ENUM_CTF_TYPE m_emCTFType;

protected:
	CInnerInterface*     m_pInterface;
	GLGaeaClient*        m_pGaeaClient;

};
