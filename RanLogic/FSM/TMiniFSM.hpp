#pragma once

__forceinline const unsigned int unsignedState::getNBit(void) const
{
	enum
	{
		_nField0 = 0x49249249ul, // = 0100_1001_0010_0100_1001_0010_0100_1001
		_nField1 = 0x381c0e07ul, // = 0011_1000_0001_1100_0000_1110_0000_0111 
	};
	unsigned int _nState(this->nState);

	_nState = (_nState & _nField0) + ((_nState >> 1) & _nField0) + ((_nState >> 2) & _nField0);
	_nState = ((_nState + (_nState >> 3)) & _nField1) + ((_nState >> 6) & _nField1);
	return (_nState + (_nState >> 9) + (_nState >> 18) + (_nState >> 27)) & 0x3f;
}

__forceinline const bool unsignedState::isState(const unsigned int _nState) const
{
	return this->nState & _nState ? true : false;
}

__forceinline void unsignedState::setState(const unsigned int _nState, const bool _bOn)
{
	*this = _bOn == true ? unsignedState::operator+=(_nState) : unsignedState::operator-=(_nState);
	//		this->nState = _bOn == true ? (this->nState | _nState) : (this->nState & (~_nState));
}
__forceinline void unsignedState::reset(void)
{
	this->nState = 0;
}

__forceinline const bool unsignedState::operator ==(const unsigned int _nState)
{
	return this->nState == _nState;
}
__forceinline const bool unsignedState::operator ==(const unsignedState& _rhs)
{
	return this->nState == _rhs.nState;
}
__forceinline const unsignedState& unsignedState::operator=(const unsigned int _nState)
{
	this->nState = _nState;
	return *this;
}
__forceinline const unsignedState& unsignedState::operator=(const unsignedState& _rhs)
{
	this->nState = _rhs.nState;
	return *this;
}
__forceinline const unsignedState& unsignedState::operator+=(const unsigned int _nState)
{
	this->nState |= _nState;
	return *this;
}
__forceinline const unsignedState& unsignedState::operator-=(const unsigned int _nState)
{
	this->nState &= (~_nState);
	return *this;
}

__forceinline const bool unsignedState::operator[](const unsigned int _nState) const
{
	return this->nState & _nState ? true : false;
}
__forceinline const bool unsignedState::operator()(void) const
{
	return this->nState ? true : false;
}