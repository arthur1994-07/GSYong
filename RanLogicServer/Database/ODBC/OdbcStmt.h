#ifndef _MIN_ODBC_STMT_H_
#define _MIN_ODBC_STMT_H_

#pragma once

#include <sqltypes.h>

namespace db
{
    class ODBC_STMT
    {
    public:
        ODBC_STMT();
        ~ODBC_STMT();

        SQLHSTMT hStmt;
        SQLHDBC hOdbc;
        bool m_bUse;
    };
} // namespace db

#endif // _MIN_ODBC_STMT_H_