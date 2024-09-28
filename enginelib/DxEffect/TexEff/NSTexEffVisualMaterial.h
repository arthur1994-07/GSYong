#pragma once

namespace NSVMRender
{
	class DxVMStack;
};

//------------------------------------------------------------------------------
//      VisualMaterial 의 Translucent 재질들을 렌더링 하기 위한 namespace
namespace NSTexEffVisualMaterial
{
    void Render( LPDIRECT3DDEVICEQ pd3dDevice, NSVMRender::DxVMStack* pVMStack );
}