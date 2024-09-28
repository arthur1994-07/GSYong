#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CSwapImage;

class CSelectCountryNode : public CUIGroup
{
private:
    enum
    {

        NODE_FLAG = NO_ID + 1,
    };

public:
    CSelectCountryNode(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CSelectCountryNode();

protected:
    GLGaeaClient* m_pGaeaClient;
    COuterInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void CreateSubControl();
    void SetCountryInfo( const wb::COUNTRY_INFO& CountryInfo );

    const wb::COUNTRY_INFO& GetCountyInfo();

private:
    CSwapImage* m_pFlag;

private:
    wb::COUNTRY_INFO m_CountryInfo;
};