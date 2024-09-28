#ifndef SC_ODBC_DEFINE_H_
#define SC_ODBC_DEFINE_H_

#pragma once

struct ODBC_STMT
{	
    SQLHSTMT hStmt;
    SQLHDBC hOdbc;

    ODBC_STMT()
        : hStmt(NULL)
        , hOdbc(NULL)
    {       
    }
};

#endif // SC_ODBC_DEFINE_H_