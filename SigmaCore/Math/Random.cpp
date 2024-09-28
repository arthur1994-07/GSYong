#include "stdafx.h"
#include "./Random.h"

#include <boost/thread.hpp>
#include <cstdlib>

// ----------------------------------------------------------------------------
#include "../DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{

    Random& Random::getInstance()
    {
		random::GenerateSeed();

	    static Random Instance;
	    return Instance;
    }

	unsigned int Random::GetValue( DWORD nMin, DWORD nMax )
	{
		if ( nMax < nMin )
			return 0;
		else if ( nMax == nMin )
			return nMin;

		DWORD dwValue = (*spGenerator)();

		return (dwValue%(nMax+1-nMin))+nMin;
	}

//     WORD Random::get()
//     {
// 		WORD wTemp = Next(1,9999);
// 		return wTemp;
//     }

//     //! 0 ~ 99
//     WORD Random::getPercent()
//     {
// 		WORD wTemp = Get() / 100;
// 	    return wTemp;
//     }

//     float Random::getFloatPercent()
//     {
// 		float fTemp = ((float) Get() / 100.0f);
// 	    return fTemp;
//     }
// 	const float Random::getFloatRate(void)
// 	{
// 		const float _fRate(float(Get()) * 0.0001f);
// 		return _fRate;
// 	}


	boost::mutex mutex;
	void random::GenerateSeed()
	{
		static boost::thread_specific_ptr<bool> tls;

		boost::lock_guard<boost::mutex> lock(mutex);

		if( !tls.get() )
			tls.reset( new bool( false ) );

		if( !*tls )
		{
			*tls = true;
			srand( static_cast<unsigned int>( time( 0 ) ) );
		}
	}





    RandomInt::RandomInt()
    {
        srand((unsigned int) time(NULL));
    }

    RandomInt::~RandomInt()
    {
    }

    __int64 RandomInt::Int64()
    {
		random::GenerateSeed();

        int hi  = rand();
        int mid = rand();
        int lo  = rand();
        return ((__int64) hi << 32) | mid << 16 | lo;
    }





}