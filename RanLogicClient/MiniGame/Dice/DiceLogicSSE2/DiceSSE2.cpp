#include "pch.h"
#include "../IDiceDefine.h"
#include "DiceSSE2.h"

namespace MiniGame
{
	void* DiceSSE2::operator new(const size_t _nSize)
	{
		return (DiceSSE2*)::_aligned_malloc(_nSize, 16);
	}
	void* DiceSSE2::operator new[](const size_t _nSize)
	{
		return (DiceSSE2*)::_aligned_malloc(_nSize, 16);
	}
	void DiceSSE2::operator delete(void* _pThis)
	{
		::_aligned_free(_pThis);
	}
	void DiceSSE2::operator delete[](void* _pThis)
	{
		::_aligned_free(_pThis);
	}
	DiceSSE2::DiceSSE2(void) 
		: fMovingSpeed(0.0f)
		, fRollingSpeed(0.0f)
		, nDetermineNumber(1)
		, nState(MiniGame::EMDICE_STATE_IDLE)		
		, xMoveDirect(SSE2::xmm(1.0f, 0.0f, 0.0f, 0.0f))
		, xRollingAxis(SSE2::xmm(1.0f, 0.0f, 0.0f, 0.0f))
		, xPosition(SSE2::xmm(0.0f, 0.0f, 0.0f))
	{
		SSE2::identityMatrix(this->xWorld);

		this->xDetermineWorld = this->xWorld;
	}

	DiceSSE2::~DiceSSE2(void)
	{
	}

	void DiceSSE2::reset(void)
	{
		this->nState = MiniGame::EMDICE_STATE_IDLE;
		this->xMoveDirect = SSE2::xmm(0.0f, 0.0f, 0.0f, 0.0f);
		this->xRollingAxis = SSE2::xmm(1.0f, 0.0f, 0.0f, 0.0f);		
		this->fMovingSpeed = 0.0f;
		this->fRollingSpeed = 0.0f;
	}

	void DiceSSE2::_setRotateAxis(void)
	{
		switch ( this->nState )
		{
		case MiniGame::EMDICE_STATE_INTERPOLATE:
		case MiniGame::EMDICE_STATE_DETERMINE:
			return;
		}

		this->xRollingAxis = SSE2::xmm(0.0f, 0.0f, 1.0f, 0.0f) ^ this->xMoveDirect;
		SSE2::normalize(this->xRollingAxis);
	}	

	void DiceSSE2::holdOut(const unsigned int _nDetNum, const __m128& _xHoldOutPosition)
	{
		if ( this->nState != MiniGame::EMDICE_STATE_HOLD )
			return;

		this->xMoveDirect = _xHoldOutPosition - this->xPosition;
		this->nState = MiniGame::EMDICE_STATE_MOVE;
		this->fMovingSpeed = SSE2::normalize(this->xMoveDirect);
		this->fRollingSpeed = this->fMovingSpeed * 0.1f;
		if ( _nDetNum > 6 || _nDetNum == 0)
			this->nDetermineNumber = 1;
		else
			this->nDetermineNumber = _nDetNum;

		DiceSSE2::_setRotateAxis();
	}

	const bool DiceSSE2::_isGrab(void)
	{
		switch ( this->nState )
		{
		case MiniGame::EMDICE_STATE_MOVE:
		case MiniGame::EMDICE_STATE_DETERMINE:
		case MiniGame::EMDICE_STATE_INTERPOLATE:
			return false;
		}

		return true;
	}
	void DiceSSE2::grab(const __m128& _xGrabPosition, const float _fWidth, const float _fHeight)
	{
		if ( DiceSSE2::_isGrab() == false )
			return;	
			
		this->xGrabPosition = _xGrabPosition;

		const __m128& _xMinimum = -SSE2::xmm(_fWidth, _fHeight, 0.0f);
		const __m128& _xMaximum = SSE2::xmm(_fWidth, _fHeight, DEFDEPTH);
		SSE2::limit(this->xGrabPosition, _xMinimum, _xMaximum);
		this->xMoveDirect = this->xGrabPosition - this->xPosition;	

		this->fMovingSpeed = SSE2::normalize(this->xMoveDirect) * 0.5f;
		this->fRollingSpeed = this->fMovingSpeed * 0.05f;
		this->nState = MiniGame::EMDICE_STATE_HOLD;

		DiceSSE2::_setRotateAxis();
	}

	void DiceSSE2::moveBlock(void)
	{
		this->xPosition += (this->xMoveDirect * this->fMovingSpeed * 0.2f);
	}

