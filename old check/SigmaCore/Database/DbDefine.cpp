#include "stdafx.h"
#include "./DbDefine.h"


#include "../DebugInclude.h"

namespace sc
{
    namespace db
    {
        LONGLONG AdjustMoney(LONGLONG Money)
        {
            if (Money > SQL_MONEY_MAX)
                return SQL_MONEY_MAX;

            if (Money < SQL_MONEY_MIN)
                return SQL_MONEY_MIN;

            return Money;
        }

        LONGLONG AdjustSmallMoney(LONGLONG Money)
        {
            if (Money > SQL_SMALL_MONEY_MAX)
                return SQL_SMALL_MONEY_MAX;
            
            if (Money < SQL_SMALL_MONEY_MIN)
                return SQL_SMALL_MONEY_MIN;

            return Money;
        }

    } // namespace db
} // namespace sc