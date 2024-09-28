#pragma once

#include "./GLGuidance.h"

// [10/28/2014 hsshin];
// 구버전의 선도전입니다.;
// 구버전의 선도전 [10/29/2014 hsshin];
// 주의 ! : 구버전의 선도전은 신규 선도전과 혼용될 수 없다.;
// 구버전을 사용하고 싶다면 신규 선도전은 해당 인던스크립트를 로드를 하지 않아야한다.;
// 신규 선도전은 게임진행에 대한 부분을 인던에서 처리 후;
// 선도클럽 보상에 대한 부분은 구버전을 사용하기 때문에 구버전으로 열리는 선도전이 있다면 제대로된 동작을 보장하기 어렵다.;

// 리뉴얼된 신규인던의 선도전이 전부 들어가게 되면 해당 코드는 삭제해야함;
// 현재는 선도클럽의 대한 보상부분은 구버전의 기능을 사용하고 있어서 삭제불가;
// 해당부분을 새로 만들거나 옮긴후 삭제해야함;


class GLGuidanceAgentMan : public GLGuidanceMan
{
public:
    GLGuidanceAgentMan(GLAgentServer* pServer);
    virtual ~GLGuidanceAgentMan();

protected:
    GLAgentServer*      m_pServer;
    bool                m_bDisableOldGuidance;

public:
    void                DisableOldGuidance(const bool _bDisable) { m_bDisableOldGuidance = _bDisable; };  // 구버전 선도전의 선도클럽 결정권한을 막음;
    bool                ChangeGuidClub_RenewalGuidance(DWORD dwID, DWORD dwClubID);  // 신규 선도전에서 선도클럽 설정 함수;
    bool                ChangeGuidClub(DWORD dwID, DWORD dwClubID);  // 구버전의 선도전에서 선도클럽 설정 함수;
    bool                ChangeCommission(DWORD dwID, float fRate);

    GLGuidance*         FindByClubID(DWORD dwClubID);

    bool                SetMapState();

    DWORD               GetGuidID(DWORD dwClubID) const;
    DWORD               GetNumGuid() const { return (DWORD) m_vecGuidance.size(); };
    
    std::string         GetGuidName( DWORD GuidNum ) const { return m_vecGuidance[GuidNum].m_strName; };

    bool                FrameMove(float fElapsedAppTime);

    void                SendInfo(DWORD ClientSlot);
    void                SendGuidanceMapInfo( DWORD ClientSlot, DWORD ClubDbNum );
    void                SendGuidBattle_RemainTime(DWORD ClientSlot);                                    //선도전 종료까지 남은시간에 대한 패킷전송 (맵이동시등의 상황에서 해당 유저에게만)
//protected:           
    void                SendGuidBattle_RemainTime_BRD();                                                //선도전 종료까지 남은시간에 대한 패킷전송 (선도전 정보의 변화로인해 전 유저에게)
    bool                MakeGuidBattle_RemainTime_Packet( msgpack::sbuffer& PackBuffer);                //선도전 종료까지 남은 시간에 대한 패킷을 만듬
};
