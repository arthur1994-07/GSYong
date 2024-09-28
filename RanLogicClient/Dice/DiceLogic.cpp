#include "pch.h"
#include <time.h>
#include "../../EngineLib/DxSound/DxSoundLib.h"
#include "SDice.h"
#include "DiceLogic.h"

DiceLogic::DiceLogic(const unsigned int _nDice, const unsigned int _iStageSizeX, const unsigned int _iStageSizeY) : 
bMoving(false), bGrab(false), nDice(_nDice), iWidth(_iStageSizeX), iHeight(_iStageSizeY) {
	::srand((unsigned int)::time(NULL));
	if ( _nDice == 1 )
		this->pDice = new SDICE;
	if ( _nDice )
		this->pDice = new SDICE[_nDice];

	const float _fSize = 3.0f;
	const float _fAngle = (D3DX_PI * 2.0f / float(_nDice - 1));

#if g_bSSE2_DICE
	const M128 _xSize = ::_mm_set1_ps(_fSize);
	this->pDice[_nDice - 1].xGrabPosition = ::_mm_setzero_ps();
	for ( unsigned int _i = _nDice - 1; _i; --_i ){
		SDICE& _sDice = this->pDice[_i - 1];
		const M128 _xSinCos = ::_mm_set_ps(0.0f, 0.0f, ::sin(_fAngle * float(_i - 1)), ::cos(_fAngle * float(_i - 1)));		
		_sDice.xGrabPosition = ::_mm_mul_ps(_xSize, _xSinCos);
	}
#else	
	this->pDice[_nDice - 1].vGrabPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	for ( unsigned int _i = _nDice - 1; _i; --_i ){
		SDICE& _sDice = this->pDice[_i - 1];
		const float _fSin = ::sin(_fAngle * float(_i - 1));
		const float _fCos = ::cos(_fAngle * float(_i - 1));
		_sDice.vGrabPosition = D3DXVECTOR3(_fCos * _fSize, _fSin * _fSize, 0.0f);
	}
#endif	
}

/**
 * 주사위를 집어 들고 있는 좌표로 호출;  
 * 처음 호출 되면 HoldOut()가 호출 되기 전까진 상태는 유지되나 이동 좌표 갱신은 되지 않음;
 *
 * @param _fPositionX					[in] X 절대 좌표;
 * @param _fPositionY					[in] Y 절대 좌표;
 * @param _fPositionZ					[in] Z 절대 좌표;
 */
void DiceLogic::Grab(const float _fPositionX, const float _fPositionY, const float _fPositionZ){
	if ( this->bMoving )
		return;
	if ( ::abs(_fPositionX) > 1.0f || ::abs(_fPositionY) > 1.0f )		
		return;
	this->bGrab = true;

	const float _fPosition[2] = {_fPositionX * float(this->iWidth), _fPositionY * float(this->iHeight)};
	for ( unsigned int _i = this->nDice; _i; --_i )		
		this->pDice[_i - 1].SetDice(_fPosition[0], _fPosition[1], _fPositionZ);
}

/**
 * 주사위를 집어 들고 있는 좌표로 호출;  
 * 처음 호출 되면 HoldOut()가 호출 되기 전까진 상태는 유지되나 이동 좌표 갱신은 되지 않음;
 *
 * @param _fPositionX					[in] X 상대 좌표(-1.0 ~ 1.0f);
 * @param _fPositionY					[in] Y 상대 좌표(-1.0 ~ 1.0f);
 * @param _fPositionZ					[in] Z 절대 좌표;
 */
void DiceLogic::Grab(const int _iPositionX, const int _iPositionY, const int _iPositionZ){
	if ( this->bMoving )
		return;
	if ( ::abs(_iPositionX) > int(this->iWidth) || ::abs(_iPositionY) > int(this->iHeight) )		
		return;
	this->bGrab = true;

	for ( unsigned int _i = this->nDice; _i; --_i )		
		this->pDice[_i - 1].SetDice(float(_iPositionX), float(_iPositionY), float(_iPositionZ));
}


/**
 * 주사위를 놓음;  
 * HoldOut() 함수가 호출 되면 주사위 결과가 나오기전까지 잠금 상태로 전환;
 * 주의점 : 결과 값을 먼저 입력 받음;
 *
 * @param _pDetNum					[in] 결과값 배열; 
 */
void DiceLogic::HoldOut(const unsigned int* const _pDetNum){
	if ( this->bGrab && !this->bMoving ){
		this->bGrab = false;
		this->bMoving = true;		
		for ( unsigned int _i = this->nDice; _i; --_i )			
			this->pDice[_i - 1].SetDice(1.5f, _pDetNum[_i - 1]);
	}
}


