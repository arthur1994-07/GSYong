#pragma once

#include "./GLGuidance.h"

// [10/28/2014 hsshin];
// �������� �������Դϴ�.;
// �������� ������ [10/29/2014 hsshin];
// ���� ! : �������� �������� �ű� �������� ȥ��� �� ����.;
// �������� ����ϰ� �ʹٸ� �ű� �������� �ش� �δ���ũ��Ʈ�� �ε带 ���� �ʾƾ��Ѵ�.;
// �ű� �������� �������࿡ ���� �κ��� �δ����� ó�� ��;
// ����Ŭ�� ���� ���� �κ��� �������� ����ϱ� ������ ���������� ������ �������� �ִٸ� ����ε� ������ �����ϱ� ��ƴ�.;

// ������� �ű��δ��� �������� ���� ���� �Ǹ� �ش� �ڵ�� �����ؾ���;
// ����� ����Ŭ���� ���� ����κ��� �������� ����� ����ϰ� �־ �����Ұ�;
// �ش�κ��� ���� ����ų� �ű��� �����ؾ���;


class GLGuidanceAgentMan : public GLGuidanceMan
{
public:
    GLGuidanceAgentMan(GLAgentServer* pServer);
    virtual ~GLGuidanceAgentMan();

protected:
    GLAgentServer*      m_pServer;
    bool                m_bDisableOldGuidance;

public:
    void                DisableOldGuidance(const bool _bDisable) { m_bDisableOldGuidance = _bDisable; };  // ������ �������� ����Ŭ�� ���������� ����;
    bool                ChangeGuidClub_RenewalGuidance(DWORD dwID, DWORD dwClubID);  // �ű� ���������� ����Ŭ�� ���� �Լ�;
    bool                ChangeGuidClub(DWORD dwID, DWORD dwClubID);  // �������� ���������� ����Ŭ�� ���� �Լ�;
    bool                ChangeCommission(DWORD dwID, float fRate);

    GLGuidance*         FindByClubID(DWORD dwClubID);

    bool                SetMapState();

    DWORD               GetGuidID(DWORD dwClubID) const;
    DWORD               GetNumGuid() const { return (DWORD) m_vecGuidance.size(); };
    
    std::string         GetGuidName( DWORD GuidNum ) const { return m_vecGuidance[GuidNum].m_strName; };

    bool                FrameMove(float fElapsedAppTime);

    void                SendInfo(DWORD ClientSlot);
    void                SendGuidanceMapInfo( DWORD ClientSlot, DWORD ClubDbNum );
    void                SendGuidBattle_RemainTime(DWORD ClientSlot);                                    //������ ������� �����ð��� ���� ��Ŷ���� (���̵��õ��� ��Ȳ���� �ش� �������Ը�)
//protected:           
    void                SendGuidBattle_RemainTime_BRD();                                                //������ ������� �����ð��� ���� ��Ŷ���� (������ ������ ��ȭ������ �� ��������)
    bool                MakeGuidBattle_RemainTime_Packet( msgpack::sbuffer& PackBuffer);                //������ ������� ���� �ð��� ���� ��Ŷ�� ����
};
