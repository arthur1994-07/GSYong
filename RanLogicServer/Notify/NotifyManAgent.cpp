#include "pch.h"
#include "NotifyManAgent.h"

#include "../../SigmaCore/DebugInclude.h"

namespace gamelogic
{

    NotifyManAgent::NotifyManAgent()
    {
    }

    NotifyManAgent::~NotifyManAgent()
    {
    }

    NotifyManAgent& NotifyManAgent::operator = (const NotifyManAgent& rhs)
    {
        if (this == &rhs)
            return *this;

        NotifyMan::operator = (rhs);

        return *this;
    }

} // namespace gamelogic