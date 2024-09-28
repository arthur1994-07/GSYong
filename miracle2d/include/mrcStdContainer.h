

#ifndef _mrcStdContainer_H_
#define _mrcStdContainer_H_


#include "mrcConfig.h"



#include <vector>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <stack>

#include <boost/functional/hash.hpp>

#include <boost/interprocess/containers/flat_set.hpp>


#include <boost/ptr_container/ptr_vector.hpp>

// #include <boost/unordered_map.hpp>
// #include <boost/unordered_set.hpp>
#include <hash_map>
#include <hash_set>


#include "mrcAssert.h"



namespace mrc
{

	template<class T>
	struct container_key_compare : public std::binary_function< T, T , bool>
	{
		enum { bucket_size = 4, min_buckets = 8  };
		bool operator()(const T& t1, const T& t2) const
		{
			return t1.getKey() < t2.getKey();
		}

		size_t operator()(const T& t1) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
			return boost::hash_value(t1.getKey());
		}
	};



}

namespace mrc
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	template<class T, class Alloc = MIRACLE_ALLOCATOR(T) >
	struct vector : public std::vector<T,Alloc>
	{
		typedef std::vector<T,Alloc> ori_container;
		template<class _Iter>
		vector(_Iter first, _Iter last) : ori_container(first,last) {}
		vector(size_t size) : ori_container(size) {}
		vector() {}
	};

	template<class T, class Alloc = MIRACLE_NODE_ALLOCATOR(T) >
	struct list : public std::list<T,Alloc>
	{	 
	};

	template<class T, class Alloc = MIRACLE_ALLOCATOR(T) >
	struct deque : public std::deque<T,Alloc>
	{
	};

	template<class T,class Pred = std::less<T>, class Alloc = MIRACLE_NODE_ALLOCATOR(T) >
	struct set : public std::set<T, Pred ,Alloc>
	{
	};

	template<class T,class Pred = std::less<T>, class Alloc = MIRACLE_NODE_ALLOCATOR(T) >
	struct multiset : public std::multiset<T, Pred ,Alloc>
	{
	};

	//////////////////////////////////////////////////////////////////////////
	template<class T,class T2 ,class Pred = std::less<T>, class Alloc = MIRACLE_NODE_PAIR_ALLOCATOR(const T,T2) >
	struct map : public std::map<T,T2,Pred,Alloc>
	{
	};

	//////////////////////////////////////////////////////////////////////////

	template<class T,class T2,class Pred = std::less<T>, class Alloc = MIRACLE_NODE_PAIR_ALLOCATOR(const T,T2) >
	struct multimap : public std::multimap<T,T2,Pred,Alloc>
	{
	};

	//////////////////////////////////////////////////////////////////////////
// 	template<class T,class T2,class Hash = boost::hash<T>, class Pred = std::equal_to<T>, class Alloc = MIRACLE_NODE_PAIR_ALLOCATOR(const T,T2) >
// 	struct hash_map : public boost::unordered_map<T,T2,Hash,Pred,Alloc>
// 	{
// 	};

	template<class T,class T2, class Pred = stdext::hash_compare< T,std::less<T> >, class Alloc = MIRACLE_NODE_PAIR_ALLOCATOR(const T,T2) >
	struct hash_map : public stdext::hash_map<T,T2,Pred,Alloc>
	{
	};

	//////////////////////////////////////////////////////////////////////////

// 	template<class T,class Hash = boost::hash<T>, class Pred = std::equal_to<T>, class Alloc = MIRACLE_NODE_ALLOCATOR(const T) >
// 	struct hash_set : public boost::unordered_set<T,Hash,Pred,Alloc>
// 	{
// 	};
// 
	template<class T,class Pred = stdext::hash_compare< T,std::less<T> >, class Alloc = MIRACLE_NODE_ALLOCATOR(T) >
	struct hash_set : public stdext::hash_set<T,Pred,Alloc>
	{
	};

	//////////////////////////////////////////////////////////////////////////


	template<class T,class Pred = std::less<T>, class Alloc = MIRACLE_ALLOCATOR(T)>
	class ordered_vector : public boost::container::flat_set<T,Pred,Alloc>
	{
	public:
		typedef boost::container::flat_set<T,Pred,Alloc> ori_container;

		// 정렬 되어야 하기때문에 읽기만 되고 쓸때는 직접 삭제하고 다시 넣어줘야함
		const T& operator[](size_t pos) const
		{
			mrcASSERT(pos < size());
			return *(begin() + pos);
		}

		const T& at(size_t pos) const
		{	// subscript nonmutable sequence with checking
			if (size() <= pos)
				_Xran();
			return *(begin() + pos);
		}

	private:
		static void _Xran()
		{	// report an out_of_range error
			throw std::out_of_range("invalid vector<T> subscript");
		}

	};



	//////////////////////////////////////////////////////////////////////////
	template<class T >
	struct queue : public std::queue< T, deque<T> >
	{
		void clear() { *this = queue(); }
	};

	template<class T >
	struct stack : public std::stack< T, vector<T> >
	{
		void clear() { *this = stack(); }
	};


	template<class T>
	struct ptr_vector : public boost::ptr_vector<T,boost::heap_clone_allocator,MIRACLE_ALLOCATOR(T)>
	{

	};
}






#endif //_mrcStdContainer_H_