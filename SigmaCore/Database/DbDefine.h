#ifndef _SG_DBBASE_H_
#define _SG_DBBASE_H_

#pragma once

// http://msdn.microsoft.com/en-us/library/ms179882(v=sql.90).aspx
// money -922,337,203,685,477.5808 to 922,337,203,685,477.5807 8 bytes
// smallmoney - 214,748.3648 to 214,748.3647 4 bytes
#define SQL_MONEY_MAX  922337203685476 //  922337203685477
#define SQL_MONEY_MIN -922337203685476 // -922337203685477

#define SQL_SMALL_MONEY_MAX  214748
#define SQL_SMALL_MONEY_MIN -214748

#define SQL_TINY_INT_MAX 255
#define SQL_TINY_INT_MIN 0

#define SQL_SMALL_INT_MAX  32767
#define SQL_SMALL_INT_MIN -32768

#define SQL_INT_MAX  2147483647 
#define SQL_INT_MIN -2147483648 

#define SQL_BIG_INT_MAX  9223372036854775807
#define SQL_BIG_INT_MIN -9223372036854775808 

namespace sc
{
    namespace db
    {
        // Seconds of database response time
        enum {
			DB_ERROR_OUTOFMEMORY = -1977, 
            DB_RESPONSE_TIME    = 10,
            DB_CONNECTION_COUNT = 10,
            DB_ERROR            = -1,
            DB_OK               = 0,
            DB_ROWCNT_ERROR     = 1,
            DB_USE              = 1,
            DB_NOT_USE          = 0,
            DB_CHA_MAX          = -2,
            DB_CHA_DUF          = -3,
			DB_CHA_GM			= -4,
            DB_INSERT_ITEM      = -2,
            DB_NOT_ITEM         = -3,
            EXTREME_OK          = 1,
            DB_PACKET_SIZE      = 8192,
            DB_IMAGE_BUF_SIZE   = 1024,
            DB_IMAGE_MIN_SIZE   = 12,
            DB_POOL_SIZE        = 5,
            ODBC_ERROR_MESSAGE_LENGTH = 250,
            DB_SVR_NAME_LENGTH  = 50,
            DB_NAME_LENGTH = 50,
			DB_OK_CHAR_ERROR	= -5,
			DB_OK_CHAR_WORLDBATTLE_ERROR = -6,
			DB_OK_CHAR_WORLDBATTLE_VALIDATION_ERROR = -7,
			DB_OK_CHAR_WORLDBATTLE_NAME_ERROR = -8,
        };

        LONGLONG AdjustMoney(LONGLONG Money);
        LONGLONG AdjustSmallMoney(LONGLONG Money);
        
        template <class T>
        int AdjustTinyInt(T value)
        {
            if (value > SQL_TINY_INT_MAX)
                return SQL_TINY_INT_MAX;
            if (value < SQL_TINY_INT_MIN)
                return SQL_TINY_INT_MIN;
            return value;
        }

        template <class T>
        int AdjustSmallInt(T value)
        {
            if (value > SQL_SMALL_INT_MAX)
                return SQL_SMALL_INT_MAX;
            if (value < SQL_SMALL_INT_MIN)
                return SQL_SMALL_INT_MIN;
            return static_cast<int> (value);
        }

        template <class T>
        int AdjustInt(T value)
        {
            if (value > SQL_INT_MAX)
                return SQL_INT_MAX;
            if (value < SQL_INT_MIN)
                return SQL_INT_MIN;
            return value;
        }

        template <class T>
        LONGLONG AdjustBigint(T value)
        {
            if (value > SQL_BIG_INT_MAX)
                return SQL_BIG_INT_MAX;
            if (value < SQL_BIG_INT_MIN)
                return SQL_BIG_INT_MIN;
            return value;
        }

    }
}

#endif // _SG_DBBASE_H_