#include "pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../RanLogic/Club/ClubAuthGrade.h"
#include "../../RanLogic/Msg/GuidanceMsg.h"
#include "../../RanLogicClient/GLGaeaClient.h"

#include "./GLClubClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLClubClient::GLClubClient()
    : m_bVALID_STORAGE(false)
{
    MakeDb();
    m_vecGuidanceMapName.clear();
}

GLClubClient::~GLClubClient()
{
    if (m_SQLite.IsOpen())
        m_SQLite.Close(); 
}

bool GLClubClient::MakeDb()
{
    // memory db 로 관리한다.
    if (SQLITE_OK != m_SQLite.Open(":memory:"))
    {
        //ASSERT(0 && m_SQLite.GetLastError());
        return false;
    }

    // Table 생성
    const char* CreateTable = "\
CREATE TABLE ClubMember( \
 ChaDbNum INTEGER NOT NULL, \
 ChaLevel INTEGER NOT NULL, \
 ChaChannel INTEGER NOT NULL, \
 ChaBaseMap INTEGER NOT NULL, \
 ChaGaeaMap INTEGER NOT NULL, \
 ChaFlags INTEGER NOT NULL, \
 ChaName TEXT NOT NULL, \
 ChaOnline INTEGER NOT NULL, \
 LastPlayTime INTEGER NOT NULL, \
 ChaClass INTEGER NOT NULL, \
 PRIMARY KEY(ChaDbNum)); \
CREATE UNIQUE INDEX Idx_ItemId ON ClubMember (ChaDbNum); \
CREATE INDEX Idx_ChaName ON ClubMember (ChaName); \
CREATE INDEX Idx_ChaLevel ON ClubMember (ChaLevel); \
CREATE INDEX Idx_ChaOnline ON ClubMember (ChaOnline);";

    if (SQLITE_OK != m_SQLite.ExecuteSQL(CreateTable))
    {
        sc::ErrorMessageBox(
            sc::string::format(
                "GLClubClient::MakeDb %1%", CreateTable), false);
        return false;
    }
    else
    {
        return true;
    }
}
/*
bool GLClubClient::SetMemberFlag(DWORD dwCharID, DWORD dwFlags)
{
    if (!GLClub::SetMemberFlag(dwCharID, dwFlags))
        return false;

    std::string Query(
        sc::string::format(
            "UPDATE ClubMember SET ChaFlags=%1% WHERE ChaDbNum=%2%;",
            dwFlags, dwCharID));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
    {
        sc::writeLogError(Query);
        return false;
    }
    else
    {
        return true;
    }
}
*/
void GLClubClient::SetMember(const GLCLUBMEMBER& Member)
{
    std::string ChaName(Member.m_ChaName);
    sc::string::replaceForDb(ChaName);
    GLCLUBMEMBER* pMember = GetMember(Member.m_ChaDbNum);
    if (pMember)
    {        
        std::string Query(
            sc::string::format(
                "UPDATE ClubMember SET ChaLevel=%1%, ChaChannel=%2%, ChaBaseMap=%3%, ChaGaeaMap=%4%, ChaFlags=%5%, ChaName='%6%', ChaOnline=%7%, LastPlayTime=%8%, ChaClass=%9% WHERE ChaDbNum=%10%;",
                Member.m_ChaLevel,
                Member.m_Channel,
                Member.m_CurrentMap.getBaseMapID().Id(),
				Member.m_CurrentMap.getGaeaMapID().Id(),
                Member.m_Grade,
                ChaName,
                static_cast<int> (Member.m_Online),
                Member.m_LastPlayTime,
                Member.m_ChaClass,
                Member.m_ChaDbNum));
        if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
            sc::writeLogError(Query);
    }
    else
    {
        std::string Query(
            sc::string::format(
                "INSERT INTO ClubMember (ChaDbNum, ChaLevel, ChaChannel, ChaBaseMap, ChaGaeaMap, ChaFlags, ChaName, ChaOnline, LastPlayTime, ChaClass) VALUES (%1%, %2%, %3%, %4%, %5%, %6%, '%7%', %8%, %9%, %10%);",
                Member.m_ChaDbNum,
                Member.m_ChaLevel,
                Member.m_Channel,
				Member.m_CurrentMap.getBaseMapID().Id(),
                Member.m_CurrentMap.getGaeaMapID().Id(),
                Member.m_Grade,
                ChaName,
                static_cast<int> (Member.m_Online),
                Member.m_LastPlayTime,
                Member.m_ChaClass));
        if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
            sc::writeLogError(Query);
    }

    GLClub::SetMember(Member);
}

