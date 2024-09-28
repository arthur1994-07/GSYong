#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Competition/PVEIndicator.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../InnerInterface.h"

class CUINumberTimer;
class CUIStageIndicator;
class EngineDeviceMan;
class CTournamentExpeditionInfoGroup;
class TournamentMiniExpeditionWindow;

class CTournamentIndicator
	: public CPVEIndicator
{
protected:
	enum
	{
		HP_BAR_LEFT = NO_ID + 1,
		HP_BAR_RIGHT = NO_ID + 2,
		SINGLE_LEFT_ID_TEXT_BOX = NO_ID + 3,
		SINGLE_RIGHT_ID_TEXT_BOX = NO_ID + 4,

		TEAM_LEFT_UP_ID_TEXT_BOX = NO_ID + 5,
		TEAM_LEFT_DWON_ID_TEXT_BOX = NO_ID + 6,
		TEAM_RIGHT_UP_ID_TEXT_BOX = NO_ID + 7,
		TEAM_RIGHT_DWON_ID_TEXT_BOX = NO_ID + 8,

		HP_HUD_LEFT_SLOT_WINCOUNT_0 = NO_ID + 9,
		HP_HUD_LEFT_SLOT_WINCOUNT_1 = NO_ID + 10,
		HP_HUD_LEFT_SLOT_WINCOUNT_2 = NO_ID + 11,
		HP_HUD_RIGHT_SLOT_WINCOUNT_0 = NO_ID + 12,
		HP_HUD_RIGHT_SLOT_WINCOUNT_1 = NO_ID + 13,
		HP_HUD_RIGHT_SLOT_WINCOUNT_2 = NO_ID + 14,

	};

public:
		CTournamentIndicator(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CTournamentIndicator();


		/**
			���� ��Ʈ�ѵ��� ���� �Ѵ�.
			�츮�� 2�� �ؽ�Ʈ�ڽ��� ���� �Ѵ�.
			�ϳ��� ��� �ؽ�Ʈ �ڽ��� �������� ���� ǥ�� �Ѵ�.
			�ϳ��� �ϴ� �ؽ�Ʈ �ڽ��� �����鿡�� �÷��̽� ����� �޽����� ǥ�� �Ѵ�.

		 */
virtual bool				CreateSubControl();

		/**
			�������� ���� �ð��� �Է� �Ѵ�.

			\param nSec �ʴ��� �Է�
		 */
		void				SetTime(int nSec, bool bBlank, bool bShow);
		void				SetGameType( DWORD PlayerCount, DWORD FactionCount );

		void				SetPlayerCount(DWORD PlayerCount,BOOL IsShow);
		void				SetGameWinLoseCount( DWORD PlayerCount, DWORD FactionCount );
		int					GetTime();
		CBasicTextBox*		CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID cID = NO_ID );
		CBasicLineBoxSmart* CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID = NO_ID  );

		void				SetOneByOne();
		void				SetNbyN();
        void                SetTeamVSTeam();

        void                SetTeamVsTeamInfo( const NET_MSG_GENERIC* const _pMessage );

		void				UpdateOneByOne();
        void                UpdateTeamVSTeam(const int _x, const int _y, const float _elapsedTime);



		/**
			������Ʈ ����.
		 */
		virtual				void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );


protected:
    D3DXVECTOR2                 m_vPos;
	
	CSwapImage*					pLeftIMAGECLASS;		//@ Image
	CSwapImage*					pRightIMAGECLASS;		//@ Image

	CSwapImage*					pLeftIMAGESCHOOL;		//@ Mini Image
	CSwapImage*					pRightIMAGESCHOOL;		//@ Mini Image

	CBasicProgressBar*			m_pLeftHP;				//@ HP
	CBasicProgressBar*			m_pRightHP;				//@ HP

	CBasicTextBox*				pLftBOXText;			//@ 
	CBasicTextBox*				pRightBOXText;			//@ 

	CBasicLineBoxSmart*			pLftBOX;				//@ 
	CBasicLineBoxSmart*			pRightBOX;				//@ 

	CSwapImage*					m_pTournamentRight[5];	//@ ������ OOO ǥ��
	CSwapImage*					m_pTournamentLeft[5];	//@ ���� OOO ǥ��

	CBasicLineBoxSmart*			TOURNAMENT_LEFT_TEAMNAME_BOX;				//@ 
	CBasicLineBoxSmart*			TOURNAMENT_LEFT_TEAMNAME_BOX_BG;				//@
	CBasicLineBoxSmart*			TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN;				//@

	CBasicTextBox*			    TOURNAMENT_LEFT_TEAMNAME_BOX_UP_TB;				//@
	CBasicTextBox*			    TOURNAMENT_LEFT_TEAMNAME_BOX_DOWN_TB;				//@

	CBasicLineBoxSmart*			TOURNAMENT_RIGHT_TEAMNAME_BOX;				//@ 
	CBasicLineBoxSmart*			TOURNAMENT_RIGHT_TEAMNAME_BOX_BG;				//@
	CBasicLineBoxSmart*			TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN;				//@

	CBasicTextBox*			    TOURNAMENT_RIGHT_TEAMNAME_BOX_UP_TB;				//@
	CBasicTextBox*			    TOURNAMENT_RIGHT_TEAMNAME_BOX_DOWN_TB;				//@

    // �� �� �� UI;
    CSwapImage*					pLeftTeamIcon;		// ���� ��� team ������;
    CSwapImage*					pRightTeamIcon;		// ������ ��� team ������;

    CSwapImage*					pLeftTeamImage;		// ���� team �߰� �̹���;
    CSwapImage*					pRightTeamImage;	// ������ team �߰� �̹���;

    CBasicTextBox*				pTeamObeject;		// ���� ��� ��ǥ,���� ���� �ؽ�Ʈ;

    CUIGroup*   				pTeamVsTeamWindowBox;  // �� �� �� UI���� �巡�� ��ġ ������ ���� ������ �ڽ�;

	/**
		�ܺ� �ǵ��� Ŭ������

	 */
	GLGaeaClient*		m_pGaeaClient;
	CInnerInterface*	m_pInterface;

	/**
		PVP ���� 
	*/
	DWORD LeftDBID;
	float LeftIdPersent;
	DWORD RightDBID;
	float RightIdPersent;

	bool m_bNotFound;

    float fBlinkTimer;

	std::vector<DWORD> vec_factionlist ;
};
