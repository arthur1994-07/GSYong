#pragma once

namespace NSVertexDeclaration
{
	bool BuildVertexDeclaration( LPDIRECT3DDEVICEQ pd3dDevice, 
									ID3DXEffect* pFX, 
									const char* pSemantic, 
									DWORD& dwFVF, 
									LPDIRECT3DVERTEXDECLARATION9& pDCRT, 
									D3DVERTEXELEMENT9** ppVertElements,
									BOOL& bLightMap,
									BOOL& bLightMap_float4,
									int& nLightMapIndex,
									BOOL& bTangent, 
									BOOL& bBinormal );
};