void GLClubClient::SetMemberName(DWORD ChaDbNum, std::string& chaName)
{
	GLClub::SetMemberName(ChaDbNum, chaName);

	std::string Query(
		sc::string::format(
			"UPDATE ClubMember SET ChaName='%1%' WHERE ChaDbNum=%2%;",
			chaName,
			ChaDbNum ) );
	if( SQLITE_OK != m_SQLite.ExecuteSQL(Query) )
		sc::writeLogError( Query );
}

void GLClubClient::SetMemberState(DWORD dwCharID, bool bONLINE, int nCHANNEL, const MapID& mapID)
{
    GLClub::SetMemberState(dwCharID, bONLINE, nCHANNEL, mapID);
    
    std::string Query(
        sc::string::format(
            "UPDATE ClubMember SET ChaChannel=%1%, ChaBaseMap=%2%, ChaGaeaMap=%3%, ChaOnline=%4% WHERE ChaDbNum=%5%;",
            nCHANNEL,
            mapID.getBaseMapID().Id(),
			mapID.getGaeaMapID().Id(),
            static_cast<int> (bONLINE),
            dwCharID));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
        sc::writeLogError(Query);
}

void GLClubClient::SetMemberState(DWORD ChaDbNum, const MapID& mapID)
{
    GLClub::SetMemberState(ChaDbNum, mapID);
    
    std::string Query(
        sc::string::format(
            "UPDATE ClubMember SET ChaBaseMap=%1%, ChaGaeaMap=%2% WHERE ChaDbNum=%3%;",
            mapID.getBaseMapID().Id(),
			mapID.getGaeaMapID().Id(),
            ChaDbNum));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
        sc::writeLogError(Query);
}

void GLClubClient::SetMemberState(DWORD ChaDbNum, WORD ChaLevel)
{
    GLClub::SetMemberState(ChaDbNum, ChaLevel);
    
    std::string Query(
        sc::string::format(
            "UPDATE ClubMember SET ChaLevel=%1% WHERE ChaDbNum=%2%;",
            ChaLevel,
            ChaDbNum));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
        sc::writeLogError(Query);
}

bool GLClubClient::SetMemberOffline(DWORD ChaDbNum)
{
    if (!GLClub::SetMemberOffline(ChaDbNum))
        return false;

    std::string Query(
        sc::string::format(
        "UPDATE ClubMember SET ChaOnline=0 WHERE ChaDbNum=%1%;",
        ChaDbNum));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
        sc::writeLogError(Query);
    return true;
}

void GLClubClient::SetMemberPos(DWORD dwCharID, const D3DXVECTOR3& vecPOS)
{
    GLClub::SetMemberPos(dwCharID, vecPOS);
    
    // nothing to do
}

void GLClubClient::RESET()
{
    GLClub::RESET();
	m_vecGuidanceMapName.clear();
    m_lnStorageMoney = 0;
    
    m_bVALID_STORAGE = false;

    RESET_STORAGE();
    
    std::string Query("DELETE FROM ClubMember;");
    m_SQLite.ExecuteSQL(Query);
}

bool GLClubClient::AddMember(
    DWORD CharDbNum,
    const char* szCharName,
    DWORD dwFlags,
    WORD ChaLevel,
    const __time64_t& LastPlayTime,
    bool bOnline,
    EMCHARCLASS ChaClass)
{
    if (!GLClub::AddMember(CharDbNum, szCharName, dwFlags, ChaLevel, LastPlayTime, bOnline, ChaClass))
        return false;
    
    std::string ChaName(szCharName);
    sc::string::replaceForDb(ChaName);

    int Online = bOnline ? 1 : 0;

    std::string Query(
        sc::string::format(
            "INSERT INTO ClubMember (ChaDbNum, ChaLevel, ChaChannel, ChaBaseMap, ChaGaeaMap, ChaFlags, ChaName, ChaOnline, LastPlayTime, ChaClass) VALUES (%1%, %2%, %3%, %4%, %5%, %6%, '%7%', %8%, %9%, %10%);",
            CharDbNum,
            ChaLevel,
            0,
            0,
			0,
            dwFlags,
            ChaName,
            Online,
            LastPlayTime,
            ChaClass));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
        sc::writeLogError(Query);

    return true;
}

