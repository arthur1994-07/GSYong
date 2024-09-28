#include "stdafx.h"
#include "../../String/StringFormat.h"
#include "./AdoConnectionString.h"

// ----------------------------------------------------------------------------
#include "../../DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc {
    namespace db {

CjConnectionStrings* CjConnectionStrings::m_pInstance = NULL;

CjConnectionStrings::CjConnectionStrings()
{
}

CjConnectionStrings::~CjConnectionStrings()
{
}

CjConnectionStrings* CjConnectionStrings::GetInstance()
{
    static sc::DefaultLock instanceLock;

    if (!m_pInstance)
    {
        instanceLock.lockOn();
        if (!m_pInstance)
            m_pInstance = new CjConnectionStrings();
        instanceLock.lockOff();
    }
    return m_pInstance;	
}

void CjConnectionStrings::ReleaseInstance()
{
    static sc::DefaultLock instanceLock;

    instanceLock.lockOn();
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
    instanceLock.lockOff();
}

void CjConnectionStrings::Reset()
{ 
    m_mConnStrings.clear();
}

BOOL CjConnectionStrings::AddConnString(
    int nKey,
    const CString& csIP,
    const CString& csDBName,
    const CString& csUserID,
    const CString& csUserPass)
{
    return AddConnString(
        nKey,
        std::string(csIP.GetString()),
        std::string(csDBName.GetString()),
        std::string(csUserID.GetString()),
        std::string(csUserPass.GetString()));
}

BOOL CjConnectionStrings::AddConnString(
    int nKey,
    const std::string& strIP,
    const std::string& strDBName,
    const std::string& strUserID,
    const std::string& strUserPass)
{
    lockOn();

    std::string DbConnString(
        sc::string::format(
            "Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s",
            strUserPass.c_str(),
            strUserID.c_str(),
            strDBName.c_str(),
            strIP.c_str()));

    if (m_mConnStrings.find(nKey) != m_mConnStrings.end())
    {
        //
        // the Key already exists in the map
        //
        lockOff();
        return FALSE;
    }

    m_mConnStrings.insert(MAP_CONNS_VALUE(nKey, DbConnString));
    lockOff();

    return TRUE;
}

BOOL CjConnectionStrings::GetConnString(int nKey, std::string& csConnString)
{
    lockOn();
    MAP_CONNS_CITER iter = m_mConnStrings.find(nKey);
    if (iter == m_mConnStrings.end())
    {
        // could not find
        lockOff();
        return FALSE;
    }

    csConnString = iter->second;
    lockOff();
    return TRUE;
}

    } // namespace db
} // namespace sc