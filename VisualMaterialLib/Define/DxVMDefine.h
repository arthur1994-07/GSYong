#pragma once

#include <boost/tr1/memory.hpp>
#include <vector>

enum VM_OUTPUT
{
    VM_OUTPUT_ZERO,
    VM_OUTPUT_ONE,
    VM_OUTPUT_COLOR,
};

enum
{
    DEVM_CENTER_H   = 0x01, // 가로 센터정렬
    DEVM_CENTER_V   = 0x02, // 세로 센터정렬
    DEVM_RIGHT      = 0x04, // 우측으로 붙임.
};

enum VM_CAN_SELECT_BOX
{
    CAN_SELECT_ZERO = 0,
    CAN_SELECT_INPUT = 1,
    CAN_SELECT_OUTPUT = 2,
};

struct VM_CAN_SELECT_INDEX
{
    VM_CAN_SELECT_BOX   m_emMode;
    DWORD               m_dwID;
    int                 m_nIndexBox;

    VM_CAN_SELECT_INDEX();
};

struct DxVMLineData
{
    int     m_nIndexBox_Input;
    DWORD   m_dwID_Output;
    int     m_nIndexBox_Output;
};

typedef std::vector<std::tr1::shared_ptr<DxVMLineData>>    VEC_VMLINEDATA;
typedef VEC_VMLINEDATA::iterator                        VEC_VMLINEDATA_ITER;

// Note : Add Command
enum VMC_TYPE
{
    VMC_CORE                = 0,
    VMC_TEXTURESAMPLE       = 1,
    VMC_MULTIPLY            = 2,
    VMC_SINE                = 3,
    VMC_CONSTANT            = 4,
    VMC_TIME                = 5,
    VMC_CONSTANT2VECTOR     = 6,
    VMC_CONSTANT3VECTOR     = 7,
    VMC_CONSTANT4VECTOR     = 8,
    VMC_ADD                 = 9,
    VMC_REFLECTION_VECTOR   = 10,
    VMC_CLAMP               = 11,
    VMC_SUBTRACT            = 12,
    VMC_LINEAR_INTERPOLATE  = 13,
    VMC_FRESNEL             = 14,
    VMC_PANNER              = 15,
    VMC_ROTATOR             = 16,
    VMC_TEXTURE_COORDINATE  = 17,
    VMC_VECTOR_PARAMETER    = 18,
	VMC_TEXTURE_REFRACT     = 19,
    VMC_SIZE                = 20,
};

#define VM_HERMITE_LINE_VERT 100

enum VMFX
{
    VMFX_COMMAND        = 0,
    VMFX_PIECE          = 1,
    VMFX_PIECE_COLOR    = 2,
    VMFX_SKIN_VS        = 3,
};

enum VM_BLENDMODE
{
    VM_BLENDMODE_OPAQUE         = 0,
    VM_BLENDMODE_MASKED         = 1,
    VM_BLENDMODE_TRANSLUCENT    = 2,
    VM_BLENDMODE_ADDITIVE       = 3,
    VM_BLENDMODE_MODULATE       = 4,
    VM_BLENDMODE_SIZE           = 5,
};

enum VM_TWOSIDEDMODE
{
    VM_TWOSIDEDMODE_OFF             = 0,
    VM_TWOSIDEDMODE_ON              = 1,
    VM_TWOSIDEDMODE_SEPERATE_PASS   = 2,
    VM_TWOSIDEDMODE_SIZE            = 3,
};

namespace NSVISUAL_MATERIAL
{
    extern float        TITLE_CORE_WIDTH;
    extern float        TITLE_CORE_HEIGHT;
    extern float        TITLE_COMMAND_WIDTH;
    extern float        TITLE_COMMAND_HEIGHT;
    extern float        TITLE_HEIGHT_SMALL;
    extern float        MOUSE_MOVE_VEL;
    extern TSTRING      g_strFont;
    extern DWORD        g_dwHERMITE_LINE_VERT;
    extern TSTRING      g_strVisualMaterialCommand[VMC_SIZE];
    extern TSTRING      g_strBlendMode[VM_BLENDMODE_SIZE];
    extern TSTRING      g_strTwoSideMode[VM_TWOSIDEDMODE_SIZE];