bool GLClubClient::AddMemberEx(DWORD ChaDbNum, const GLCLUBMEMBER& Member)
{
    bool bReturn = AddMember(
        ChaDbNum,
        Member.m_ChaName.c_str(),
        Member.m_Grade,
        Member.m_ChaLevel,
        Member.m_LastPlayTime,
        Member.m_Online,
        static_cast<EMCHARCLASS> (Member.m_ChaClass));
    if (bReturn)
    {
        SetMemberState(ChaDbNum, Member.m_Online, Member.m_Channel, Member.m_CurrentMap);
        SetMemberPos(ChaDbNum, Member.m_CurrentPos);
    }

    return bReturn;
}

bool GLClubClient::DelMember(DWORD dwCharID)
{
    if (!GLClub::DelMember(dwCharID))
        return false;
    
    std::string Query(
        sc::string::format(
            "DELETE FROM ClubMember WHERE ChaDbNum=%1%;",
            dwCharID));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
        sc::writeLogError(Query);

    return true;
}

void GLClubClient::GetList(
    std::vector<club::GLCLUBMEMBER_CLIENT>& vResult,
    const std::vector<club::ALIGN_ORDER>& Align,
    bool bOnline)
{
    std::string Query("SELECT ChaDbNum, ChaLevel, ChaChannel, ChaBaseMap, ChaGaeaMap, ChaFlags, ChaName, ChaOnline, LastPlayTime, ChaClass FROM ClubMember ");

    // WHERE ------------------------------------------------------------------
    MakeWhereQuery(Query, bOnline);

    // ORDER BY ---------------------------------------------------------------
    MakeOrderQuery(Align, Query);

    // Query ------------------------------------------------------------------
    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int loop=0; loop<nRowCount; ++loop)
        {
            D3DXVECTOR3	vecPOS;                // 위치번호
            
            club::GLCLUBMEMBER_CLIENT Member;
            Member.m_ChaDbNum        = rTable.GetValue<DWORD> (0);
            Member.m_ChaLevel        = rTable.GetValue<WORD>  (1);
            Member.m_Channel         = rTable.GetValue<int>   (2);
			const DWORD _baseMapID(rTable.GetValue<DWORD> (3));
			const DWORD _gaeaMapID(rTable.GetValue<DWORD> (4));

            Member.m_CurrentMap = MapID(_baseMapID, _gaeaMapID);
            Member.m_Grade           = rTable.GetValue<DWORD> (5);
            std::string MemberName   = rTable.GetValue<std::string> (6);
            Member.SetName(MemberName);
            Member.m_Online         = rTable.GetValue<bool> (7);
            Member.m_LastPlayTime   = rTable.GetValue<__time64_t> (8);
            Member.m_ChaClass       = rTable.GetValue<DWORD> (9);

            const GLCLUBMEMBER* pMember = GetMember(Member.m_ChaDbNum);
            if (pMember)
                Member.m_CurrentPos = pMember->m_CurrentPos;
            
            Member.SetGradeName(m_pGrade->GetName(Member.m_Grade));
            vResult.push_back(Member);
            rTable.GoNext();
        }
    }
}

void GLClubClient::MakeWhereQuery(std::string& Query, bool bOnline)
{
    if (bOnline)
        Query.append(" WHERE ChaOnline=%1 ");
}

void GLClubClient::MakeOrderQuery(const std::vector<club::ALIGN_ORDER>& OrderList, std::string& Query) 
{
    if (OrderList.size() > 0)
    {
        Query.append(" ORDER BY ");
        for (size_t i=0; i<OrderList.size(); ++i)
        {
            bool bType = false;
            const club::ALIGN_ORDER& Order = OrderList[i];
            switch (Order.Type())
            {
            case club::ORDER_CHA_LEVEL:
                Query.append("ChaLevel ");
                bType = true;
                break;
            case club::ORDER_CHA_NAME:
                Query.append("ChaName ");
                bType = true;
                break;
            case club::ORDER_MAP:
                Query.append("ChaBaseMap ");
                bType = true;
                break;
            case club::ORDER_FLAG:
                Query.append("ChaFlags ");
                bType = true;
                break;
            default:
                break;
            }

            if (bType)
            {
                if (Order.Degree() == club::SQL_ASC)
                    Query.append("ASC");
                else
                    Query.append("DESC");

                if (i != (OrderList.size()-1))
                    Query.append(", ");
            }
        }
    }
}

void GLClubClient::RESET_STORAGE()
{
    m_bVALID_STORAGE = false;
    m_lnIncomeMoney = 0;
    m_lnStorageMoney = 0;
    for (int i=0; i<MAX_CLUBSTORAGE; ++i)
        m_cStorage[i].DeleteItemAll();
}

