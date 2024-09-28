#pragma once

#include "./GLGuidance.h"

// 구버전의 선도전 [10/29/2014 hsshin];
// 주의 ! : 구버전의 선도전은 신규 선도전과 혼용될 수 없다.;
// 구버전을 사용하고 싶다면 신규 선도전은 해당 인던스크립트를 로드를 하지 않아야한다.;
// 신규 선도전은 게임진행에 대한 부분을 인던에서 처리 후;
// 선도클럽 보상에 대한 부분은 구버전을 사용하기 때문에 구버전으로 열리는 선도전이 있다면 제대로된 동작을 보장하기 어렵다.;

// 리뉴얼된 신규인던의 선도전이 전부 들어가게 되면 해당 코드는 삭제해야함;
// 현재는 선도클럽의 대한 보상부분은 구버전의 기능을 사용하고 있어서 삭제불가;
// 해당부분을 새로 만들거나 옮긴후 삭제해야함;

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
