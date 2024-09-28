#pragma once

#include "../../RanLogic/Notify/NotifyMan.h"

namespace gamelogic
{

    class NotifyManAgent : public NotifyMan
    {
    public:
        NotifyManAgent();
        virtual ~NotifyManAgent();

        NotifyManAgent& operator = (const NotifyManAgent& rhs);

    };

} // namespace gamelogic