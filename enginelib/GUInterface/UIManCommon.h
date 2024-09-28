#pragma once

#include "./UIDataType.h"

class ITranslateUIMsg
{
public:
    virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg ) = 0;
};