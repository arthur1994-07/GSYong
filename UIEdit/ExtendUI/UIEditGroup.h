#pragma	once

#include "../../EngineLib/GUInterface/UIGroup.h"

#include <vector>

class CBasicLineBox;
class CInterfaceAdmin;

class CUIEditGroup : public CUIGroup
{
private:
	enum
	{
		CONTROL_NORMAL,
		CONTROL_SELECT,
		CONTROL_PARENT,
	};
//
public:
	CUIEditGroup (EngineDeviceMan* pEngineDevice, CInterfaceAdmin* pInterfaceAdmin);
	virtual	~CUIEditGroup ();

protected:
    CInterfaceAdmin* m_pInterfaceAdmin;

public:
	virtual void ResizeWindow(const UIRECT& rcNew) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void SetGlobalPos(const UIRECT& Pos);
	virtual void SetGlobalPos(const D3DXVECTOR2& vPos);
	virtual void SetLocalPos(const UIRECT& Pos);
	virtual void SetLocalPos(const D3DXVECTOR2& vPos);
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void AlignMainControlEX ( const UIRECT& rcLastParentPos, const UIRECT& rcNewParentPos );
	virtual void AlignSubControlEX( const UIRECT& rcLastParentPos, const UIRECT& rcNewParentPos );


public:
	virtual void CreateWindowControl (const TCHAR* szControl);
	virtual void ResizeControl (const TCHAR* szControl);
	virtual void CheckMousestate();
	virtual void SetSelected(int nSel);

	virtual int FindChild(UIGUID ControlID);
	virtual void AddChild(UIGUID ControlID);
	virtual void DelChild(UIGUID ControlID);

public:
	void SetLineView( BOOL bView ) { m_bLineView = bView; }
	BOOL GetLineView() { return m_bLineView; }

public:
	std::vector<UIGUID> m_ChildID;
	BOOL			m_nSelected;

	BOOL			m_bFirstGap;
	D3DXVECTOR2		m_vGap;

	int				m_PosX;
	int				m_PosY;

	BOOL			m_bLineView;

	CBasicLineBox* m_pSelLineBox;
};