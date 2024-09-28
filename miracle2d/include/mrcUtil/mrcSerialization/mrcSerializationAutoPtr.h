

#ifndef _mrcSerializationAutoPtr_h_
#define _mrcSerializationAutoPtr_h_

//////////////////////////////////////////////////////////////////////////
#include <boost/serialization/split_free.hpp>

namespace boost { 
	namespace serialization {

		/////////////////////////////////////////////////////////////
		// implement serialization for auto_ptr<T>
		// note: this must be added to the boost namespace in order to
		// be called by the library
		template<class Archive, class T>
		inline void save(
			Archive & ar,
			const std::auto_ptr<T> &t,
			const unsigned int file_version
			){
				// only the raw pointer has to be saved
				// the ref count is rebuilt automatically on load
				const T * const tx = t.get();
				ar << tx;
		}

		template<class Archive, class T>
		inline void load(
			Archive & ar,
			std::auto_ptr<T> &t,
			const unsigned int file_version
			){
				T *pTarget;
				ar >> pTarget;
				// note that the reset automagically maintains the reference count
#if BOOST_WORKAROUND(BOOST_DINKUMWARE_STDLIB, == 1)
				t.release();
				t = std::auto_ptr<T>(pTarget);
#else
				t.reset(pTarget);
#endif
		}

		// split non-intrusive serialization function member into separate
		// non intrusive save/load member functions
		template<class Archive, class T>
		inline void serialize(
			Archive & ar,
			std::auto_ptr<T> &t,
			const unsigned int file_version
			){
				boost::serialization::split_free(ar, t, file_version);
		}

	} // namespace serialization
} // namespace boost

//////////////////////////////////////////////////////////////////////////







#endif