#include "stdafx.h"

#include "EditMeshs.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


namespace vm
{

namespace EDITMESHS
{
	void RENDERPLANE ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vSize, D3DXVECTOR3 &vSnap )
	{
		//	Note : Set the world matrix
		//
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		static DWORD BOXVERTEX = D3DFVF_XYZ;
		static D3DXVECTOR3 VertBox[8];
		static WORD IndexBox[8] =  { 0,1 ,1,2, 2,3, 3,0 };

		DWORD OldLighting;
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

		//	Note : 메터리얼, 텍스쳐 지정.
		//
		pd3dDevice->SetTexture ( 0, NULL );
		pd3dDevice->SetFVF(BOXVERTEX);

		//	Note : 외각 라인.
		//
		VertBox[0] = vCenter + D3DXVECTOR3(-vSize.x,0,-vSize.z);
		VertBox[1] = vCenter + D3DXVECTOR3(-vSize.x,0,vSize.z);
		VertBox[2] = vCenter + D3DXVECTOR3(vSize.x,0,vSize.z);
		VertBox[3] = vCenter + D3DXVECTOR3(vSize.x,0,-vSize.z);

		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 4,
			IndexBox, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		//	Note : 축.
		//
		static WORD IndexAxis[4] = { 0,1, 2,3 };
		VertBox[0] = vCenter + D3DXVECTOR3(-vSize.x,0,0);
		VertBox[1] = vCenter + D3DXVECTOR3(+vSize.x,0,0);
		VertBox[2] = vCenter + D3DXVECTOR3(0,0,+vSize.z);
		VertBox[3] = vCenter + D3DXVECTOR3(0,0,-vSize.z);

		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
			IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );

		//	Note : 그리드 그리기.
		//
		D3DXVECTOR3 vLineA;
		D3DXVECTOR3 vLineB;

		vLineA = vCenter + D3DXVECTOR3(-vSize.x,0,0);
		vLineB = vCenter + D3DXVECTOR3(+vSize.x,0,0);
		for ( float fStep=vSnap.z; fStep<vSize.z; fStep+=vSnap.z )
		{
			VertBox[0] = vLineA + D3DXVECTOR3(0,0,fStep);
			VertBox[1] = vLineB + D3DXVECTOR3(0,0,fStep);
		
			pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
				IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );
		}

		for ( float fStep=vSnap.z; fStep<vSize.z; fStep+=vSnap.z )
		{
			VertBox[0] = vLineA + D3DXVECTOR3(0,0,-fStep);
			VertBox[1] = vLineB + D3DXVECTOR3(0,0,-fStep);
		
			pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
				IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );
		}

		vLineA = vCenter + D3DXVECTOR3(0,0,-vSize.z);
		vLineB = vCenter + D3DXVECTOR3(0,0,+vSize.z);
		for ( float fStep=vSnap.x; fStep<vSize.x; fStep+=vSnap.x )
		{
			VertBox[0] = vLineA + D3DXVECTOR3(+fStep,0,0);
			VertBox[1] = vLineB + D3DXVECTOR3(+fStep,0,0);
		
			pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
				IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );
		}

		for ( float fStep=vSnap.x; fStep<vSize.x; fStep+=vSnap.x )
		{
			VertBox[0] = vLineA + D3DXVECTOR3(-fStep,0,0);
			VertBox[1] = vLineB + D3DXVECTOR3(-fStep,0,0);
		
			pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 4, 2,
				IndexAxis, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(BOXVERTEX) );
		}

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );
	}
};

};