GLInventory* GLClubClient::GetStorage(DWORD Channel)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return NULL;
    else
        return &m_cStorage[Channel];
}

SINVENITEM* GLClubClient::FindPosItem(DWORD Channel, WORD PosX, WORD PosY)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return NULL;
    else
        return m_cStorage[Channel].FindPosItem(PosX, PosY);
}

BOOL GLClubClient::InsertItem(
    const SITEMCUSTOM& ItemCustom,
    DWORD Channel,
    const WORD wPosX,
    const WORD wPosY,
    bool bLOAD,
    bool bLoadCheck)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;
    else
	{	
		bool bSuccess =  m_cStorage[Channel].InsertItem(ItemCustom, wPosX, wPosY, bLOAD, bLoadCheck);
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_Club_ItemChange, "-dw, -n, -n", Channel, wPosX, wPosY);
         
		 return bSuccess;
	}
}

BOOL GLClubClient::InsertItem(
    const SITEMCUSTOM& ItemCustom,
    DWORD Channel,
    const WORD wPosX,
    const WORD wPosY,
    const WORD wBackX,
    const WORD wBackY)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;
    else  
	{
		 bool bSuccess = m_cStorage[Channel].InsertItem(ItemCustom, wPosX, wPosY, wBackX, wBackY);
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ItemChange, "-dw, -dw, -dw", Channel, wPosX, wPosY);
		
		 return bSuccess;
	}	
}

BOOL GLClubClient::InsertItem(
    const SITEMCUSTOM& ItemCustom,
    DWORD Channel,
    bool Force)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;
    else
	{
		bool bSuccess = m_cStorage[Channel].InsertItem(ItemCustom, Force);         
		return bSuccess;
	}
}

BOOL GLClubClient::DeleteItem(DWORD Channel, WORD PosX, WORD PosY)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;
    else
	{
		bool bSuccess = m_cStorage[Channel].DeleteItem(PosX, PosY);
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ItemChange, "-dw, -dw, -dw", Channel, PosX, PosY);
        return bSuccess;
	}
}

void GLClubClient::DeleteItemAll()
{
    for (int i=0; i<MAX_CLUBSTORAGE; ++i)
        m_cStorage[i].DeleteItemAll();
}

void GLClubClient::DeleteItemAll(DWORD Channel)
{
    if (Channel < MAX_CLUBSTORAGE)
        m_cStorage[Channel].DeleteItemAll();
}

SINVENITEM* GLClubClient::GetItem(DWORD Channel, WORD PosX, WORD PosY)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return NULL;
    else
        return m_cStorage[Channel].GetItem(PosX, PosY);
}

BOOL GLClubClient::IsInsertable(
    DWORD Channel,
    const WORD wPosX,
    const WORD wPosY,
    bool bLOAD)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;
    else
        return m_cStorage[Channel].IsInsertable(wPosX, wPosY, bLOAD);
}
/*
BOOL GLClubClient::IsInsertable(
    DWORD Channel,
    const SITEMCUSTOM* pItemCustomArray,
    const WORD wCount,
    bool bLOAD)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;
    else
        return m_cStorage[Channel].IsInsertable(pItemCustomArray, wCount, bLOAD);
}
*/
void GLClubClient::GetMemberOnOffline(club::MEMBER_ON_OFFLINE& Result) const
{
    WORD Online = 0;    
    for (CLUBMEMBERS_CITER iter=m_mapMembers.begin(); iter!=m_mapMembers.end(); ++iter)
    {
        const GLCLUBMEMBER& Member = iter->second;
        if (Member.m_Online)
            ++Online;
    }
    Result.m_Online = Online;
    Result.m_Total = static_cast<WORD> (m_mapMembers.size());
}

bool GLClubClient::ChangeMemberGrade(DWORD MemberDbNum, size_t Grade)
{
    if (!GLClub::ChangeMemberGrade(MemberDbNum, Grade))
        return false;    
    
    std::string Query(
        sc::string::format(
            "UPDATE ClubMember SET ChaFlags=%1% WHERE ChaDbNum=%2%;",
            Grade, MemberDbNum));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
    {
        sc::writeLogError(Query);
        return false;
    }
    else
    {
        return true;
    }
}

void GLClubClient::SetGuidanceMapName( std::vector<std::string>& VecGuidanceMapName )
{
    m_vecGuidanceMapName = VecGuidanceMapName;
}

void GLClubClient::GetGuidanceMapName( std::vector<std::string>& vMap )
{
    vMap = m_vecGuidanceMapName;
}

