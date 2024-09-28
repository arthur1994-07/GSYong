#pragma once
#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxMeshs/SkinMesh/DxMaterialHLSL.h"

struct EFFCHAR_PROPERTY_TS_NORMAL
{
	//////////////////////////////////////////////////////////////////////////
	// �븻�� ������
	//////////////////////////////////////////////////////////////////////////
	// �븻�� �ؽ���
	char		szNormalMap[MAX_PATH];
	// ����ŧ�� �ؽ���
	bool		bUseSpecularTex;
	char		 szSpecularTex[MAX_PATH];
	// �۷νôϽ� �ؽ���
	bool		bUseGlossinessTex;
	bool		bUseGlossNormalAlpha;
	char		szGlossinessTex[MAX_PATH];

	EFFCHAR_PROPERTY_TS_NORMAL()		
		: bUseSpecularTex(FALSE)
		, bUseGlossinessTex(FALSE)
		, bUseGlossNormalAlpha(FALSE)
	{
		ZeroMemory( szNormalMap, MAX_PATH );
		ZeroMemory( szSpecularTex, MAX_PATH );
		ZeroMemory( szGlossinessTex, MAX_PATH );
	}
};

struct EFFCHAR_PROPERTY_TS_TEXUPDOWN
{
	//////////////////////////////////////////////////////////////////////////
	// �ؽ��� ���ٿ� 
	//////////////////////////////////////////////////////////////////////////
	// �۷ο� �ؽ���
	char		szTexUpDownTex[MAX_PATH];
	// �ؽ��� ���ٿ� ����
	float		fTexUpDownBright;
	float		fTexUpDownAlpha;
	float		fTexUpDownAniSpeed;

	EFFCHAR_PROPERTY_TS_TEXUPDOWN()		
		: fTexUpDownBright(1.0f)
		, fTexUpDownAlpha(1.0f)
		, fTexUpDownAniSpeed(1.0f)

	{
		ZeroMemory( szTexUpDownTex, MAX_PATH );
	}
};

struct EFFCHAR_PROPERTY_TS_OVERLAY
{
	//////////////////////////////////////////////////////////////////////////
	// ��������
	//////////////////////////////////////////////////////////////////////////
	// �������� ����� ����� ���͸���
	DWORD		dwSelectMaterial;
	// �������� �ؽ���
	char		szOverlayTex[MAX_PATH];
	// �������� ���� ��
	D3DXCOLOR	cColor1;
	D3DXCOLOR	cColor2;
	float		fSpecularLevel;	

	EFFCHAR_PROPERTY_TS_OVERLAY()
		: dwSelectMaterial(0)
		, cColor1(D3DCOLOR_ARGB(0,128,128,128))
		, cColor2(D3DCOLOR_ARGB(0,128,128,128))
		, fSpecularLevel(20.0f)

	{
		ZeroMemory( szOverlayTex, MAX_PATH );
	}
};

struct EFFCHAR_PROPERTY_TS_REFLECTION
{
	//////////////////////////////////////////////////////////////////////////
	// �ݻ�ȿ��
	//////////////////////////////////////////////////////////////////////////
	// �ݻ�ȿ�� �ؽ���, ���� �ؽ���
	char		szReflectTex[MAX_PATH];
	char		szReflectAlphaTex[MAX_PATH];
	// �ݻ�ȿ�� ���ð�
	float		fReflectActivity;
	BOOL		bUseReflectAlpha;
	// ������ ��
	float		fFresnelPower;
	float		fFresnelScale;
	float		fFresnelBias;

	EFFCHAR_PROPERTY_TS_REFLECTION()
		: fReflectActivity(FALSE)
		, bUseReflectAlpha(FALSE)
		, fFresnelPower(1.0f)
		, fFresnelScale(1.0f)
		, fFresnelBias(1.0f)
	{
		ZeroMemory( szReflectTex, MAX_PATH );
		ZeroMemory( szReflectAlphaTex, MAX_PATH );
	}
};


struct EFFCHAR_PROPERTY_TOTALSHADER : public EFFCHAR_PROPERTY
{
	
	EFFCHAR_PROPERTY_TS_NORMAL		m_ShaderNormalMap;
	EFFCHAR_PROPERTY_TS_OVERLAY		m_ShaderOverlay;
	EFFCHAR_PROPERTY_TS_REFLECTION  m_ShaderReflection;
	EFFCHAR_PROPERTY_TS_TEXUPDOWN	m_ShaderTexUpDown;

