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
class RnCDMBattle;
class RnCDMHistory;

class RnCDMPage : public CCompetitionPage
{

public:
	enum
	{
		RNCDM_PAGE = PAGE_NEXT,
		RNCDM_BATTLE,
		RNCDM_HISTORY,
	};

public:
	RnCDMPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~RnCDMPage();

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
	void				 SetMapinfo(std::string	strMapName,
											int nTimeLimit, 
											int nPlayerLimite, 
											int nLevelLimit );
	void				 SetMapImageName(std::string strMapImageName);
	void				 BattelState(int nStartTime, int nEndTime);

protected:
	virtual void         BeginSubControl();
	virtual void         EndSubControl();

private:
	CSwapImage*			 m_pMap;

	CBasicTextBox*		 m_pText_Title;
	CBasicTextBox*		 m_pText_Info;
	CBasicTextBox*		 m_pText_Award;

	RnCDMBattle*			 m_pBattle;
	RnCDMHistory*		 m_pHistory;
public:
	std::string			m_strMapName; //맵이름
	int					m_nTimeLimit; //시간 제한
	int					m_nPlayerLimite; // 진행 인원
	int					m_nLevelLimit; // 참가 가능 레밸

	std::string			m_strMapImageName; //맵이미지 이름


private:
	PVP::ENUM_CTF_TYPE m_emCTFType;

protected:
	CInnerInterface*     m_pInterface;
	GLGaeaClient*        m_pGaeaClient;

};