    // Edit
    extern int          g_nSelectCommandVM;
};

struct COMMENT_FX
{
    std::string m_strDiffuseFX;
    std::string m_strDiffusePowerFX;
    std::string m_strAmbientPowerFX;
    std::string m_strSpecularPowerFX;
    std::string m_strShininessPowerFX;
    std::string m_strLevelPowerFX;
    std::string m_strLevelFlowPowerFX;
    std::string m_strSpecularFX;
    std::string m_strShininessFX;
    std::string m_strEmissiveFX;
	std::string m_strStarGlowFX;
    std::string m_strOpacityFX;
    std::string m_strNormalFX;
    std::string m_strLevelFX;
    std::string m_strUserColor1FX;
    std::string m_strUserColor2FX;

    std::string     m_strWireframeFX;
    std::string     m_strAlphaREFFX;
    std::string     m_strDisableDepthTest;
    VM_BLENDMODE    m_emBlendMode;
    VM_TWOSIDEDMODE m_emTwoSidedMode;
    BOOL            m_bShininessPowerTexture;
    BOOL            m_bCharUV_MIRROR;
    BOOL            m_bUserColorOverlay;

    COMMENT_FX()
        : m_strDiffuseFX("")
        , m_strDiffusePowerFX("")
        , m_strAmbientPowerFX("")
        , m_strSpecularPowerFX("")
        , m_strShininessPowerFX("")
        , m_strLevelPowerFX("")
        , m_strLevelFlowPowerFX("")
        , m_strSpecularFX("")
        , m_strShininessFX("")
        , m_strEmissiveFX("")
		, m_strStarGlowFX("")
        , m_strOpacityFX("")
        , m_strNormalFX("")
        , m_strLevelFX("")
        , m_strUserColor1FX("")
        , m_strUserColor2FX("")

        , m_strWireframeFX("")
        , m_strAlphaREFFX("")
        , m_strDisableDepthTest("")
        , m_emBlendMode(VM_BLENDMODE_OPAQUE)
        , m_emTwoSidedMode(VM_TWOSIDEDMODE_OFF)
        , m_bShininessPowerTexture(TRUE)
        , m_bCharUV_MIRROR(FALSE)
        , m_bUserColorOverlay(FALSE)
    {
    }
};

#include <vector>
#include <set>

typedef std::vector<D3DXVECTOR3>	VEC_VECTOR3;
typedef VEC_VECTOR3::iterator		VEC_VECTOR3_ITER;

typedef std::set<DWORD>		SET_DWORD;
typedef SET_DWORD::iterator	SET_DWORD_ITER;

namespace vm
{
    struct VERTEX 
    { 
	    D3DXVECTOR3 vPos; 
	    D3DXVECTOR3 vNor; 
	    D3DXVECTOR2 vTex;
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
    };
    typedef VERTEX* PVERTEX;

    struct VERTEXNORTEX2
    { 
	    D3DXVECTOR3 vPos; 
	    D3DXVECTOR3 vNor; 
	    D3DXVECTOR2 vTex[2];
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2;
    };

    struct VERTEXNORCOLORTEX
    { 
	    D3DXVECTOR3 vPos; 
	    D3DXVECTOR3 vNor;
        DWORD       cColor;
	    D3DXVECTOR2 vTex;
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;
    };

    struct VERTEXNORCOLORTEX2
    { 
	    D3DXVECTOR3 vPos; 
	    D3DXVECTOR3 vNor;
        DWORD       cColor;
	    D3DXVECTOR2 vTex;
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2;
    };

    struct VERTEXCOLOR
    {
	    D3DXVECTOR3 vPos;
	    DWORD		dwColor;
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;
    };
    typedef VERTEXCOLOR* PVERTEXCOLOR;

    struct VERTEXNOR
    { 
	    D3DXVECTOR3 vPos; 
	    D3DXVECTOR3 vNor; 
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL;
    };
    typedef VERTEXNOR* PVERTEXNOR;

    struct POSTEX
    { 
	    D3DXVECTOR3 vPos; 
	    D3DXVECTOR2 vTex;
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_TEX1;
    };
    typedef POSTEX* PPOSTEX;

