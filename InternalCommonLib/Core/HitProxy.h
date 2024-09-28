/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

namespace ic
{
    struct SHitProxy
    {
	    DWORD nOrder;

	    SHitProxy()
            : nOrder( 0 )
        {
        }
	    virtual ~SHitProxy()
        {
        }
    };
}