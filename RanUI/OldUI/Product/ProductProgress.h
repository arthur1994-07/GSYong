#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class RnProgressBar;

class CProductProgress :public CUIGroup
{
private:
    enum
    {
        ID_PRODUCT_PROGRESS = NO_ID + 1,
        ID_PRODUCT_TEXT_BOX
    };

public:
    CProductProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	void CreateSubControl();
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual bool IsProdution();
    void StartProduct( DWORD nID, WORD nCount );    //제작진행률 표시를 시작한다. (끝낼때는 setvisible(false)를 사용)
	void GoAlongProduct();
	void EndProduct();
	void CancelProduct();

protected:
	CInnerInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;

    CBasicTextBox* m_pProductText;
    RnProgressBar* m_pProgressBar;

	CString m_strProductName;
	WORD m_nCount;
	WORD m_nCurCount;
	float m_fMixDelay;
    float m_fElapsedTime;

	BOOL m_bUIClicked;              // 클릭을 하고 있느냐?
    D3DXVECTOR2 m_vOldMousePos;     // 마우스의 클릭하기 이전 좌표
    UIRECT m_vOldUIPos;             // 마우스 클릭하기 이전의 UI좌표
};

class MyProductProgress : public IProductProgress, private CProductProgress
{
public:
    MyProductProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
    virtual void CreateUIWindowAndRegisterOwnership();
    virtual void StartProduct( DWORD nID, WORD nCount );
	virtual void GoAlongProduct();
	virtual void EndProduct();
	virtual void CancelProduct();
	virtual bool IsProdution();
};