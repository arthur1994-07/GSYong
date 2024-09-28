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

//! ������ ��ü������ �����´�
void GLogicData::wbSearch(std::vector<wb::COUNTRY_INFO>& Result)
{
    m_pWbCountry->Search(Result);
}

bool GLogicData::wbIsExistCountry(wb::EM_COUNTRY CountryCode) const
{
    return m_pWbCountry->IsExist(CountryCode);
}

//! ���� ������ �����´�
const wb::COUNTRY_INFO& GLogicData::wbGetData(wb::EM_COUNTRY CountryCode) const
{
    return m_pWbCountry->GetData(CountryCode);
}

//! ���� ��ü �ؽ��� ��/����
WORD GLogicData::wbGetTextureWidth() const
{
    return m_pWbCountry->GetTextureWidth();
}

WORD GLogicData::wbGetTextureHeight() const
{
    return m_pWbCountry->GetTextureHeight();
}

//! ���� �̹���
const std::string& GLogicData::wbGetFlagTexture(wb::EM_COUNTRY CountryCode) const
{ 
    return m_pWbCountry->GetFlagTexture(CountryCode);
}

//! ���ػ� �̹���. ���� ��/����
WORD GLogicData::wbGetFlagWidth() const 
{ 
    return m_pWbCountry->GetFlagWidth();
}

WORD GLogicData::wbGetFlagHeight() const
{
    return m_pWbCountry->GetFlagHeight();
}

//! ���� �̹���. ���� ��ġ
const wb::FLAG_TEXTURE_POSITION& GLogicData::wbGetFlagTexturePos(wb::EM_COUNTRY CountryCode) const
{
    return m_pWbCountry->GetFlagTexturePos(CountryCode);
}

//! ����/������ Ŭ������
void GLogicData::wbSetCountryInfo(wb::EM_COUNTRY CountryCode, WORD ClubCount)
{
    m_pWbCountry->SetCountryClubInfo(CountryCode, ClubCount);
}

//! Effect �� ��� ���� ������ ��ǥ�ϴ°�~?
BOOL GLogicData::wbGetGuidanceID( const std::string& strEffName, DWORD& dwGuidanceID_OUT ) const
{
	return m_pWbCountry->GetGuidanceID( strEffName, dwGuidanceID_OUT );
}