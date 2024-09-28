#include "pch.h"

#include "./DxTexEffDiffuse.h"
#include "./DxTexEffFlowUV.h"
#include "./DxTexEffRotate.h"
#include "./DxTexEffSpecular.h"
#include "./DxTexEffVisualMaterial.h"
#include "./DxTexEff.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxTexEffBase::DxTexEffBase() :
	m_pNext(NULL)
{
}

DxTexEffBase::~DxTexEffBase()
{
	SAFE_DELETE( m_pNext );
}

DxTexEffBase* DxTexEffBase::NEW_TEXEFF_STATIC( DWORD dwType )
{
    DxTexEffBase* pNew(NULL);
    switch ( dwType )
    {
    case DEF_TEXEFF_DIFFUSE:
        {
            pNew = new DxTexEffDiffuse;
        }
        break;

    case DEF_TEXEFF_FLOWUV:
        {
            pNew = new DxTexEffFlowUV;
        }
        break;

    case DEF_TEXEFF_ROTATE:
        {
            pNew = new DxTexEffRotate;
        }
        break;

    case DEF_TEXEFF_SPECULAR:
        {
            pNew = new DxTexEffSpecular;
        }
        break;

    case DEF_TEXEFF_VISUAL_MATERIAL:
        {
            pNew = new DxTexEffVisualMaterial;
        }
        break;
    };

    return pNew;
}