#pragma once

#include "./GLGuidance.h"

// �������� ������ [10/29/2014 hsshin];
// ���� ! : �������� �������� �ű� �������� ȥ��� �� ����.;
// �������� ����ϰ� �ʹٸ� �ű� �������� �ش� �δ���ũ��Ʈ�� �ε带 ���� �ʾƾ��Ѵ�.;
// �ű� �������� �������࿡ ���� �κ��� �δ����� ó�� ��;
// ����Ŭ�� ���� ���� �κ��� �������� ����ϱ� ������ ���������� ������ �������� �ִٸ� ����ε� ������ �����ϱ� ��ƴ�.;

// ������� �ű��δ��� �������� ���� ���� �Ǹ� �ش� �ڵ�� �����ؾ���;
// ����� ����Ŭ���� ���� ����κ��� �������� ����� ����ϰ� �־ �����Ұ�;
// �ش�κ��� ���� ����ų� �ű��� �����ؾ���;

class GLGuidanceFieldMan : public GLGuidanceMan
{
public:
    GLGuidanceFieldMan(GLGaeaServer* pServer);
    virtual ~GLGuidanceFieldMan();

private:
    GLGaeaServer* m_pServer;

public:
    bool SetMapState();

    bool BeginBattle(DWORD dwID);
    bool EndBattle(DWORD dwID);

    bool ChangeGuidClub(DWORD dwID, DWORD dwClubID);
    bool ChangeCommission(DWORD dwID, float fRate);

    bool DoCertify(DWORD dwID, DWORD dwCHARID, DWORD dwNpcGlobID, const D3DXVECTOR3 &vPOS);

    void SendWorldBattleNationFlag(GLChar* pChar);

    DWORD GetGuidID(DWORD dwClubID) const;
    EMCHECKCERTIFY CheckCertify(DWORD dwID, DWORD dwCHARID);
    std::string GetName(DWORD dwID);

    void CheckExtraGuild(float fElaps);

    void DoLogPrint(DWORD dwID);

    bool FrameMove(float fElaps);
};
