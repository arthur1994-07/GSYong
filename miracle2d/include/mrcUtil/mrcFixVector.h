

#ifndef _mrcFixVector_h_
#define _mrcFixVector_h_

#include "mrcConfig.h"

#include "detail/fix_vector.hpp"

namespace mrc
{
	template<class T, std::size_t N>
	class fix_vector  : public ::fix_vector<T,N>
	{

	};



}




#endif