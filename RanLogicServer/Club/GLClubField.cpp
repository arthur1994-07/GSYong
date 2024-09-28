#include "pch.h"
#include "../../RanLogic/Club/ClubAuthGrade.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./GLClubField.h"

#include "../Database/DBAction/DbActionGameClub.h"
#include "../Util/GLItemLimit.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLClubField::GLClubField(GLGaeaServer* pServer)
    : m_pServer(pServer)
    , m_ProfitMoeny(0)
    , m_fTIMER_MONEY(0)
    //, m_fTIMER_POS(0)
{
}

GLClubField::~GLClubField()
{
}

const GLClubField& GLClubField::operator = (const GLClub& value)
{
    if (this == &value)
        return *this;

	 GLClub::operator = (value);

    // 수익금은 복사하지 않는다.
    m_lnIncomeMoney = 0;
    m_lnStorageMoney = 0;

    return *this;
}

const GLClubField& GLClubField::operator = (const GLClubField& value)
{
    if( this == &value )
        return *this;

    GLClub::operator = (value);

    m_ProfitMoeny = 0; // 수익금은 복사하지 않는다.
    //m_fTIMER_POS = value.m_fTIMER_POS;
    m_fTIMER_MONEY = value.m_fTIMER_MONEY;    
    
    return *this;
}

void GLClubField::RESET()
{
    GLClub::RESET();
    m_ProfitMoeny = 0;
    //m_fTIMER_POS = 0;
    m_fTIMER_MONEY = 0;
}

void GLClubField::SendClubClient(NET_MSG_GENERIC* pMsg)
{
    for (CLUBMEMBERS_CITER iter=m_mapMembers.begin(); iter!=m_mapMembers.end(); ++iter)
    {
        GLChar* pChar = m_pServer->GetCharByDbNum(iter->first);
        if (pChar)
            m_pServer->SENDTOCLIENT(pChar->ClientSlot(), pMsg);
    }
}

/*
void GLClubField::SAVESTORAGE2DB()
{
    db::IDbManager *pDBMan = m_pServer->GetDBMan();
    if (!pDBMan)
        return;

    if (!m_bVALID_STORAGE)
        return;

    // 창고 인벤 저장.
    se::ByteStream cStream;
    BOOL bOK = GETSTORAGE_BYBUF ( cStream );
    if (bOK)
    {
        LPBYTE pBuff(NULL);
        DWORD dwSize(NULL);
        cStream.GetBuffer( pBuff, dwSize );

        pDBMan->AddGameJob(
            db::DbActionPtr(
            new db::ClubStorageWrite(m_DbNum, (BYTE*) pBuff, dwSize)));        
    }

    // Money Db에 저장.
    pDBMan->AddGameJob(
        db::DbActionPtr(
            new db::ClubMoneySet(m_DbNum, m_lnStorageMoney)));
}
*/