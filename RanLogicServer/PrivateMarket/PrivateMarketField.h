#pragma once

#include "../../RanLogic/Market/GLPrivateMarket.h"

class GLChar;
class GLGaeaServer;

class PrivateMarketField : public GLPrivateMarket
{
public:
    PrivateMarketField(GLChar* pChar, GLGaeaServer* pServer);
    virtual ~PrivateMarketField();

protected:
    GLChar* m_pChar;
    GLGaeaServer* m_pServer;

public:
    virtual bool DoMarketOpen() override;
    virtual void DoMarketClose() override;
    virtual void DoMarketInfoRelease() override;

    void FindItem(const SSEARCHITEMDATA& sSearchData, std::vector<SFINDRESULT>& vecFindResult);
    
    const MAPSEARCH& GetSearchItemList() const { return m_mapSearchItem; }	

    bool DoSale(const SNATIVEID& sSALEPOS, DWORD dwNUM, bool bSearchMarket=FALSE);
};