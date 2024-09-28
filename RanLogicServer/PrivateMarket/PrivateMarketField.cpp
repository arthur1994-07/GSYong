#include "pch.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./PrivateMarketField.h"

PrivateMarketField::PrivateMarketField(GLChar* pChar, GLGaeaServer* pServer)
    : m_pChar(pChar)
    , m_pServer(pServer)
{
}

PrivateMarketField::~PrivateMarketField()
{
}

bool PrivateMarketField::DoMarketOpen()
{
    if (m_pServer && m_pChar)
    {
        DWORD ChaDbNum = m_pChar->CharDbNum();
        m_pServer->PrivateMarketOpen(ChaDbNum);

        // 만일 상점을 학원 광장에서 열었다면 아이템 검색 리스트에 추가한다.
		const SNATIVEID& _gaeaMapID(m_pChar->GetCurrentMap().getGaeaMapID());
		if ( _gaeaMapID == SNATIVEID(22, 0) )
        {
            // 아이템 등록
            for (MAPSEARCH_CITER iter=m_mapSearchItem.begin(); iter!=m_mapSearchItem.end(); ++iter)
            {
                const SSEARCHITEMDATA& ItemData = iter->second;
                m_pServer->PrivateMarketAddItem(ChaDbNum, ItemData);
            }
            
        }
    }
    return GLPrivateMarket::DoMarketOpen();
}

void PrivateMarketField::DoMarketClose()
{
    if (m_pServer && m_pChar)
        m_pServer->PrivateMarketClose(m_pChar->CharDbNum());
    return GLPrivateMarket::DoMarketClose();
}

void PrivateMarketField::DoMarketInfoRelease()
{
    if (m_pChar && m_pServer)
        m_pServer->PrivateMarketDelItem(m_pChar->CharDbNum());
    GLPrivateMarket::DoMarketInfoRelease();
}


void PrivateMarketField::FindItem(const SSEARCHITEMDATA& sSearchData, std::vector<SFINDRESULT>& vecFindResult)
{
    bool bFindUseClass = FALSE;
    bool bFindUseType  = FALSE;
    bool bFindUseLevel = FALSE;
    bool bFindUseStats = FALSE;

    if (sSearchData.dwReqCharClass != 0)
        bFindUseClass = TRUE;
    if (sSearchData.dwSuitType != 0)
        bFindUseType  = TRUE;
    if (sSearchData.wReqLevel != 0)
        bFindUseLevel = TRUE;
    if (!sSearchData.sReqStats.IsZERO())
        bFindUseStats = TRUE;

    //std::vector<SFINDRESULT> vecFindResult;
    SFINDRESULT findResult;
    vecFindResult.clear();

    if( m_mapSearchItem.size() == 0 )
        return; // vecFindResult;

    MAPSEARCH_CITER pos = m_mapSearchItem.begin();
    for( ; pos != m_mapSearchItem.end(); ++pos )
    {
        const SSEARCHITEMDATA& itemData = pos->second;

        CString str = itemData.m_ItemName.c_str();
        if( str.Find( sSearchData.m_ItemName.c_str() ) == -1 )
            continue;

        if( bFindUseClass )
        {
            if( !itemData.dwReqCharClass & sSearchData.dwReqCharClass )
                continue;
        }

        if( bFindUseType )
        {
            if( itemData.dwSuitType != sSearchData.dwSuitType )
                continue;
        }

        if( bFindUseLevel )
        {
            if( itemData.wReqLevel < sSearchData.wReqLevel )
                continue;
        }

        if( bFindUseStats )
        {
            if( sSearchData.sReqStats.wPow != 0 && itemData.sReqStats.wPow == 0 )
                continue;
            if( sSearchData.sReqStats.wDex != 0 && itemData.sReqStats.wDex == 0 )
                continue;
            if( sSearchData.sReqStats.wSpi != 0 && itemData.sReqStats.wSpi == 0 )
                continue;
        }

        findResult.sSaleItemID = pos->first;
        findResult.llPRICE	   = itemData.llPRICE;		
        vecFindResult.push_back( findResult );
    }

    //return vecFindResult;
}

bool PrivateMarketField::DoSale(const SNATIVEID& sSALEPOS, DWORD dwNUM, bool bSearchMarket)
{
    MAPITEM_ITER pos = m_mapSALE.find(sSALEPOS.dwID);
    if (pos == m_mapSALE.end())
        return false;

    SSALEITEM& sSALE = (*pos).second;

    if (sSALE.bSOLD)
        return false;
    if (sSALE.dwNUMBER < dwNUM)
        return false;

    sSALE.dwNUMBER -= dwNUM;

    if (0 == sSALE.dwNUMBER)
    {
        sSALE.bSOLD = true;
        if (bSearchMarket)
        {
            SSALEITEM* pSaleItem = GetItem(sSALEPOS);
            if (pSaleItem)
            {
                MAPSEARCH_ITER pos = m_mapSearchItem.find(pSaleItem->sITEMCUSTOM.GetNativeID().dwID);
                if (pos != m_mapSearchItem.end())
                    m_mapSearchItem.erase(pos);
            }
        }

    }
    return true;
}
