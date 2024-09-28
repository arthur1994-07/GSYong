#pragma once

/*
 * 자료 구조 array;
 * STL에서 제공하지 않음;
 */

#include <boost/pool/object_pool.hpp>

namespace QTL
{
	template<typename T>
	class arrayListNode
	{
	public:
		enum
		{
			LEFT, RIGHT, NSIZE,
		};
	public:
		arrayListNode(const unsigned int _key, const T& _dataTemplate)
			: first(_key)
			, second(_dataTemplate)
		{
		}
		~arrayListNode(void)
		{
		}

	public:
		const unsigned int first;
		T second;

		union
		{
			struct
			{
				arrayListNode* pNodeNeighbor[arrayListNode::NSIZE];
			};
			struct
			{ // autoexp.dat에서 사용하기 위함;
				arrayListNode* pNodeNeighborPrevious;
				arrayListNode* pNodeNeighborNext;
			};
		};
	};

	template<typename T>
	class ArrayListNodePool
	{
	public:
		inline arrayListNode<T>* const construct(const unsigned int _key, const T& _dataTemplate)
		{
			arrayListNode<T>* const _pNode(this->arrayListNodePool.construct(_key, _dataTemplate));
			if ( _pNode == 0 )
			{
				this->arrayListNodePool.set_next_size(32);
				arrayListNode<T>* const _pNodeRealloc(this->arrayListNodePool.construct(_key, _dataTemplate));
				return _pNodeRealloc;
			}

			return _pNode;
		}
		inline void destroy(arrayListNode<T>* const _pNode)
		{
			this->arrayListNodePool.destroy(_pNode);
		}

	public:
		static ArrayListNodePool& getInstance(void)
		{
			static ArrayListNodePool _instance;
			return _instance;
		}
		~ArrayListNodePool(void)
		{
		}

	private:
		inline const ArrayListNodePool& operator =(const ArrayListNodePool& _rhs){}
		ArrayListNodePool(void)
		{
		}

	private:
		boost::object_pool<arrayListNode<T>> arrayListNodePool;
	};

	template<typename T>
	class arrayList
	{
		enum
		{
			DEFAULT_BIT_SIZE = 4,
		};
	public:
		class iterator;
		class const_iterator;

		inline const int insert(const T& _dataTemplate, const unsigned int _indexInsert);
		inline const int insert(const const_iterator& _iterator);
		inline const int erase(const unsigned int _indexNode);
		inline const int erase(const const_iterator& _iterator);
		inline iterator find(const unsigned int _indexNode) const
		{
			return _indexNode < this->sizeReserve ? arrayList::iterator(this->pNode[_indexNode]) : arrayList::iterator();
		}

		inline iterator begin(void) const { return arrayList::iterator(this->pNodeHead); }
		inline const const_iterator end(void) const { return arrayList::const_iterator(); }

		inline const unsigned int size(void) const;

		inline void clear(void);

	private:
		void _resize(const unsigned int _index);

	public:
		inline const arrayList& operator =(const arrayList& _rhs);
		inline const T& operator [](const unsigned int _indexNode) const { return this->pNode[_indexNode]->second; }
		inline T& operator [](const unsigned int _indexNode) { return this->pNode[_indexNode]->second; }
		arrayList(void);
		arrayList(const unsigned int _sizeReserve);
		arrayList(const arrayList& _rhs);
		~arrayList(void);

	private:
		arrayListNode<T>** pNode;
		arrayListNode<T>* pNodeHead;
		unsigned int sizeNode;
		unsigned int sizeReserve;

		

		class iterator
		{
		public:
			__forceinline void operator ++(void) { pNode = pNode->pNodeNeighbor[arrayListNode<T>::RIGHT]; }
			__forceinline void operator ++(int) { pNode = pNode->pNodeNeighbor[arrayListNode<T>::RIGHT]; }
			__forceinline void operator --(void) { pNode = pNode->pNodeNeighbor[arrayListNode<T>::LEFT]; }
			__forceinline void operator --(int) { pNode = pNode->pNodeNeighbor[arrayListNode<T>::LEFT]; }
			__forceinline const bool operator !=(const iterator& _iterator) const { return pNode != _iterator.pNode; }
			__forceinline const bool operator ==(const iterator& _iterator) const { return pNode == _iterator.pNode; }
			__forceinline const bool operator !=(const const_iterator& _iterator) const { return pNode != _iterator.pNode; }
			__forceinline const bool operator ==(const const_iterator& _iterator) const { return pNode == _iterator.pNode; }
			__forceinline arrayListNode<T>* const operator ->(void) const { return pNode; }
			__forceinline T& operator *(void) const { return pNode->second; }

		public:
			iterator(const iterator& _rhs)
				: pNode(_rhs.pNode)
			{
			}

		private:
			friend class QTL::arrayList;
			__forceinline const iterator& operator=(const iterator& _rhs) { pNode = _rhs.pNode; return *this; }
			__forceinline const iterator& operator=(const const_iterator& _rhs) { pNode = _rhs.pNode; return *this; }
			__forceinline const iterator& operator=(const arrayListNode<T>* const _pRhs) { pNode = _pRhs; return *this; }

			iterator(arrayListNode<T>* _pNode)
				: pNode(_pNode)
			{
			}
			iterator(const arrayListNode<T>* const _pNode)
				: pNode(const_cast<arrayListNode<T>* const>(_pNode))
			{
			}
			iterator(void)
				: pNode(0)
			{
			}

		private:
			arrayListNode<T>* pNode;
		};

		class const_iterator
		{
		public:			
			__forceinline void operator ++(void) { pNode = pNode->pNodeNeighbor[arrayListNode<T>::RIGHT]; }
			__forceinline void operator ++(int) { pNode = pNode->pNodeNeighbor[arrayListNode<T>::RIGHT]; }
			__forceinline void operator --(void) { pNode = pNode->pNodeNeighbor[arrayListNode<T>::LEFT]; }
			__forceinline void operator --(int) { pNode = pNode->pNodeNeighbor[arrayListNode<T>::LEFT]; }
			__forceinline const bool operator !=(const iterator& _iterator) const { return pNode != _iterator.pNode; }
			__forceinline const bool operator ==(const iterator& _iterator) const { return pNode == _iterator.pNode; }
			__forceinline const bool operator !=(const const_iterator& _iterator) const { return pNode != _iterator.pNode; }
			__forceinline const bool operator ==(const const_iterator& _iterator) const { return pNode == _iterator.pNode; }
			__forceinline const arrayListNode<T>* const operator ->(void) const { return pNode; }
			__forceinline const T& operator *(void) const { return pNode->second; }

		public:
			const_iterator(const const_iterator& _iterator)
				: pNode(_iterator.pNode)
			{
			}

			const_iterator(const iterator& _iterator)
				: pNode(_iterator.pNode)
			{
			}

		private:
			friend class QTL::arrayList;
			__forceinline const const_iterator& operator =(const const_iterator& _rhs) { pNode = _rhs.pNode; return *this; }
			__forceinline const const_iterator& operator =(const iterator& _rhs) { pNode = _rhs.pNode; return *this; }
			__forceinline const const_iterator& operator =(const arrayListNode<T>* const _pNode) { pNode = _pNode; return *this; }

			const_iterator(arrayListNode<T>* _pNode)
				: pNode(_pNode)
			{
			}

			const_iterator(const arrayListNode<T>* const _pNode)
				: pNode(_pNode)
			{
			}
			const_iterator(void)
				: pNode(0)
			{
			}

		private:
			const arrayListNode<T>* pNode;
		};
	};
}

#include "./ArrayList.hpp"
