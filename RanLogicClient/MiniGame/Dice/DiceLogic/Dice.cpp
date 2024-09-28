#include "pch.h"
#include "../IDiceDefine.h"
#include "Dice.h"

namespace MiniGame
{
	const D3DXVECTOR3 _limit(D3DXVECTOR3& _vTarget, const D3DXVECTOR3& _vMin, const D3DXVECTOR3& _vMax);
	Dice::Dice(void) 
		: fMovingSpeed(0.0f)
		, fRollingSpeed(0.0f)
		, nDetermineNumber(1)
		, nState(MiniGame::EMDICE_STATE_IDLE)		
		, vMoveDirect(D3DXVECTOR3(1.0f, 0.0f, 0.0f))
		, vRollingAxis(D3DXVECTOR3(1.0f, 0.0f, 0.0f))
		, vPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
	{
		::D3DXMatrixIdentity(&this->mWorld);

		this->mDetermineWorld = this->mWorld;
	}

	Dice::~Dice(void)
	{
	}

	void Dice::reset(void)
	{
		this->nState = MiniGame::EMDICE_STATE_IDLE;
		this->vMoveDirect = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		this->vRollingAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);		
		this->fMovingSpeed = 0.0f;
		this->fRollingSpeed = 0.0f;
	}

	void Dice::_setRotateAxis(void)
	{
		switch ( this->nState )
		{
		case MiniGame::EMDICE_STATE_INTERPOLATE:
		case MiniGame::EMDICE_STATE_DETERMINE:
			return;
		}

		::D3DXVec3Cross(&this->vRollingAxis, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &this->vMoveDirect);
		::D3DXVec3Normalize(&this->vRollingAxis, &this->vRollingAxis);
	}	

	void Dice::holdOut(const unsigned int _nDetNum, const D3DXVECTOR3& _vHoldOutPosition)
	{
		if ( this->nState != MiniGame::EMDICE_STATE_HOLD )
			return;

		this->vMoveDirect = _vHoldOutPosition - this->vPosition;
		this->nState = MiniGame::EMDICE_STATE_MOVE;
		this->fMovingSpeed = ::D3DXVec3Length(&this->vMoveDirect);
		this->fRollingSpeed = this->fMovingSpeed * 0.1f;
		if ( _nDetNum > 6 || _nDetNum == 0)
			this->nDetermineNumber = 1;
		else
			this->nDetermineNumber = _nDetNum;

		::D3DXVec3Normalize(&this->vMoveDirect, &this->vMoveDirect);
		Dice::_setRotateAxis();
	}


	const bool Dice::_isGrab(void)
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
	void Dice::grab(const D3DXVECTOR3& _vGrabPosition, const float _fWidth, const float _fHeight)
	{
		if ( Dice::_isGrab() == false )
			return;		
		
		this->vGrabPosition = _vGrabPosition;

		const D3DXVECTOR3 _vMinimum(-_fWidth, -_fHeight, 0.0f);
		const D3DXVECTOR3 _vMaximum(_fWidth, _fHeight, DEFDEPTH);		
		MiniGame::_limit(this->vGrabPosition, _vMinimum, _vMaximum);	
		this->vMoveDirect = this->vGrabPosition - this->vPosition;

		this->fMovingSpeed = ::D3DXVec3Length(&this->vMoveDirect) * 0.5f;
		this->fRollingSpeed = this->fMovingSpeed * 0.05f;
		this->nState = MiniGame::EMDICE_STATE_HOLD;

		::D3DXVec3Normalize(&this->vMoveDirect, &this->vMoveDirect);
		Dice::_setRotateAxis();
	}

	void Dice::moveBlock(void)
	{
		this->vPosition += (this->vMoveDirect * this->fMovingSpeed * 0.2f);
	}

	const int Dice::updateBlock(void)
	{
		switch ( this->nState )
		{
		case MiniGame::EMDICE_STATE_IDLE:
			Dice::_interpolateMatrix();
			return -1;
		case MiniGame::EMDICE_STATE_MOVE:
			if ( this->fMovingSpeed < DEFLIMIT_SPEED )
				this->nState = MiniGame::EMDICE_STATE_DETERMINE;
			break;
		case MiniGame::EMDICE_STATE_DETERMINE:
			Dice::_determineMatrix();
			this->nState = MiniGame::EMDICE_STATE_INTERPOLATE;
			break;
		case MiniGame::EMDICE_STATE_INTERPOLATE:
			if ( this->fMovingSpeed < 0.001f )
				this->nState = MiniGame::EMDICE_STATE_FINISH;
			break;
		case MiniGame::EMDICE_STATE_FINISH:
			Dice::_interpolateMatrix();
			return 0;
		}

		D3DXQUATERNION _quaternion;	
		::D3DXQuaternionRotationAxis(&_quaternion, &this->vRollingAxis, this->fRollingSpeed);

		D3DXMATRIXA16 _mQuaternion;
		::D3DXMatrixRotationQuaternion(&_mQuaternion, &_quaternion);		
		this->mWorld *= _mQuaternion;
		this->mWorld.m[3][0] = this->vPosition.x;
		this->mWorld.m[3][1] = this->vPosition.y;
		this->mWorld.m[3][2] = this->vPosition.z;
		this->mWorld.m[3][3] = 1.0f;

		return -1;
	}

	const bool Dice::isCollision(Dice& _targetDice)
	{
		D3DXVECTOR3 _vLength(_targetDice.vPosition - this->vPosition);		
		const float _fLength = ::D3DXVec3Length(&_vLength);
		::D3DXVec3Normalize(&_vLength, &_vLength);
		const float _fRate = (3.0f - _fLength) * 0.51f;
		if ( _fRate < 0.0f )
			return false;

		this->vPosition -= (_vLength * _fRate);
		_targetDice.vPosition += (_vLength * _fRate);

		const D3DXVECTOR3 _vMoveDirect(this->vMoveDirect - _targetDice.vMoveDirect);
		const float _fDot = ::D3DXVec3Dot(&_vMoveDirect, &_vLength) * 2.0f;
		{ // Dice_0;
			this->vMoveDirect = _vMoveDirect + (_vLength * -_fDot);
			this->fMovingSpeed *= 0.95f;
			::D3DXVec3Normalize(&this->vMoveDirect, &this->vMoveDirect);
			Dice::_setRotateAxis();
		}

		{ // Dice_1;
			_targetDice.vMoveDirect = (_vLength * _fDot) - _vMoveDirect;
			_targetDice.fMovingSpeed *= 0.95f;
			::D3DXVec3Normalize(&_targetDice.vMoveDirect, &_targetDice.vMoveDirect);
			_targetDice._setRotateAxis();
		}		

		return true;
	}


	const bool Dice::isCollision(const float _fWidth, const float _fHeight)
	{
		const D3DXVECTOR3 _vMinimum(-_fWidth, -_fHeight, 0.0f);
		const D3DXVECTOR3 _vMaximum(_fWidth, _fHeight, DEFDEPTH);

		const D3DXVECTOR3 _vCollision(MiniGame::_limit(this->vPosition, _vMinimum, _vMaximum) * -2.0f + D3DXVECTOR3(1.0f, 1.0f, 1.0f));
		this->vMoveDirect.x *= _vCollision.x;
		this->vMoveDirect.y *= _vCollision.y;
		this->vMoveDirect.z *= _vCollision.z;

		Dice::_setRotateAxis();

		const bool _bCollision = (_vCollision.x > 0.000001f) || (_vCollision.y > 0.000001f) || (_vCollision.z > 0.000001f);
		if ( this->nState != MiniGame::EMDICE_STATE_INTERPOLATE && _bCollision == false )
			return false;

		this->fMovingSpeed *= 0.96f;
		this->fRollingSpeed *= 0.97f;

		return _bCollision;
	}

	void Dice::_determineMatrix(void)
	{
		const float _fAngle = float(((::rand() & 1023) - 511) << 2) * 0.001f * D3DX_PI;

		D3DXMATRIXA16 _mRotateZ;
		::D3DXMatrixRotationZ(&_mRotateZ, _fAngle);
		this->mWorld = DiceConstNumber::mConstNumber[this->nDetermineNumber - 1] * _mRotateZ;
		this->mWorld.m[3][0] = this->vPosition.x;
		this->mWorld.m[3][1] = this->vPosition.y;
		this->mWorld.m[3][2] = this->vPosition.z;

		this->mDetermineWorld = this->mWorld;

		this->fRollingSpeed = 0.190f;
		this->fMovingSpeed = DEFLIMIT_SPEED;
	}

	void Dice::_interpolateMatrix(void)
	{
		const D3DXMATRIXA16 _mInterpolateMatrix((this->mDetermineWorld - this->mWorld) * DEFDICE_INTERPOLATE);
		this->mWorld = this->mDetermineWorld + _mInterpolateMatrix;		
		this->mWorld.m[3][0] = this->vPosition.x;
		this->mWorld.m[3][1] = this->vPosition.y;
		this->mWorld.m[3][2] = this->vPosition.z;
	}

	const D3DXVECTOR3 _limit(D3DXVECTOR3& _vTarget, const D3DXVECTOR3& _vMin, const D3DXVECTOR3& _vMax)
	{
		D3DXVECTOR3 _vCollision(0.0f, 0.0f, 0.0f);

		if ( _vTarget.x > _vMax.x )
		{
			_vTarget.x = _vMax.x;
			_vCollision.x = 1.0f;
		}
		else if ( _vTarget.x < _vMin.x )
		{
			_vTarget.x = _vMin.x;
			_vCollision.x = 1.0f;
		}

		if ( _vTarget.y > _vMax.y )
		{
			_vTarget.y = _vMax.y;
			_vCollision.y = 1.0f;
		}
		else if ( _vTarget.y < _vMin.y )
		{
			_vTarget.y = _vMin.y;
			_vCollision.y = 1.0f;
		}

		if ( _vTarget.z > _vMax.z )
		{
			_vTarget.z = _vMax.z;
			_vCollision.z = 1.0f;
		}
		else if ( _vTarget.z < _vMin.z )
		{
			_vTarget.z = _vMin.z;
			_vCollision.z = 1.0f;
		}

		return _vCollision;
	}
}

