#ifndef _DXUT_UTIL_H_
#define _DXUT_UTIL_H_

#pragma once

#define GXRELEASE(_p) do { if ((_p) != NULL) {(_p)->Release(); (_p) = NULL;} } while (0)

////-----------------------------------------------------------------------------
//// Name: D3DUtil_InitMaterial()
//// Desc: Initializes a D3DMATERIAL8 structure, setting the diffuse and ambient
////       colors. It does not set emissive or specular colors.
////-----------------------------------------------------------------------------
//VOID D3DUtil_InitMaterial( D3DMATERIAL9& mtrl, FLOAT r=0.0f, FLOAT g=0.0f,
//                                               FLOAT b=0.0f, FLOAT a=1.0f );

////-----------------------------------------------------------------------------
//// Name: D3DUtil_InitLight()
//// Desc: Initializes a D3DLIGHT structure, setting the light position. The
////       diffuse color is set to white, specular and ambient left as black.
////-----------------------------------------------------------------------------
//VOID D3DUtil_InitLight( D3DLIGHT9& light, D3DLIGHTTYPE ltType,
//                        FLOAT x=0.0f, FLOAT y=0.0f, FLOAT z=0.0f );

////-----------------------------------------------------------------------------
//// Name: D3DUtil_GetCubeMapViewMatrix()
//// Desc: Returns a view matrix for rendering to a face of a cubemap.
////-----------------------------------------------------------------------------
//D3DXMATRIX D3DUtil_GetCubeMapViewMatrix( DWORD dwFace );

//inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

#endif // _DXUT_UTIL_H_