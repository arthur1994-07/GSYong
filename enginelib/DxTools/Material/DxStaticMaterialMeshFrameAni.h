// DxStaticMaterialMeshFrameAni.h
//
//	class DxStaticMaterialMeshFrameAni : ���ο� DxStaticMaterialMeshAni ������ �ƴ� ID �� ������ �ִ�.
//										���ο� �� ��� Render �� �ʿ���� Tree �� Ÿ���� ��쵵 ���ܼ�, �и����� ���Ҵ�.
//
//	Name : Sung-Hwan Han
//	Begin :2013/07/09
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../../../SigmaCore/File/SerialFile.h"

struct DxAnimation;
struct DxFrame;
class CMaterialSetting;
class DxStaticMaterialMeshAniMan;


class DxStaticMaterialMeshFrameAni
{
private:
	enum
	{
		VERSION = 0x0100,           // �⺻���� Version
	};

private:

	union
	{
		struct { D3DXVECTOR3 m_vTreeMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 m_vTreeMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	//	Note : Ʈ������ ����.
	//
	D3DXMATRIX		m_matRot;				//	�ش�ð�(���ϰ� ������)�� Ʈ������.
	D3DXMATRIX		m_matCombined;			//	�θ� ���� ������ ���� Ʈ������.

	DxAnimation		*m_pframeFromAnimate;	//	�� �������� �����ϴ� ���ϸ��̼�.

	DWORD			m_dwStaticMeshMaterialAniID;	// DxStaticMaterialMeshAni �� ����� ID ( ���߿� Load �� �������ֱ� ����. )

	//	Note : �ڽ� ��� �� ���� ��� ����.
	DxStaticMaterialMeshFrameAni			*m_pframeSibling;		//	���� ����
	DxStaticMaterialMeshFrameAni			*m_pframeFirstChild;	//	�ڽ� ����

public:
	void UpdateMatrix( float fGlobalTime, const D3DXMATRIX &matCur, bool bAniLoop=true );
	void UpdateMirrorMatrix( float fHeight );

	void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3&  vMin );

	void Import( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxFrame* pframeCur, 
				float fUNITTIME, 
				DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, 
				const CMaterialSetting *pMaterialSetting, 
				std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer,
				float fCellSize,
				BOOL bParentAnimation=FALSE );			// bParentAnimation - ���ο��� ���̴� �ڵ�. ����ڰ� ������ �ʿ䰡 ����.

	void CleanUp();

public:
	void Save ( sc::SerialFile& SFile );
	void Load ( sc::SerialFile& SFile, DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer );

private:
	void SaveTree( sc::SerialFile& SFile );
	void LoadTree( sc::SerialFile& SFile, DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer );

public:
	DxStaticMaterialMeshFrameAni();
	~DxStaticMaterialMeshFrameAni();
};