LuaTable GLClubClient::GetClubMemberList()
{
	LuaTable tbClubMemberList( GLWidgetScript::GetInstance().GetLuaState() );

	if( false == IsValid() )
		return tbClubMemberList;

	int nMemberCount = 1;

	CLUBMEMBERS_ITER iter = m_mapMembers.begin ();
	for( ; iter != m_mapMembers.end(); ++iter )
	{
		GLCLUBMEMBER& sClubMember = (*iter).second;

		LuaTable tbClubMember( GLWidgetScript::GetInstance().GetLuaState() );
		tbClubMember.set( 1, sClubMember.m_ChaName.c_str() );
		tbClubMember.set( 2, sClubMember.m_Online );
		tbClubMember.set( 3, sClubMember.GetChaDbNum() );

		tbClubMemberList.set( nMemberCount++, tbClubMember );
	}

	return tbClubMemberList;
}

LuaTable GLClubClient::GetClubMemberOnOff()
{
	LuaTable tbClubMemberOnOff( GLWidgetScript::GetInstance().GetLuaState() );

	WORD Online = 0;    
	for (CLUBMEMBERS_CITER iter=m_mapMembers.begin(); iter!=m_mapMembers.end(); ++iter)
	{
		const GLCLUBMEMBER& Member = iter->second;
		if (Member.m_Online)
			++Online;
	}
	tbClubMemberOnOff.set(1, Online);
	tbClubMemberOnOff.set(2, static_cast<WORD> (m_mapMembers.size()));

	return tbClubMemberOnOff;
}

DWORD GLClubClient::GetMemberDBNum(const char* _Name)
{
	CLUBMEMBERS_ITER _begin = m_mapMembers.begin();
	CLUBMEMBERS_ITER _end = m_mapMembers.end();	

	for(;_begin != _end; ++_begin)
	{
		GLCLUBMEMBER _Member = _begin->second;
		if(!strcmp(_Member.GetName(), _Name))
			return _Member.GetChaDbNum();
	}

	return 0;
}


LuaTable GLClubClient::GetAutoKickData()
{
	LuaTable tbAutoKick( GLWidgetScript::GetInstance().GetLuaState() );
	tbAutoKick.set(1, m_AutoKickOut.m_Use);
	tbAutoKick.set(2, m_AutoKickOut.m_Day);
	tbAutoKick.set(3, m_AutoKickOut.m_GradeIndex);

	return tbAutoKick;
}

LuaTable GLClubClient::GetGudianceMapInfo()
{
	LuaTable tbRet( GLWidgetScript::GetInstance().GetLuaState() );
	for( size_t i = 0; i < m_vecGuidanceMapInfo.size(); ++i )
	{
		const SGuidanceMapInfo& sInfo = m_vecGuidanceMapInfo[ i ];

		LuaTable tbInfo( GLWidgetScript::GetInstance().GetLuaState() );

		// 선도전 이름
		tbInfo.set( 1, sInfo.strName.c_str() );
		// 수수료율
		tbInfo.set( 2, sInfo.fCommision );

		// 수수료 맵 이름
		LuaTable tbInfoMap( GLWidgetScript::GetInstance().GetLuaState() );
		for( size_t j = 0; j < sInfo.vecMapName.size(); ++j )
			tbInfoMap.set( j + 1, sInfo.vecMapName[ j ].c_str() );
		tbInfo.set( 3, tbInfoMap );

		tbRet.set( i + 1, tbInfo );
	}

	return tbRet;
}

void GLClubClient::MsgGuidanceMapInfo( NET_MSG_GENERIC* pMsg )
{
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	if( m_DbNum != pClient->GetMyClub()->DbNum() )
		return;

	NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
	NET_GUIDANCE_MAP_INFO_AC recvData;
	if( false == msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), recvData ) )
		return;

	m_vecGuidanceMapInfo.clear();

	for( size_t i = 0; i < recvData.m_GuidMap.size(); ++i )
	{
		SGuidanceMapInfo sInfo;
		sInfo.strName = recvData.m_GuidMap[ i ].strName;
		sInfo.fCommision = recvData.m_GuidMap[ i ].fCommision;

		for( size_t j = 0; j < recvData.m_GuidMap[ i ].vecMap.size(); ++j )
		{
			sInfo.vecMapName.push_back( 
				pClient->GetMapNameStr( SNATIVEID( recvData.m_GuidMap[ i ].vecMap[ j ] ) ) );
		}
		m_vecGuidanceMapInfo.push_back( sInfo );
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Club_UpdateGuidanceMap );
}