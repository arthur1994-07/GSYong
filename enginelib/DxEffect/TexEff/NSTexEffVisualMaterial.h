#pragma once

namespace NSVMRender
{
	class DxVMStack;
};

//------------------------------------------------------------------------------
//      VisualMaterial �� Translucent �������� ������ �ϱ� ���� namespace
namespace NSTexEffVisualMaterial
{
    void Render( LPDIRECT3DDEVICEQ pd3dDevice, NSVMRender::DxVMStack* pVMStack );
}