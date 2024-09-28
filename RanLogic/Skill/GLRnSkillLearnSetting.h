#pragma once

// 개선된 스킬 UI의 설정파일을 로드하고
// MID, SID를 통해서 스킬을 습득할 수 있는지 여부를 판정합니다.
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
