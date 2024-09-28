#pragma once

// ������ ��ų UI�� ���������� �ε��ϰ�
// MID, SID�� ���ؼ� ��ų�� ������ �� �ִ��� ���θ� �����մϴ�.
// 
struct SLEARN_SETTING
{
    WORD        wMID;
    WORD        wSID;
    bool        bUseScrool;
    DWORD       dwUseMoney;
};

class GLRnSkillLearnSetting
{
public:
    bool                LoadFile( std::string strFileName );
    const DWORD         GetUseMoney( WORD wMID, WORD wSID );
    const bool          GetUseScrool( WORD wMID, WORD wSID );

private:
    std::vector<SLEARN_SETTING> m_vecSetting;
};
