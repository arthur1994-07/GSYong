#include "pch.h"

#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"

#include "../../DxEffect/Frame/DxEffectCloth.h"
#include "../../DxEffect/Frame/DxEffectClothForMaterial.h"

#include "./DxPieceEffMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxPieceEffMan& DxPieceEffMan::GetInstance()
{
	static DxPieceEffMan Instance;
	return Instance;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxPieceEffMan::DxPieceEffMan() :
	pEffectTypeList(NULL)
{
	//RegistType ( DxEffectCloth::TYPEID,				DxEffectCloth::NAME );
	RegistType ( DxEffectClothForMaterial::TYPEID,	DxEffectClothForMaterial::NAME );
}

DxPieceEffMan::~DxPieceEffMan()
{
	SAFE_DELETE(pEffectTypeList);
}

void DxPieceEffMan::RegistType ( const DWORD TypeID, const char *Name )
{
	GASSERT( Name );

	//	Note : �� ��� ������ ����.
	//
	DxEffectType *pNewType = new DxEffectType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : ����Ʈ�� ���.
	//
	pNewType->pNextType = pEffectTypeList;
	pEffectTypeList = pNewType;
}

void DxPieceEffMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//DxEffectCloth::CreateDevice ( pd3dDevice );
}

void DxPieceEffMan::OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//DxEffectCloth::ReleaseDevice ( pd3dDevice );
}

DxEffectBase* DxPieceEffMan::CreateEffInstance ( DWORD TypeID )
{
	DxEffectBase* pEffectBase = NULL;

	if( DxEffectCloth::TYPEID==TypeID )						pEffectBase = new DxEffectCloth ();
	else if( DxEffectClothForMaterial::TYPEID==TypeID )		pEffectBase = new DxEffectClothForMaterial ();

	return pEffectBase;
}

DxEffectBase* DxPieceEffMan::AdaptToDxFrame ( DWORD TypeID, DxFrameMesh *pFrameMesh, DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, 
											 PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts )
{
	HRESULT hr;

	//	Note : Effect Instance ����.
	DxEffectBase* pEffBase = CreateEffInstance( TypeID );
	if ( !pEffBase ) return NULL;

	//	Note : Property, Affine  ���� ����.
	if ( pProperty )	pEffBase->SetProperty( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffBase->IsUseAffineMatrix() )	pEffBase->SetAffineValue ( pAffineParts );

	pEffBase->AdaptToDxFrame ( pFrame, pd3dDevice );

	hr = pEffBase->Create ( pd3dDevice );
	if ( FAILED(hr) )	goto E_ERROR;

	pFrameMesh->AddEffect( pEffBase );

	if ( pEffBase->GetFlag () & _EFF_REPLACE )
	{
		//	Note : ���ĸ� ����Ʈ���� ����.
		pFrameMesh->DelAlphaMapFrame ( pFrame );
	}

	return pEffBase;

E_ERROR:
	pEffBase->CleanUp();
	SAFE_DELETE(pEffBase);
	return NULL;
}