
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

#include "../../../RanLogic/GLogicEx.h"
#include "StatsItem.h"

class CStatsInfo : public CUIGroup
{

protected:
	enum
	{
		STATSINFO_NONE = NO_ID + 1,

		STATSINFO_ITEM_POW,
		STATSINFO_ITEM_DEX,
		STATSINFO_ITEM_SPI,
		STATSINFO_ITEM_STR,
		STATSINFO_ITEM_STA,
	};

public:
	CStatsInfo(EngineDeviceMan* pEngineDevice);
	virtual	~CStatsInfo();

public:
	void			Reset        ( const SCHARSTATS& sBasic, const SCHARSTATS& sBasicAdd );
	void			SetBasicData ( const SCHARSTATS& sBasic, const SCHARSTATS& sBasicAdd );
	void			SetAdd       ( const SCHARSTATS& sCurAdd );

public:
	void			SetType ( const DWORD dwType );

public:
	void			LockDownButton   ();
	void			UnlockDownButton ();

public:
	void			GetDecreaseStats ( WORD* pOutStats );

public:
	void			CreateSubControl();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CStatsItem*		m_pItem [EMSIZE];

	SCHARSTATS		m_sStatsBasic;
	SCHARSTATS		m_sStatsBasicAdd;
};
