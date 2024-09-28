#include "pch.h"
#include "./WorldBattle/WorldBattleCountry.h"
#include "./GLogicDataMan.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLogicData::wbSearch(const std::string& SearchWord, std::vector<wb::COUNTRY_INFO>& Result)
{
    m_pWbCountry->Search(SearchWord, Result);
}

//! 설정된 전체국가를 가져온다
void GLogicData::wbSearch(std::vector<wb::COUNTRY_INFO>& Result)
{
    m_pWbCountry->Search(Result);
}

bool GLogicData::wbIsExistCountry(wb::EM_COUNTRY CountryCode) const
{
    return m_pWbCountry->IsExist(CountryCode);
}

//! 국가 정보를 가져온다
const wb::COUNTRY_INFO& GLogicData::wbGetData(wb::EM_COUNTRY CountryCode) const
{
    return m_pWbCountry->GetData(CountryCode);
}

//! 국기 전체 텍스쳐 폭/높이
WORD GLogicData::wbGetTextureWidth() const
{
    return m_pWbCountry->GetTextureWidth();
}

WORD GLogicData::wbGetTextureHeight() const
{
    return m_pWbCountry->GetTextureHeight();
}

//! 국기 이미지
const std::string& GLogicData::wbGetFlagTexture(wb::EM_COUNTRY CountryCode) const
{ 
    return m_pWbCountry->GetFlagTexture(CountryCode);
}

//! 저해상도 이미지. 국기 폭/높이
WORD GLogicData::wbGetFlagWidth() const 
{ 
    return m_pWbCountry->GetFlagWidth();
}

WORD GLogicData::wbGetFlagHeight() const
{
    return m_pWbCountry->GetFlagHeight();
}

//! 국기 이미지. 국기 위치
const wb::FLAG_TEXTURE_POSITION& GLogicData::wbGetFlagTexturePos(wb::EM_COUNTRY CountryCode) const
{
    return m_pWbCountry->GetFlagTexturePos(CountryCode);
}

//! 국가/국가별 클럽숫자
void GLogicData::wbSetCountryInfo(wb::EM_COUNTRY CountryCode, WORD ClubCount)
{
    m_pWbCountry->SetCountryClubInfo(CountryCode, ClubCount);
}

//! Effect 가 어느 선도 구역을 대표하는가~?
BOOL GLogicData::wbGetGuidanceID( const std::string& strEffName, DWORD& dwGuidanceID_OUT ) const
{
	return m_pWbCountry->GetGuidanceID( strEffName, dwGuidanceID_OUT );
}