/** 
 * 란의 FrameMove()에 해당하는 함수;
 * 결과가 나오면 잠금 상태 해제;
*/
const bool DiceLogic::Update(void){
	unsigned int _iExitCount = 0;	
	for ( unsigned int _i = this->nDice; _i; --_i ){
		SDICE& _sDice = this->pDice[_i - 1];
		_sDice.Moving();
		if ( _sDice.nCrashCount > (8 - 1) )
			_iExitCount += _sDice.RevisionAxis();		
		_sDice.Rolling();
	}
	DiceLogic::CheckWall();	
	DiceLogic::CheckCrash();	

	if ( _iExitCount == this->nDice && this->bMoving ){
		this->bMoving = false;
		return false;
	}
	return true;
}





DiceLogic::~DiceLogic(void){
	if ( this->nDice == 1)
		delete this->pDice;
	if ( this->nDice )
		delete[] this->pDice;
	this->pDice = NULL;
}
const unsigned int DiceLogic::GetNDice(void){
	return this->nDice;
}
const D3DXMATRIXA16& DiceLogic::GetMatrix(const unsigned int _iIndex){
	return this->pDice[_iIndex].mWorld;
}

void DiceLogic::CheckWall(void){
	for ( unsigned int _i = this->nDice; _i; --_i ){
		SDICE& _sDice = this->pDice[_i - 1];

		const float _fHalfSize[2] = {float(this->iWidth >> 1), float(this->iHeight >> 1)};
		const float _fPosition[2] = {_sDice.mWorld.m[3][0],_sDice.mWorld.m[3][1]};

		bool _bWallCrash = false;		
#if g_bSSE2_DICE
		SSEFLOAT4& _vMoveDirect = _sDice.vMoveDirect;		
#else
		D3DXVECTOR3& _vMoveDirect = _sDice.vMoveDirect;
#endif	

		if ( _fPosition[0] > _fHalfSize[0] ){
			_sDice.mWorld.m[3][0] = _fHalfSize[0];
			_vMoveDirect.x = -_vMoveDirect.x;
			++_sDice.nCrashCount;
			_bWallCrash = true;
			if ( !this->bGrab )
				DxSoundLib::GetInstance()->PlaySound("DICE_ROLL");
		}
		if ( _fPosition[0] < -_fHalfSize[0] ){
			_sDice.mWorld.m[3][0] = -_fHalfSize[0];
			_vMoveDirect.x = -_vMoveDirect.x;
			++_sDice.nCrashCount;
			_bWallCrash = true;
			if ( !this->bGrab )
				DxSoundLib::GetInstance()->PlaySound("DICE_ROLL");
		}

		if ( _fPosition[1] > _fHalfSize[1] ){
			_sDice.mWorld.m[3][1] = _fHalfSize[1];
			_vMoveDirect.y = -_vMoveDirect.y;
			++_sDice.nCrashCount;
			_bWallCrash = true;
			if ( !this->bGrab )
				DxSoundLib::GetInstance()->PlaySound("DICE_ROLL");
		}
		if ( _fPosition[1] < -_fHalfSize[1] ){
			_sDice.mWorld.m[3][1] = -_fHalfSize[1];
			_vMoveDirect.y = -_vMoveDirect.y;
			++_sDice.nCrashCount;
			_bWallCrash = true;
			if ( !this->bGrab )
				DxSoundLib::GetInstance()->PlaySound("DICE_ROLL");
		}

		if ( _sDice.mWorld.m[3][2] > 0.0f ){
			_sDice.mWorld.m[3][2] = 0.0f;
			_vMoveDirect.z = -_vMoveDirect.z * 0.6f;
			++_sDice.nCrashCount;
			_bWallCrash = true;			
		}

		if ( _sDice.mWorld.m[3][2] < -2.0f ){
			_sDice.mWorld.m[3][2] = -2.0f;
			_vMoveDirect.z = -_vMoveDirect.z * 0.6f;
			++_sDice.nCrashCount;
			_bWallCrash = true;			
		}


		if ( _sDice.nCrashCount < 8 ){
			if ( _bWallCrash ){
				_sDice.fMovingSpeed *= 0.75f;
				_sDice.fRollingSpeed *= 0.75f;
				_sDice.SetAxis();
			}
		} else {
			_vMoveDirect.z = 0.0f;
			_sDice.fMovingSpeed = _sDice.fMovingSpeed < 0.0001f ? 0.0f : _sDice.fMovingSpeed - 0.004f;
			_sDice.fRollingSpeed = _sDice.fRollingSpeed < 0.0001f ? 0.0f : _sDice.fRollingSpeed - 0.001f;		
		}
		if ( this->bGrab )
			_sDice.nCrashCount = 0;
	}
}

void DiceLogic::CheckCrash(void){
	for ( unsigned int _i = 0; _i < this->nDice - 1; ++_i ){		
		SDICE& _sDice0 = this->pDice[_i];
		for ( unsigned int _j = _i + 1; _j < this->nDice; ++_j )	
		{
			if ( _sDice0.isCrash(this->pDice[_j]) ){
				DxSoundLib::GetInstance()->PlaySound("DICE_ROLL");				
			}
				
		}
	}
}

