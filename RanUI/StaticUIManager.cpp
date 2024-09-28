#include "stdafx.h"

#include "./StaticUIManager.h"

namespace uiman
{
    CUIMan s_innerUIMan;
    CUIMan s_outerUIMan;

    CUIMan& GetInnerUIMan()
    {
        return s_innerUIMan;
    }

    CUIMan& GetOuterUIMan()
    {
        return s_outerUIMan;
    }
};