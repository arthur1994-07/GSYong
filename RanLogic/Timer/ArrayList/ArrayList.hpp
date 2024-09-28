#pragma once

#include <assert.h>

namespace  QTL
{
	__forceinline const unsigned int __fastcall bitScanReverse(const unsigned int _valueInteger)
	{
		unsigned long _indexBit;
		::_BitScanReverse(&_indexBit, _valueInteger - 1);
		return _indexBit + 1;
	}

	template<typename T>
	arrayList<T>::arrayList(void)
		: pNode((arrayListNode<T>**)::_aligned_malloc(sizeof(arrayListNode<T>*) << arrayList<T>::DEFAULT_BIT_SIZE, 16))
		, pNodeHead(0)
		, sizeNode(0)
		, sizeReserve(1 << arrayList<T>::DEFAULT_BIT_SIZE)
	{
		::memset(this->pNode, 0, sizeof(arrayListNode<T>*) << arrayList<T>::DEFAULT_BIT_SIZE);
	}

	template<typename T>
	arrayList<T>::arrayList(const unsigned int _sizeReserve)
		: pNode((arrayListNode<T>**)::_aligned_malloc(sizeof(arrayListNode<T>*) << QTL::bitScanReverse(_sizeReserve), 16))
		, pNodeHead(0)
		, sizeNode(0)
		, sizeReserve(QTL::bitScanReverse(_sizeReserve))
	{
		::memset(this->pNode, 0, sizeof(arrayListNode<T>*) << QTL::bitScanReverse(_sizeReserve));
	}

	template<typename T>
	arrayList<T>::arrayList(const arrayList<T>& _rhs)
		: pNode((arrayListNode<T>**)::_aligned_malloc(sizeof(arrayListNode<T>*) * _rhs.sizeReserve, 16))
		, pNodeHead(0)
		, sizeNode(0)
		, sizeReserve(_rhs.sizeReserve)
	{
		for ( arrayList<T>::const_iterator _iteratorNode(_rhs.head()); _iteratorNode != _rhs.end(); ++_iteratorNode )
			arrayList<T>::insert(_iteratorNode);
	}

	template<typename T>
	arrayList<T>::~arrayList(void)
	{
		arrayList<T>::clear();
		::_aligned_free(this->pNode);
	}

	template<typename T>
	inline const arrayList<T>& arrayList<T>::operator =(const arrayList<T>& _rhs)
	{
		const unsigned int _sizeNode(_rhs.size());

		if ( (this->pNode == 0) || (this->sizeReserve < _sizeNode) )
			arrayList<T>::_resize(_rhs.sizeReserve);


		for ( arrayList<T>::const_iterator _iteratorNode(_rhs.begin()); _iteratorNode != _rhs.end(); ++_iteratorNode )
			arrayList<T>::insert(_iteratorNode);

		return *this;
	}

	template<typename T>
	inline const int arrayList<T>::insert(const const_iterator& _iterator)
	{
		const unsigned int _key(_iterator->first);
		const T& _dataTemplate(_iterator->second);
		return arrayList<T>::insert(_dataTemplate, _key);
	}

	template<typename T>
	inline const int arrayList<T>::insert(const T& _dataTemplate, const unsigned int _indexInsert)
	{
		const unsigned int _sizeReserve(this->sizeReserve);
		if ( _indexInsert >= _sizeReserve)
			arrayList<T>::_resize(_indexInsert);

		arrayListNode<T>* _pNodeNew(ArrayListNodePool<T>::getInstance().construct(_indexInsert, _dataTemplate));

		arrayList<T>::erase(_indexInsert);
		arrayListNode<T>*& _pNodeInsert = this->pNode[_indexInsert];
		_pNodeInsert = _pNodeNew;
		_pNodeInsert->pNodeNeighbor[arrayListNode<T>::RIGHT] = this->pNodeHead;
		_pNodeInsert->pNodeNeighbor[arrayListNode<T>::LEFT] = 0;

		if ( this->pNodeHead != 0 )
			this->pNodeHead->pNodeNeighbor[arrayListNode<T>::LEFT] = _pNodeInsert;

		this->pNodeHead = _pNodeInsert;

		++this->sizeNode;

		return _indexInsert;
	}

	template<typename T>
	inline const int arrayList<T>::erase(const unsigned int _indexErase)
	{
		const unsigned int _sizeReserve(this->sizeReserve);
		if ( _indexErase >= _sizeReserve )
			return -1;

		arrayListNode<T>* _pNodeErase(this->pNode[_indexErase]);
		if ( _pNodeErase == 0 )
			return -1;

		arrayListNode<T>* _pNodeLeft(_pNodeErase->pNodeNeighbor[arrayListNode<T>::LEFT]);
		arrayListNode<T>* _pNodeRight(_pNodeErase->pNodeNeighbor[arrayListNode<T>::RIGHT]);

		if ( _pNodeLeft != 0 )
			_pNodeLeft->pNodeNeighbor[arrayListNode<T>::RIGHT] = _pNodeRight;
		else
			this->pNodeHead = _pNodeRight;

		if ( _pNodeRight != 0 )
			_pNodeRight->pNodeNeighbor[arrayListNode<T>::LEFT] = _pNodeLeft;

		ArrayListNodePool<T>::getInstance().destroy(_pNodeErase);
		this->pNode[_indexErase] = 0;

		--this->sizeNode;

		return 0;
	}

	template<typename T>
	inline const int arrayList<T>::erase(const const_iterator& _iterator)
	{
		return arrayList<T>::erase(_iterator->first);
	}

	template<typename T>
	inline const unsigned int arrayList<T>::size(void) const
	{
		return this->sizeNode;
	}

	template<typename T>
	inline void arrayList<T>::clear(void)
	{
		arrayListNode<T>* _pNode(this->pNodeHead);
		if ( _pNode != 0 )
		{
			do
			{
				this->pNode[_pNode->first] = 0;
				ArrayListNodePool<T>::getInstance().destroy(_pNode);
			}
			while ( (_pNode = _pNode->pNodeNeighbor[arrayListNode<T>::RIGHT]) != 0 );
		}

		this->sizeNode = 0;
	}

	template<typename T>
	void arrayList<T>::_resize(const unsigned int _index)
	{
		assert(_index != -1);

		const unsigned int _bitSizeRealloc(QTL::bitScanReverse(_index) + 1);
		const unsigned int _sizeRealloc(1 << _bitSizeRealloc);

		arrayListNode<T>** const _pNode = (arrayListNode<T>** const)::_aligned_realloc(this->pNode , sizeof(arrayListNode<T>*) << _bitSizeRealloc, 16);

		assert(_pNode != 0 );

		this->pNode = _pNode;

		for ( unsigned int _index(this->sizeReserve); _index < _sizeRealloc; ++_index )
			this->pNode[_index] = 0;

		this->sizeReserve = _sizeRealloc;
	}
}