	const int DiceSSE2::updateBlock(void)
	{
		switch ( this->nState )
		{
		case MiniGame::EMDICE_STATE_IDLE:
			DiceSSE2::_interpolateMatrix();
			return -1;
		case MiniGame::EMDICE_STATE_MOVE:
			if ( this->fMovingSpeed < DEFLIMIT_SPEED )
				this->nState = MiniGame::EMDICE_STATE_DETERMINE;
			break;
		case MiniGame::EMDICE_STATE_DETERMINE:
			DiceSSE2::_determineMatrix();
			this->nState = MiniGame::EMDICE_STATE_INTERPOLATE;
			break;
		case MiniGame::EMDICE_STATE_INTERPOLATE:
			if ( this->fMovingSpeed < 0.001f )
				this->nState = MiniGame::EMDICE_STATE_FINISH;
			break;
		case MiniGame::EMDICE_STATE_FINISH:
			DiceSSE2::_interpolateMatrix();
			return 0;
		}

		const __m128_4& _xQuaternion = SSE2::quarternionMatrix(this->xRollingAxis, this->fRollingSpeed);

		this->xWorld *= _xQuaternion;
		SSE2::normalize(this->xWorld[0]);
		SSE2::normalize(this->xWorld[1]);
		SSE2::normalize(this->xWorld[2]);
		this->xWorld[3] = this->xPosition;		

		return -1;
	}

	const bool DiceSSE2::isCollision(DiceSSE2& _targetDice)
	{
		__m128 _xLength = _targetDice.xPosition - this->xPosition;
		const float _fLength = SSE2::normalize(_xLength);
		const float _fRate = (3.0f - _fLength) * 0.51f;
		if ( _fRate < 0.0f )
			return false;

		this->xPosition -= (_xLength * _fRate);
		_targetDice.xPosition += (_xLength * _fRate);

		const __m128& _xMoveDirect = this->xMoveDirect - _targetDice.xMoveDirect;
		const __m128& _xDotProduct = SSE2::dotProduct(_xMoveDirect * _xLength) * 2.0f;
		{ // Dice_0;			
			const __m128& _xDot = -_xDotProduct;
			this->xMoveDirect = _xMoveDirect + (_xLength * _xDot);
			this->fMovingSpeed *= 0.95f;
			SSE2::normalize(this->xMoveDirect);
			DiceSSE2::_setRotateAxis();
		}

		{ // Dice_1;
			const __m128& _xDot = _xDotProduct;
			_targetDice.xMoveDirect = (_xLength * _xDot) - _xMoveDirect;
			_targetDice.fMovingSpeed *= 0.95f;
			SSE2::normalize(_targetDice.xMoveDirect);			
			_targetDice._setRotateAxis();
		}		

		return true;
	}


	const bool DiceSSE2::isCollision(const float _fWidth, const float _fHeight)
	{
		const __m128& _xMinimum = -SSE2::xmm(_fWidth, _fHeight, 0.0f);
		const __m128& _xMaximum = SSE2::xmm(_fWidth, _fHeight, DEFDEPTH);
		const __m128& _xCollision = SSE2::limit(this->xPosition, _xMinimum, _xMaximum);
		this->xMoveDirect *= ((_xCollision & -2.0f) + 1.0f);
		DiceSSE2::_setRotateAxis();

		const bool _bCollision = !SSE2::isZero(_xCollision);
		if ( this->nState != MiniGame::EMDICE_STATE_INTERPOLATE && _bCollision == false )
			return false;

		this->fMovingSpeed *= 0.96f;
		this->fRollingSpeed *= 0.97f;

		return _bCollision;
	}

	void DiceSSE2::_determineMatrix(void)
	{
		const float _fAngle = float(((::rand() & 1023) - 511) << 2) * 0.001f * DEFNUMBER_PI;

		this->xWorld = SSE2::rotateAxisZ(DiceConstNumber::xConstNumber[this->nDetermineNumber - 1], _fAngle);
		this->xWorld[3] = this->xPosition;

		this->xDetermineWorld = this->xWorld;

		this->fRollingSpeed = 0.190f;
		this->fMovingSpeed = DEFLIMIT_SPEED;
	}

	void DiceSSE2::_interpolateMatrix(void)
	{
		const __m128_4& _xInterpolateMatrix = (this->xDetermineWorld - this->xWorld) * DEFDICE_INTERPOLATE;
		this->xWorld = this->xDetermineWorld + _xInterpolateMatrix;		
		this->xWorld[3] = this->xPosition;
	}
}

