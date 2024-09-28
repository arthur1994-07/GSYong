#include "pch.h"
#include "DXUT_UTIL.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

////-----------------------------------------------------------------------------
//// Name: D3DUtil_InitMaterial()
//// Desc: Initializes a D3DMATERIAL8 structure, setting the diffuse and ambient
////       colors. It does not set emissive or specular colors.
////-----------------------------------------------------------------------------
//VOID D3DUtil_InitMaterial( D3DMATERIAL9& mtrl, FLOAT r, FLOAT g, FLOAT b,
//                           FLOAT a )
//{
//    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
//	mtrl.Diffuse.r = mtrl.Ambient.r = mtrl.Specular.r = r;
//    mtrl.Diffuse.g = mtrl.Ambient.g = mtrl.Specular.g = g;
//    mtrl.Diffuse.b = mtrl.Ambient.b = mtrl.Specular.b = b;
//    mtrl.Diffuse.a = mtrl.Ambient.a = mtrl.Specular.a = a;
//	mtrl.Power = 14.f;
//}
//
//


////-----------------------------------------------------------------------------
//// Name: D3DUtil_InitLight()
//// Desc: Initializes a D3DLIGHT structure, setting the light position. The
////       diffuse color is set to white; specular and ambient are left as black.
////-----------------------------------------------------------------------------
//VOID D3DUtil_InitLight( D3DLIGHT9& light, D3DLIGHTTYPE ltType,
//                        FLOAT x, FLOAT y, FLOAT z )
//{
//    ZeroMemory( &light, sizeof(D3DLIGHT9) );
//    light.Type        = ltType;
//    light.Diffuse.r   = 1.0f;
//    light.Diffuse.g   = 1.0f;
//    light.Diffuse.b   = 1.0f;
//    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3(x, y, z) );
//    light.Position.x   = x;
//    light.Position.y   = y;
//    light.Position.z   = z;
//    light.Range        = 1000.0f;
//}

////-----------------------------------------------------------------------------
//// Name: D3DUtil_GetCubeMapViewMatrix()
//// Desc: Returns a view matrix for rendering to a face of a cubemap.
////-----------------------------------------------------------------------------
//D3DXMATRIX D3DUtil_GetCubeMapViewMatrix( DWORD dwFace )
//{
//    D3DXVECTOR3 vEyePt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//    D3DXVECTOR3 vLookDir;
//    D3DXVECTOR3 vUpDir;
//
//    switch( dwFace )
//    {
//        case D3DCUBEMAP_FACE_POSITIVE_X:
//            vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
//            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
//            break;
//        case D3DCUBEMAP_FACE_NEGATIVE_X:
//            vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
//            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
//            break;
//        case D3DCUBEMAP_FACE_POSITIVE_Y:
//            vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
//            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
//            break;
//        case D3DCUBEMAP_FACE_NEGATIVE_Y:
//            vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
//            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
//            break;
//        case D3DCUBEMAP_FACE_POSITIVE_Z:
//            vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
//            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
//            break;
//        case D3DCUBEMAP_FACE_NEGATIVE_Z:
//            vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
//            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
//            break;
//    }
//
//    // Set the view transform for this cubemap surface
//    D3DXMATRIX matView;
//    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookDir, &vUpDir );
//    return matView;
//}
//
