#pragma once

class GLGaeaServer;

class PrivateMarketMan
{
public:
    PrivateMarketMan(GLGaeaServer* pServer);
    ~PrivateMarketMan();

protected:
    GLGaeaServer* m_pGaeaServer;

public:
    //! 개인상점을 닫았음
    void CloseMarket(DWORD ChaDbNum);

    //! 개인상점을 열었음
    void OpenMarket(DWORD ChaDbNum);

    void AddItem(DWORD ChaDbNum, const SSEARCHITEMDATA& Data);
    void DelItem(DWORD ChaDbNum);
};