    //-------------------------------------------------------------------------------------------
    // Object
    struct VERTEXNORTEXTANGENT
    { 
	    D3DXVECTOR3 vPos;
	    D3DXVECTOR3 vNor;
	    D3DXVECTOR2 vTex;
	    D3DXVECTOR3 vTan;
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE3(1);
    };

    // SkinObject 쪽에 쓰인다. 28byte
    struct VERTEXNORTEXTANGENT_COMPRESS
    { 
	    D3DXVECTOR3 vPos;	// 12
	    short vNor[3];		// 6
	    short vTan[3];		// 6
	    short vTex[2];	    // 4
	    static const DWORD FVF = 0L;
    };

    //-------------------------------------------------------------------------------------------
    // Object - LightMap
    struct VERTEXNORTANGENTTEX2
    { 
	    D3DXVECTOR3 vPos;
	    D3DXVECTOR3 vNor;
	    D3DXVECTOR3 vTan;
        D3DXVECTOR2 vTex[2];
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE3(0);
    };

    // Object 쪽에 쓰인다. 36byte
    struct VERTEXNORTANGENTTEX2_COMPRESS
    { 
	    D3DXVECTOR3 vPos;	    // 12
	    short       vNor[3];	// 6
	    short       vTan[3];	// 6
	    D3DXVECTOR2 vTex0;	    // 8
        short       vTex1[2];	// 4
	    static const DWORD FVF = 0L;
    };

    //-------------------------------------------------------------------------------------------
    // Object - LightMap - Blend
    struct VERTEXNORCOLORTANGENTTEX2
    { 
	    D3DXVECTOR3 vPos;
	    D3DXVECTOR3 vNor;
        DWORD       cColor;
	    D3DXVECTOR3 vTan;
        D3DXVECTOR2 vTex[2];
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE3(0);
    };

    // Object 쪽에 쓰인다. 40byte
    struct VERTEXNORCOLORTANGENTTEX2_COMPRESS
    { 
	    D3DXVECTOR3 vPos;	    // 12
	    short       vNor[3];	// 6
	    short       vTan[3];	// 6
        DWORD       cColor;     // 4
	    D3DXVECTOR2 vTex0;	    // 8
        short       vTex1[2];	// 4
	    static const DWORD FVF = 0L;
    };

    //-------------------------------------------------------------------------------------------
    // Object - Blend
    struct VERTEXNORCOLORTEXTANGENT
    { 
	    D3DXVECTOR3 vPos;
	    D3DXVECTOR3 vNor;
        DWORD       cColor;
	    D3DXVECTOR2 vTex;
	    D3DXVECTOR3 vTan;
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE3(1);
    };

    // Object 쪽에 쓰인다. 36byte
    struct VERTEXNORCOLORTEXTANGENT_COMPRESS
    { 
	    D3DXVECTOR3 vPos;	    // 12
        short       vNor[3];	// 6
	    short       vTan[3];	// 6
        DWORD       cColor;     // 4
	    D3DXVECTOR2 vTex;	    // 8
	    static const DWORD FVF = 0L;
    };

    //-------------------------------------------------------------------------------------------
    // 캐릭터
    struct VERTEXNORTEXTANGENT_VS
    { 
	    D3DXVECTOR3 vPos;
	    D3DXVECTOR3 vNor;
	    D3DXVECTOR2 vTex;
	    D3DXVECTOR3 vTan;
        D3DXVECTOR4 vWeight;
        D3DXVECTOR4 vIndies;
	    static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE3(1)|D3DFVF_TEXCOORDSIZE4(2)|D3DFVF_TEXCOORDSIZE4(3);
    };

    // 캐릭터 쪽에 쓰인다. 40byte
    struct VERTEXNORTEXTANGENT_VS_COMPRESS
    { 
	    D3DXVECTOR3 vPos;	// 12
	    short vNor[3];		// 6
	    short vTan[3];		// 6
	    short vTex[2];		// 4
        short vWeight[4];   // 8
        byte vIndies[4];    // 4
	    static const DWORD FVF = 0L;
    };

    HRESULT CreateTagentMesh( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut );
    HRESULT CreateTagentColorMeshLM( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut );
    HRESULT CreateTagentSkinMeshObject( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut );
    HRESULT CreateTagentColorMesh( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut );
    HRESULT CreateTagentSkinMeshVS( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut );
};