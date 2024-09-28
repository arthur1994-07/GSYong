#include "../../pch.h"
#include <sql.h>
#include "./OdbcStmt.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{
    ODBC_STMT::ODBC_STMT()
        : hStmt(NULL)
        , hOdbc(NULL)
        , m_bUse(false)
    {
    }

    ODBC_STMT::~ODBC_STMT()
    {
        if (hStmt)
        {
            ::SQLCloseCursor(hStmt);
            ::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }

        if (hOdbc)
        {
            ::SQLDisconnect(hOdbc);
            ::SQLFreeHandle(SQL_HANDLE_DBC, hOdbc);
        }
    }

} // namespace db