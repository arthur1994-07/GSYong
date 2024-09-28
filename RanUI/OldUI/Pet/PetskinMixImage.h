#pragma once

#include "../../../EngineLib/guinterface/uigroup.h"
#include "../../../EngineLib/GUInterface/BasicAnimationBox.h"
#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CBasicTextButton;
class GLGaeaClient;

class CPetSkinMixImage : public CUIWindowEx
{
protected:
	enum
	{
		PETSKIN_MIX_IMAGE = NO_ID + 1,
	};

public:
	CPetSkinMixImage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CPetSkinMixImage(void);

public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );
	inline void	SetItemData(WORD wPosX, WORD wPosY, SNATIVEID sItemData, WORD wHoldPosX, WORD wHoldPosY) 
	{ 
		m_ItemPosX  = wPosX; 
		m_ItemPosY  = wPosY; 
		m_sItemID	= sItemData;

        m_HoldPosX = wHoldPosX;
        m_HoldPosY = wHoldPosY;
	}


public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
    GLGaeaClient* m_pGaeaClient;
	CBasicAnimationBox* m_pAniBox;	

	WORD m_ItemPosX;
	WORD m_ItemPosY;
	SNATIVEID m_sItemID;
    WORD m_HoldPosX;
    WORD m_HoldPosY;
};
