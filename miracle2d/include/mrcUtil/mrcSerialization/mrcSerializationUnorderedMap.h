
#ifndef _mrcSerializationUnorderedMap_h_
#define _mrcSerializationUnorderedMap_h_
// 
// #include <boost/config.hpp>
// 
// #include <boost/serialization/utility.hpp>
// #include <boost/serialization/hash_collections_save_imp.hpp>
// #include <boost/serialization/hash_collections_load_imp.hpp>
// #include <boost/serialization/split_free.hpp>
// 
// namespace boost {
// namespace serialization {
// 
// template<class Archive, class Type, class Key, class Hash, class
// Compare, class Allocator >
// inline void save(
//    Archive & ar,
//    const mrc::hash_map<Key, Type, Hash, Compare, Allocator> &t,
//    const unsigned int /* file_version */
// ){
//    boost::serialization::stl::save_hash_collection<
//        Archive,
//        mrc::hash_map<Key, Type, Hash, Compare, Allocator>
// >(ar, t);
// }
// 
// template<class Archive, class Type, class Key, class Hash, class
// Compare, class Allocator >
// inline void load(
//    Archive & ar,
//    mrc::hash_map<Key, Type, Hash, Compare, Allocator> &t,
//    const unsigned int /* file_version */
// ){
//    boost::serialization::stl::load_hash_collection<
//        Archive,
//        mrc::hash_map<Key, Type, Hash, Compare, Allocator>,
//        boost::serialization::stl::archive_input_map<
//            Archive, mrc::hash_map<Key, Type, Hash, Compare,
// Allocator> >,
//            
// boost::serialization::stl::no_reserve_imp<mrc::hash_map<
//                Key, Type, Hash, Compare, Allocator
// >
// >
// >(ar, t);
// }
// 
// // split non-intrusive serialization function member into separate
// // non intrusive save/load member functions
// template<class Archive, class Type, class Key, class Hash, class
// Compare, class Allocator >
// inline void serialize(
//    Archive & ar,
//    mrc::hash_map<Key, Type, Hash, Compare, Allocator> &t,
//    const unsigned int file_version
// ){
//    boost::serialization::split_free(ar, t, file_version);
// }
// 
// // unordered_multimap
// template<class Archive, class Type, class Key, class Hash, class
// Compare, class Allocator >
// inline void save(
//    Archive & ar,
//    const boost::unordered_multimap<Key, Type, Hash, Compare,
// Allocator> &t,
//    const unsigned int /* file_version */
// ){
//    boost::serialization::stl::save_hash_collection<
//        Archive,
//        boost::unordered_multimap<Key, Type, Hash, Compare, Allocator>
// >(ar, t);
// }
// 
// template<class Archive, class Type, class Key, class Hash, class
// Compare, class Allocator >
// inline void load(
//    Archive & ar,
//    boost::unordered_multimap<Key, Type, Hash, Compare, Allocator> &t,
//    const unsigned int /* file_version */
// ){
//    boost::serialization::stl::load_hash_collection<
//        Archive,
//        boost::unordered_multimap<Key, Type, Hash, Compare, Allocator>,
//        boost::serialization::stl::archive_input_multimap<
//            Archive, boost::unordered_multimap<Key, Type, Hash,
// Compare, Allocator>
// >,
//        boost::serialization::stl::no_reserve_imp<
//            boost::unordered_multimap<Key, Type, Hash, Compare,
// Allocator>
// >
// >(ar, t);
// }
// 
// // split non-intrusive serialization function member into separate
// // non intrusive save/load member functions
// template<class Archive, class Type, class Key, class Hash, class
// Compare, class Allocator >
// inline void serialize(
//    Archive & ar,
//    boost::unordered_multimap<Key, Type, Hash, Compare, Allocator> &t,
//    const unsigned int file_version
// ){
//    boost::serialization::split_free(ar, t, file_version);
// }
// 
// } // serialization
// } // namespace boost 
// 

#endif