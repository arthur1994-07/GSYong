#pragma once

#include <string>

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

#define		ISTEXTURE1		0x0001
#define		ISTEXTURE2		0x0002
#define		ISTEXTURE3		0x0004
#define		ISTEXTURE4		0x0008
#define		USETEXTURE1		0x0010
#define		USETEXTURE2		0x0020
#define		USETEXTURE3		0x0040
#define		USETEXTURE4		0x0080

#define		NO_UPDOWN		0x0100

enum 
{ 
    EFF_CHAR_TEXT_SIZE = 28,
};

struct EFFCHAR_PROPERTY_TEXDIFF_100
{
	DWORD			m_dwFlag;
	DWORD			m_dwMaterials;

	float			m_fSpeed;

	// 28 바이트는 릴리즈모드때 std::string sizeof값
	//	std::string		m_strTex;
	char			m_strTex[EFF_CHAR_TEXT_SIZE];
};

struct EFFCHAR_PROPERTY_TEXDIFF_101	//	데이터 포맷 변경 진행
{
	DWORD			m_dwFlag;
	DWORD			m_dwMaterials;

	DWORD			m_dwIntensity;	// 1X, 2X, 4X
	float			m_fSpeed;
	
	// 28 바이트는 릴리즈모드때 std::string sizeof값
//	std::string		m_strTex;
	char			m_strTex[EFF_CHAR_TEXT_SIZE];
};

struct EFFCHAR_PROPERTY_TEXDIFF : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;

	// 28 바이트는 릴리즈모드때 std::string sizeof값
//	std::string		m_strTex;
	char			m_strTex[EFF_CHAR_TEXT_SIZE];

	float			m_fSpeed;
	DWORD			m_dwMaterials;
	DWORD			m_dwIntensity;	// 1X, 2X, 4X

	EFFCHAR_PROPERTY_TEXDIFF () :
		m_dwMaterials(0L),
		m_dwIntensity(0),		// 빛의 세기.
		m_fSpeed(1.f)
	{
		m_dwFlag = USETEXTURE1 | USETEXTURE2 | USETEXTURE3 | USETEXTURE4;
		m_dwFlag |= NO_UPDOWN;
	}

	void Assign ( EFFCHAR_PROPERTY_TEXDIFF_101& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;

		//strncpy(m_strTex,sOldData.m_strTex,sizeof(m_strTex));
        StringCchCopyA(m_strTex, EFF_CHAR_TEXT_SIZE, sOldData.m_strTex);
		
		m_fSpeed = sOldData.m_fSpeed;
		m_dwMaterials = sOldData.m_dwMaterials;
		m_dwIntensity = sOldData.m_dwIntensity;
	}
};

class DxEffCharTexDiff : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_LEVEL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_TEXDIFF m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
//			std::string		m_strTex;
			// 28 바이트는 릴리즈모드때 std::string sizeof값
			//	std::string		m_strTex;
			char			m_strTex[28];

			float			m_fSpeed;
			DWORD			m_dwMaterials;
			DWORD			m_dwIntensity;


		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_TEXDIFF*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB;

protected:
	BOOL	m_bUp;
	float	m_fTime;
	float	m_fRandSpeed;
	DWORD	m_dwColor;
	float	m_fColor;

protected:
	TextureResource m_textureRes;

protected:
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DXMATERIAL_CHAR_EFF*	m_pTempMaterials;
	DWORD					m_dwTempMaterials;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

public:
	virtual void SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials );
	virtual DXMATERIAL_CHAR_EFF*		GetMaterials () { return m_pMaterials; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

	virtual DWORD GetStateOrder () const					{ return EMEFFSO_GLOW; }

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:

	// PrecalculateForMaterialSystem의 2번 중 값만 미리 백업해 놓는거라면, 이곳에서 렌더가 된다.
	virtual void PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
												EffectMaterialData* pEffMaterialData, 
												const D3DXMATRIX* pMatrix, 
												const float fScale );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh();

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행

public:
	DxEffCharTexDiff(void);
	virtual ~DxEffCharTexDiff(void);
};
