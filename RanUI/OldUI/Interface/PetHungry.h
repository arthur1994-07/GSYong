#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Display/MiniGauge.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/Pet/GLPet.h"

class	CInnerInterface;
class	CMiniGauge;
struct	SITEMCUSTOM;
class	CItemImage;
class GLGaeaClient;

class CPetHungry : public CUIGroup
{
public:
	CPetHungry(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CPetHungry(void);

protected:
    GLGaeaClient* m_pGaeaClient;

	enum
	{
		PET_HUGNRY_HUNGRY		= NO_ID + 1,
		PET_HUNGRY_GAUGE		= NO_ID + 2,
		PET_HUNGRY_IMAGE		= NO_ID + 3,
		PET_HUNGRY_INFO			= NO_ID + 4,
	};

public:
	void	CreateSubControl ();

	void	SetPetHungry();
	void	ResetPetHungry();

	void	SetPetItem( SNATIVEID sPetItemID );

    void PetSatiationUpdate( int Satiation );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl*			m_pHungry;
	CMiniGauge*			m_pGauge;
	CItemImage*			m_pSlotImage;

	bool			m_bAlarm;
	float			m_fBLINK_TIME;
	float			m_Percent;

protected:
	CInnerInterface* m_pInterface;

};
