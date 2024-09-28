#pragma once

#include "../../enginelib/DxMeshs/DxSimpleMeshMan.h"

#include "./GLWidgetTexture.h"

namespace MiniGame
{
	class GLManagerClient;
	class IDice;
}
namespace WidgetTexture
{
	struct RenderState
	{	
		DWORD dwLight;
		DWORD dwNormalize;
		DWORD dwSpecular;
		DWORD dwAntialiased;
		DWORD dwMagFilter, dwMinFilter;	
		D3DLIGHT9 light[3];
		D3DMATERIAL9 material;
		D3DXMATRIXA16 matrixViewPrevious;
		D3DXMATRIXA16 matrixProjectionPrevious;	
	};

	class Dice : public GLWidgetTexture
	{
	public:
		// 텍스처 사이즈 설정, default : 1024 x 1024
		virtual void InitTextureSize();

		virtual bool OnCreate(void) override;
		virtual void OnResetDevice(void) override;
		virtual void OnLostDevice(void) override;
		virtual void OnFrameMove(float fTimeElapsed) override;
		virtual void OnRender(void) override;

	public:
		void setManagerMiniGame(MiniGame::GLManagerClient* const _pManagerMiniGame);

	private:
		void _renderSetRenderState(LPDIRECT3DDEVICEQ pd3dDevice, RenderState& _sRenderState);
		void _renderRenderItem(LPDIRECT3DDEVICEQ pd3dDevice);
		void _renderRecoverRenderState(LPDIRECT3DDEVICEQ pd3dDevice, const RenderState& _sRenderState);
		const HRESULT _renderRenderDiceTable(LPDIRECT3DDEVICEQ pd3dDevice);

	public:
		Dice(LPDIRECT3DDEVICEQ pDevice);
		virtual ~Dice(void);

	protected:
		MiniGame::GLManagerClient* pManagerMiniGame;
		D3DXMATRIX matrixView;
		D3DXMATRIX matrixProjection;	

		D3DMATERIAL9			material;
		D3DLIGHT9					light[3];

		DxSimMeshResource		dxMeshDice[2];
		MiniGame::IDice*			pDice; // 주사위 로직 객체;
	};

}

