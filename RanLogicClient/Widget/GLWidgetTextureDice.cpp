#include "pch.h"

#include "../../RanLogic/Msg/GLContrlMiniGameMsg.h"
#include "../MiniGame/GLMiniGameManager.h"
#include "../MiniGame/GLMiniGameFactory.h"
#include "../MiniGame/OddEven/GLOddEven.h"
#include "./GLWidgetTextureDice.h"

namespace WidgetTexture
{
	Dice::Dice(LPDIRECT3DDEVICEQ _pDevice)
		: GLWidgetTexture(_pDevice)
		, pManagerMiniGame(NULL)
		, pDice(MiniGame::createDice(MiniGame::EMODDEVEN_MAX_DICE, 6, 8))
	{
	}
	Dice::~Dice(void)
	{
		MiniGame::destroyDice(this->pDice);
	}
	void Dice::setManagerMiniGame(MiniGame::GLManagerClient* const _pManagerMiniGame)
	{
		this->pManagerMiniGame = _pManagerMiniGame;
	}

	void Dice::InitTextureSize()
	{
		m_nRenderTextureWidth = 512;
		m_nRenderTextureHeight = 512;
	}

	bool Dice::OnCreate(void)
	{
		if( false == GLWidgetTexture::OnCreate() )
			return false;

		this->dxMeshDice[0] = DxSimpleMeshMan::GetInstance().Load ( 
			"dice_red.x", 
			FALSE, FALSE, TEXTURE_RESIZE_NONE, TEXTURE_EFFECT );	
		this->dxMeshDice[1] = DxSimpleMeshMan::GetInstance().Load ( 
			"dice_black.x", 
			FALSE, FALSE, TEXTURE_RESIZE_NONE, TEXTURE_EFFECT );	

		ZeroMemory(&light[0], sizeof(D3DLIGHT9));
		ZeroMemory(&light[1], sizeof(D3DLIGHT9));
		ZeroMemory(&light[2], sizeof(D3DLIGHT9));
		D3DXVECTOR3 _vLight0(1.0f, -1.0f, 0.0f);
		D3DXVECTOR3 _vLight1(1.0f, 1.0f, 0.0f);
		D3DXVECTOR3 _vLight2(-0.5f, -1.0f, 0.0f);
		D3DXVec3Normalize(&_vLight0, &_vLight0);
		D3DXVec3Normalize(&_vLight1, &_vLight1);
		D3DXVec3Normalize(&_vLight2, &_vLight2);

		const float _fFactor(1.0f / 255.0f);
		this->light[0].Type = D3DLIGHT_DIRECTIONAL;	
		this->light[0].Direction = _vLight0; 
		this->light[0].Diffuse	= D3DXCOLOR(255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f);	
		this->light[0].Ambient	= D3DXCOLOR(211.0f * _fFactor, 211.0f * _fFactor, 211.0f * _fFactor, 1.0f);
		this->light[0].Specular	= D3DXCOLOR(255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f);	

		this->light[1].Type = D3DLIGHT_DIRECTIONAL;	
		this->light[1].Direction = _vLight1; 
		this->light[1].Diffuse	= D3DXCOLOR(139.0f * _fFactor, 106.0f * _fFactor, 0.0f * _fFactor, 1.0f);	
		this->light[1].Ambient	= D3DXCOLOR(11.0f * _fFactor, 11.0f * _fFactor, 11.0f * _fFactor, 1.0f);	
		this->light[1].Specular	= D3DXCOLOR(139.0f * _fFactor, 91.0f * _fFactor, 26.0f * _fFactor, 1.0f);	

		this->light[2].Type = D3DLIGHT_DIRECTIONAL;	
		this->light[2].Direction = _vLight2; 
		this->light[2].Diffuse	= D3DXCOLOR(69.0f * _fFactor, 96.0f * _fFactor, 255.0f * _fFactor, 1.0f);	
		this->light[2].Ambient	= D3DXCOLOR(7.0f * _fFactor, 7.0f * _fFactor, 7.0f * _fFactor, 1.0f);	
		this->light[2].Specular	= D3DXCOLOR(151.0f * _fFactor, 171.0f * _fFactor, 255.0f * _fFactor, 1.0f);	

		ZeroMemory(&this->material, sizeof(this->material));
		this->material.Diffuse	= D3DXCOLOR(255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f);	
		this->material.Ambient	= D3DXCOLOR(163.0f * _fFactor, 163.0f * _fFactor, 163.0f * _fFactor, 1.0f);	
		this->material.Specular	= D3DXCOLOR(237.0f * _fFactor, 237.0f * _fFactor, 237.0f * _fFactor, 1.0f);	
		this->material.Emissive	= D3DXCOLOR(0.0f * _fFactor, 0.0f * _fFactor, 0.0f * _fFactor, 1.0f);	
		this->material.Power = 8.8f;

		::D3DXMatrixLookAtLH(&this->matrixView,  &D3DXVECTOR3(1.8f, 0.0f, -19.0f), &D3DXVECTOR3(2.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		::D3DXMatrixPerspectiveFovLH(&this->matrixProjection, 0.2f * D3DX_PI, 1.0f, 1.0f, 128.0f);	

		return true;
	}
	void Dice::OnResetDevice(void)
	{
		GLWidgetTexture::OnResetDevice();
	}
	void Dice::OnLostDevice(void)
	{
		GLWidgetTexture::OnLostDevice();
	}
	void Dice::OnFrameMove(float fTimeElapsed)
	{
		if ( m_nRefCount <= 0 )
			return;

		MiniGame::GLManagerClient* const _pManagerMiniGame(this->pManagerMiniGame);
		const MiniGame::GLOddEvenClient* const _pMiniGameOddEven((const MiniGame::GLOddEvenClient* const)this->pManagerMiniGame->GetMiniGame(MiniGame::EMMINIGAME_ODDEVEN));
		if ( (_pMiniGameOddEven == NULL) || (this->pDice == NULL) )
		{
			_pManagerMiniGame->RequestClose();
			return; // 닫기 요청 해야 함;
		}

		const MiniGame::EMODDEVEN_STAGE _typeStage(_pMiniGameOddEven->getCurrentStage());
		switch ( _typeStage )
		{
		case MiniGame::EMODDEVEN_STAGE_READY:
			this->pDice->reset();
			break;
		case MiniGame::EMODDEVEN_STAGE_SELECT:			
			break;
		case MiniGame::EMODDEVEN_STAGE_ROLL:			
			const float _fX(sc::Random::getInstance().RANDOM_POS());
			const float _fY(sc::Random::getInstance().RANDOM_POS());
			this->pDice->grab(_fX, _fY);

			const DWORD* const nDetermineNumber(_pMiniGameOddEven->getDetermineNumber());
			this->pDice->holdOut((const unsigned int* const)nDetermineNumber, _fX, _fY);	
			if ( this->pDice->update(0.0f, fTimeElapsed) != MiniGame::EMDICE_GAME_STATE_FINISH )
				break;

			this->pDice->lock();
			_pManagerMiniGame->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RESULT());
			break;
		}
	}
	void Dice::OnRender(void)
	{
		LPDIRECT3DDEVICEQ _pDevice(this->m_pDevice);

		RenderState _staterender;
		Dice::_renderSetRenderState(_pDevice, _staterender);	
		Dice::_renderRenderItem(_pDevice);
		Dice::_renderRecoverRenderState(_pDevice, _staterender);
	}

	void Dice::_renderSetRenderState(LPDIRECT3DDEVICEQ _pDevice, RenderState& _staterender)
	{
		_pDevice->GetMaterial(&_staterender.material);
		_pDevice->GetLight( 0, &_staterender.light[0] );
		_pDevice->GetLight( 1, &_staterender.light[1] );
		_pDevice->GetLight( 2, &_staterender.light[2] );
		_pDevice->GetRenderState( D3DRS_LIGHTING, &_staterender.dwLight );
		_pDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &_staterender.dwNormalize );
		_pDevice->GetRenderState( D3DRS_SPECULARENABLE, &_staterender.dwSpecular );
		_pDevice->GetRenderState( D3DRS_ANTIALIASEDLINEENABLE, &_staterender.dwAntialiased);
		_pDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &_staterender.dwMagFilter);	
		_pDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &_staterender.dwMinFilter);	

		_pDevice->GetTransform( D3DTS_VIEW, &_staterender.matrixViewPrevious );
		_pDevice->GetTransform( D3DTS_PROJECTION, &_staterender.matrixProjectionPrevious);

		_pDevice->SetRenderState( D3DRS_LIGHTING, true );
		_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, true );
		_pDevice->SetRenderState( D3DRS_SPECULARENABLE, true );
		_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	
		_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	
		_pDevice->SetMaterial(&this->material);
		_pDevice->SetLight(0, &this->light[0]);
		_pDevice->SetLight(1, &this->light[1]);
		_pDevice->SetLight(2, &this->light[2]);
		_pDevice->LightEnable(0, true);		
		_pDevice->LightEnable(1, true);	
		_pDevice->LightEnable(2, true);	

		_pDevice->SetTransform( D3DTS_VIEW, &this->matrixView );
		_pDevice->SetTransform( D3DTS_PROJECTION, &this->matrixProjection);
	}
	void Dice::_renderRenderItem(LPDIRECT3DDEVICEQ _pDevice)
	{
		MiniGame::IDice* const _pDice(this->pDice);
		if ( _pDice == NULL )
			return;

		for ( unsigned int _index(_pDice->getNDice()); _index--; )
		{			
			DxSimMesh* const _pSimMeshDice(this->dxMeshDice[_index].get());
			if ( _pSimMeshDice == NULL )
				break;

			const D3DXMATRIX& _mWorld = D3DXMATRIX(*_pDice->getDiceWorldMatrix(_index));

			_pSimMeshDice->UseMeshMaterials(TRUE);
			_pSimMeshDice->RenderItem(_pDevice, _mWorld, &this->material);
		}
	}
	void Dice::_renderRecoverRenderState(LPDIRECT3DDEVICEQ _pDevice, const RenderState& _staterender)
	{
		_pDevice->SetMaterial(&_staterender.material);
		_pDevice->SetLight(0, &_staterender.light[0]);
		_pDevice->SetLight(1, &_staterender.light[1]);
		_pDevice->SetLight(2, &_staterender.light[2]);
		_pDevice->LightEnable(0, true);
		_pDevice->LightEnable(1, false);
		_pDevice->LightEnable(2, false);

		_pDevice->SetRenderState( D3DRS_LIGHTING, _staterender.dwLight );
		_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, _staterender.dwNormalize );
		_pDevice->SetRenderState( D3DRS_SPECULARENABLE, _staterender.dwSpecular );
		_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, _staterender.dwMagFilter);	
		_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, _staterender.dwMinFilter);		

		_pDevice->SetTransform( D3DTS_VIEW, &_staterender.matrixViewPrevious );
		_pDevice->SetTransform( D3DTS_PROJECTION, &_staterender.matrixProjectionPrevious);
	}	
}
