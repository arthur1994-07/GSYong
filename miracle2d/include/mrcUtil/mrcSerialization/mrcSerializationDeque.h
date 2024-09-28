

#ifndef _mrcSerializationDeque_h_
#define _mrcSerializationDeque_h_

#include <boost/config.hpp>

#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/collections_load_imp.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost { 
	namespace serialization {

		template<class Archive, class U, class Allocator>
		inline void save(
			Archive & ar,
			const mrc::deque<U, Allocator> &t,
			const unsigned int /* file_version */
			){
				boost::serialization::stl::save_collection<
					Archive, mrc::deque<U, Allocator> 
				>(ar, t);
		}

		template<class Archive, class U, class Allocator>
		inline void load(
			Archive & ar,
			mrc::deque<U, Allocator> &t,
			const unsigned int /*file_version*/
			){
				boost::serialization::stl::load_collection<
					Archive,
					mrc::deque<U, Allocator>,
					boost::serialization::stl::archive_input_seq<
					Archive, mrc::deque<U, Allocator> 
					>,
					boost::serialization::stl::no_reserve_imp<mrc::deque<U, Allocator> >
				>(ar, t);
		}

		// split non-intrusive serialization function member into separate
		// non intrusive save/load member functions
		template<class Archive, class U, class Allocator>
		inline void serialize(
			Archive & ar,
			mrc::deque<U, Allocator> &t,
			const unsigned int file_version
			){
				boost::serialization::split_free(ar, t, file_version);
		}

	} // namespace serialization
} // namespace boost

#include <boost/serialization/collection_traits.hpp>

BOOST_SERIALIZATION_COLLECTION_TRAITS(mrc::deque)


#endif
