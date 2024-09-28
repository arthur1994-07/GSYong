#ifndef _RAN_LOGIC_WORLD_BATTLE_COUNTRY_DEFINE_H_
#define _RAN_LOGIC_WORLD_BATTLE_COUNTRY_DEFINE_H_

#pragma once

#include <string>
#include <map>
#include <vector>
#include "../../SigmaCore/Database/Sqlite/SQLite.h"
#include "WorldBattleDefine.h"

enum EMSERVICE_PROVIDER;

//! namespace world battle 2012-02-08 jgkim
namespace wb
{

//! class Country 2012-02-08 jgkim
class Country
{
public:
    Country();
    ~Country();

protected:
    WORD m_TextureWidth;
    WORD m_TextureHeight;

    WORD m_FlagWidth;
    WORD m_FlagHeight;
    
    typedef std::map<EM_COUNTRY, COUNTRY_INFO> COUNTRY_DATA;
    typedef COUNTRY_DATA::iterator       COUNTRY_DATA_ITER;
    typedef COUNTRY_DATA::const_iterator COUNTRY_DATA_CITER;
    typedef COUNTRY_DATA::value_type     COUNTRY_DATA_VALUE;

    COUNTRY_DATA m_Data; //! ���� ����

	// Effect �̸�, ���� ���� ����
    typedef std::map<std::string, DWORD>		MAP_EFFNAME_GUID;
	typedef MAP_EFFNAME_GUID::iterator			MAP_EFFNAME_GUID_ITER;
	typedef MAP_EFFNAME_GUID::const_iterator	MAP_EFFNAME_GUID_CITER;
	typedef MAP_EFFNAME_GUID::value_type		MAP_EFFNAME_GUID_VALUE;

	// �� Effect ���� ��� ���� ������ ��ǥ�ϴ� �͵����� �����س��� ��.
	MAP_EFFNAME_GUID	m_mapEffNameGUID;
    
    //! ���� �� �˻��� ���ؼ� SQLite �� ����Ѵ�
    SQLite::Database m_SQLite;

private:
    bool MakeDbFile();

    bool InsertInfo(
        EM_COUNTRY CountryUniqueCode,
        const std::wstring& CountryFullName,
        const std::wstring& Country2Name,
        const std::wstring& Country3Name,
        const std::wstring& CountryDesc,
        const std::wstring& NationalAnthem,
        const std::wstring& FlagImageFile,
        WORD PosX,
        WORD PosY);

public:
    bool Load(EMSERVICE_PROVIDER Sp, const std::string& FileName);
    
    //! ���������� �˻��Ѵ�
    void Search(const std::string& SearchWord, std::vector<COUNTRY_INFO>& Result);
    void Search(std::vector<COUNTRY_INFO>& Result);

    //! ���� ������ �����´�
    const COUNTRY_INFO& GetData(EM_COUNTRY CountryCode) const;
    bool IsExist(EM_COUNTRY CountryCode) const;

    //! ���� ��ü �ؽ��� ��/����
    inline WORD GetTextureWidth() const { return m_TextureWidth; }
    inline WORD GetTextureHeight() const { return m_TextureHeight; }

    //! ���� �̹���
    const std::string& GetFlagTexture(EM_COUNTRY CountryCode) const;
    
    //! ���� �̹���. ���� ��/����
    inline WORD GetFlagWidth() const { return m_FlagWidth; }
    inline WORD GetFlagHeight() const { return m_FlagHeight; }
    
    //! �̹���. ���� ��ġ    
    const FLAG_TEXTURE_POSITION& GetFlagTexturePos(EM_COUNTRY CountryCode) const;
    
    //! ����/������ Ŭ������
    void SetCountryClubInfo(EM_COUNTRY CountryCode, WORD ClubCount);

	//! Effect �� ��� ���� ������ ��ǥ�ϴ°�~?
	BOOL GetGuidanceID(const std::string& strEffName, DWORD& dwGuidanceID_OUT) const;
};

} // namespace wb

#endif // _RAN_LOGIC_WORLD_BATTLE_COUNTRY_DEFINE_H_