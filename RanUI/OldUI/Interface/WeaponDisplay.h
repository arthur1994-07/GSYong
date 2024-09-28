#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

class CInnerInterface;
class CItemImage;
class CBasicTextBox;
class GLGaeaClient;

class CWeaponDisplay : public CUIGroup
{
private:
	CItemImage*		m_pWeaponImage;
	CBasicTextBox*	m_pWeaponText;
	SNATIVEID		m_sNativeID;
	CString			m_sWeaponStr;
	CUIControl*		m_pDummyControl;
public:
	CWeaponDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CWeaponDisplay();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();
public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
protected:
    void    SetItem ( const SITEMCUSTOM& sItemCustom );
	void	ResetItem ();

	void SetItemNativeID ( const SNATIVEID& sNativeID )		{ m_sNativeID = sNativeID; }
	const SNATIVEID& GetItemID () const						{ return m_sNativeID; }

protected:
	CInnerInterface* m_pInterface;
};