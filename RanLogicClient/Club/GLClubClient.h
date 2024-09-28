#ifndef _GLOGIC_CLIENT_GLCLUB_CLIENT_H_
#define _GLOGIC_CLIENT_GLCLUB_CLIENT_H_

#pragma once

#include "../../SigmaCore/Database/Sqlite/SQLite.h"
#include "../Widget/GLWidgetScript.h"
#include "../../RanLogic/Club/GLClub.h"

class GLClubClient : public GLClub
{
public:
    GLClubClient();
    virtual ~GLClubClient();

private :
	struct SGuidanceMapInfo
	{
		std::string					strName;
		float						fCommision;
		std::vector< std::string >	vecMapName;

		SGuidanceMapInfo()
			: fCommision( 0.0f )
		{
		}
	};

private:
    //! 정렬조건이 까다로워서 SQLite 를 사용한다
    SQLite::Database m_SQLite;
    GLInventory m_cStorage[MAX_CLUBSTORAGE]; //! 클럽 창고 인벤토리
    bool m_bVALID_STORAGE; //! 클럽 창고 정보를 서버로 부터 받았는가?

    std::vector<std::string> m_vecGuidanceMapName;

	std::vector< SGuidanceMapInfo > m_vecGuidanceMapInfo;

private:
    bool MakeDb();
    void MakeOrderQuery(const std::vector<club::ALIGN_ORDER>& OrderList, std::string& Query);
    void MakeWhereQuery(std::string& Query, bool bOnline);

public:
    //virtual bool SetMemberFlag(DWORD dwCharID, DWORD dwFlags) override;
    virtual void SetMember(const GLCLUBMEMBER& Member) override;
	virtual void SetMemberName(DWORD ChaDbNum, std::string& chaName) override;
    virtual void SetMemberState(DWORD dwCharID, bool bONLINE, int nCHANNEL, const MapID& mapID) override;
    virtual void SetMemberState(DWORD ChaDbNum, const MapID& mapID) override;
    virtual void SetMemberState(DWORD ChaDbNum, WORD ChaLevel) override;
    virtual void SetMemberPos(DWORD dwCharID, const D3DXVECTOR3& vecPOS) override;
    virtual bool SetMemberOffline(DWORD ChaDbNum) override;
    
    virtual void RESET() override;
    virtual bool AddMember(
        DWORD CharDbNum,
        const char* szCharName,
        DWORD dwFlags,
        WORD ChaLevel,
        const __time64_t& LastPlayTime,
        bool bOnline,
        EMCHARCLASS ChaClass) override;
    virtual bool AddMemberEx(DWORD ChaDbNum, const GLCLUBMEMBER& Member) override;

    virtual bool DelMember(DWORD dwCharID) override;

    void SetStorageState(bool State) { m_bVALID_STORAGE = State; }
    bool GetStorageState() const { return m_bVALID_STORAGE; }

    LONGLONG GetIncomeMoney() const { return m_lnIncomeMoney; }
    //void SetIncomeMoney(LONGLONG Money) { m_lnIncomeMoney = Money; }

    LONGLONG GetStorageMoney() const { return m_lnStorageMoney; }
    //void SetStorageMoney(LONGLONG Money) { m_lnStorageMoney = Money; }

    //! 조건에 따라 클럽멤버를 정렬
    //! vResult : 검색 결과    
    //! Align : 정렬 방법/순서    
    //! bool bOnline : online 인 멤버만 가져올 것인가>
    void GetList(std::vector<club::GLCLUBMEMBER_CLIENT>& vResult, const std::vector<club::ALIGN_ORDER>& Align, bool bOnline);
    void RESET_STORAGE();
    
    GLInventory* GetStorage(DWORD Channel);
    SINVENITEM* FindPosItem(DWORD Channel, WORD PosX, WORD PosY);
    
    
    BOOL InsertItem(const SITEMCUSTOM& ItemCustom, DWORD Channel, const WORD wPosX, const WORD wPosY, bool bLOAD=false, bool bLoadCheck=false);
    BOOL InsertItem(const SITEMCUSTOM& ItemCustom, DWORD Channel, const WORD wPosX, const WORD wPosY, const WORD wBackX, const WORD wBackY);    
    BOOL InsertItem(const SITEMCUSTOM& ItemCustom, DWORD Channel, bool Force=false);

    BOOL DeleteItem(DWORD Channel, WORD PosX, WORD PosY);
    void DeleteItemAll();
    void DeleteItemAll(DWORD Channel);
    SINVENITEM* GetItem(DWORD Channel, WORD PosX, WORD PosY);

    BOOL IsInsertable(DWORD Channel, const WORD wPosX, const WORD wPosY, bool bLOAD=false);
    //BOOL IsInsertable(DWORD Channel, const SITEMCUSTOM* pItemCustomArray, const WORD wCount, bool bLOAD=false);

    //! Member online/offline 숫자
    void GetMemberOnOffline(club::MEMBER_ON_OFFLINE& Result) const;

    //! Club member 계급 변경
    virtual bool ChangeMemberGrade(DWORD MemberDbNum, size_t Grade) override;

    void SetGuidanceMapName( std::vector<std::string>& VecGuidanceMapName );
    void GetGuidanceMapName( std::vector<std::string>& vMap );

public:
	LuaTable GetClubMemberList();
	LuaTable GetClubMemberOnOff();
	DWORD GetMemberDBNum(const char* _Name);
	LuaTable GetClubLog(club::EM_LOG_TYPE LogType, WORD Page, WORD PageSize);
	LuaTable GetAutoKickData();

	LuaTable GetGudianceMapInfo();

public :
	void MsgGuidanceMapInfo( NET_MSG_GENERIC* pMsg );
};

#endif // _GLOGIC_CLIENT_GLCLUB_CLIENT_H_