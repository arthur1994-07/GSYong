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
    //! ���λ����� �ݾ���
    void CloseMarket(DWORD ChaDbNum);

    //! ���λ����� ������
    void OpenMarket(DWORD ChaDbNum);

    void AddItem(DWORD ChaDbNum, const SSEARCHITEMDATA& Data);
    void DelItem(DWORD ChaDbNum);
};