	BOOL		m_bUseNormalMap;
	BOOL		m_bUseOverlay;
	BOOL		m_bUseReflect;
	BOOL		m_bUseTexUpDown;

	D3DXCOLOR	m_cAmbient;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cSpecular;
	float		m_fSpecularPower;

	EFFCHAR_PROPERTY_TOTALSHADER() 
		: m_bUseNormalMap(FALSE)
		, m_bUseTexUpDown(FALSE)
		, m_bUseOverlay(FALSE)
		, m_bUseReflect(FALSE)
		, m_cAmbient(D3DCOLOR_ARGB(0,128,128,128))
		, m_cDiffuse(D3DCOLOR_ARGB(0,128,128,128))
		, m_cSpecular(D3DCOLOR_ARGB(0,128,128,128))
		, m_fSpecularPower(0.0f)
	{
	}

};

class DxEffCharTotalShader : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_TOTAL_SHADER; }
	virtual const char* GetName ()	{ return NAME; }


protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_TOTALSHADER m_Property;
		};

		struct
		{
			EFFCHAR_PROPERTY_TS_NORMAL		m_ShaderNormalMap;
			EFFCHAR_PROPERTY_TS_OVERLAY		m_ShaderOverlay;
			EFFCHAR_PROPERTY_TS_REFLECTION  m_ShaderReflection;
			EFFCHAR_PROPERTY_TS_TEXUPDOWN	m_ShaderTexUpDown;

			BOOL		m_bUseNormalMap;
			BOOL		m_bUseOverlay;
			BOOL		m_bUseReflect;
			BOOL		m_bUseTexUpDown;

			D3DXCOLOR	m_cAmbient;
			D3DXCOLOR	m_cDiffuse;
			D3DXCOLOR	m_cSpecular;
			float		m_fSpecularPower;

		};
	};

protected:
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;

    TextureResource m_textureResNormal;
    TextureResource m_textureResSpecular;
    TextureResource m_textureResGlossiness;
    TextureResource m_textureResUpDown;
    TextureResource m_textureResOverlay;
    TextureResource m_textureResReflect;
    TextureResource m_textureResReflectAlpha;

	LPDIRECT3DVERTEXDECLARATION9 m_pNormalDCRT;
	ID3DXEffect*				 m_pEffect;

	D3DXVECTOR3					 m_vAddLight;
	BOOL						 m_bPossibleEffect;
	float						 m_fAniTexUpDownAlpha;
	float						 m_fAniValue;


	// ���̴� �ڵ�
	D3DXHANDLE					 m_hTechnique;
	D3DXHANDLE					 m_hvLightDir;
	D3DXHANDLE					 m_hvEyePos;
	D3DXHANDLE					 m_htNormalMap;
	D3DXHANDLE					 m_htSpecularTex;
	D3DXHANDLE					 m_htGlossinessTex;

	D3DXHANDLE					 m_hmWorld;
	D3DXHANDLE					 m_hmView;
	D3DXHANDLE					 m_hmProj;

	D3DXHANDLE					 m_htAmbient;
	D3DXHANDLE					 m_htDiffuse;
	D3DXHANDLE					 m_htSpecular;
	D3DXHANDLE					 m_hbUseDirectionLight;

	D3DXHANDLE					 m_htTexUpDownTex;
	D3DXHANDLE					 m_hfTexUpDownAlpha;

	D3DXHANDLE					 m_htOverlayTex;
	D3DXHANDLE					 m_hvInputColor1;
	D3DXHANDLE					 m_hvInputColor2;
	D3DXHANDLE					 m_hfSpecularLevel;

	D3DXHANDLE					 m_htReflectTex;
	D3DXHANDLE					 m_htReflectAlphaTex;




public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_TOTALSHADER*)pProperty);
	}

	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

	// ThreadLoading ���� ���� m_pmcMesh ������ ���� �� �־ ����ϴ� ������ �Ź� üũ�ϵ��� �Ѵ�.
private:
	BOOL IsExist_Copy_pmcMesh( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );


public:
	DxEffCharTotalShader(void);
	virtual ~DxEffCharTotalShader(void);
};
