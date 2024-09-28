#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CItemImage;
class GLGaeaClient;

class CItemMove : public CUIGroup
{
protected:
static const DWORD	dwDEFAULT_TRANSPARENCY;
static const float	fDEFAULT_MOUSE_INTERPOLIATION;

public:
	CItemMove(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CItemMove ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void	SetItemIcon( const SITEMCUSTOM& sItemCustom );
	SNATIVEID	GetItemIcon();
	const CString&	GetItemIconTexture() const;
	void	ResetItemIcon();

public:
	SNATIVEID	GetItem();

private:
	CItemImage*	m_pItemImage;

private:
	BOOL	m_bUseSnap;

protected:
	CInnerInterface* m_pInterface;

};

class MyItemMove : public IItemMove, private CItemMove
{
public:
    MyItemMove(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~MyItemMove()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual void SetUIWindowGlobalPos( const UIRECT& rcPos );
    virtual SNATIVEID GetItem();
};