#pragma once

enum
{
	// [2014.09.11][shhan] 모든 밉맵체인을 다 만든다. Anisotropic Filtering 을 사용하므로 최대로 한다.
	//
	// 캐릭터 관련 텍스쳐는 밉맵을 3으로 한다.
	//
	EMMM_CHAR_MIPMAP = 0,
};

enum TEXTURE_ALPHA
{
	EMTT_NORMAL,
	EMTT_ALPHA_HARD,
	EMTT_ALPHA_SOFT,
	EMTT_ALPHA_SOFT01,
	EMTT_ALPHA_SOFT02,
};

enum TEXTURE_TYPE_FLAG
{
	EMTTF_OPAQUE		= 0x01,
	EMTTF_ALPHA_HARD	= 0x02,
	EMTTF_ALPHA_SOFT	= 0x04,

	EMTTF_FRAMEANI_OPAQUE		= 0x10,
	EMTTF_FRAMEANI_ALPHA_HARD	= 0x20,
	EMTTF_FRAMEANI_ALPHA_SOFT	= 0x40,

	EMTTF_DECAL_BEFORE	= 0x100,
	EMTTF_DECAL_AFTER	= 0x200,

	EMTTF_UNKNOWN		= 0x1000,
};

enum TEXTURE_RESIZE
{
    TEXTURE_RESIZE_NONE = 0,    // 사이즈를 축소하지 않는다
	TEXTURE_RESIZE_256 = 256,	// 256 보다 큰 경우 256 사이즈로 축소
	TEXTURE_RESIZE_512 = 512,	// 512 보다 큰 경우 512 사이즈로 축소
	TEXTURE_RESIZE_1024 = 1024,	// 1024 보다 큰 경우 1024 사이즈로 축소
};

enum TEXTURE_CATEGORY
{
    TEXTURE_SKYBOX,
    TEXTURE_CHAR,
    TEXTURE_EFFECT,
    TEXTURE_LAND,
    TEXTURE_SIMPLEMESH,
    TEXTURE_ETC,
    TEXTURE_VISUALMATERIAL,
    TEXTURE_UI,
	TEXTURE_MATERIALS,

    TEXTURE_CATEGORY_SIZE
};

struct TEXTURE_ADD_DATA
{
	enum TYPE
    {
        TEXUTRE_TYPE_2D,
        TEXUTRE_TYPE_CUBE
    };

	TEXTURE_ADD_DATA(
        TYPE type = TEXUTRE_TYPE_2D,
        DWORD mipMap = 0,
		D3DFORMAT format = D3DFMT_UNKNOWN,
        TEXTURE_RESIZE resizeType = TEXTURE_RESIZE_NONE,
        TEXTURE_CATEGORY emCategory = TEXTURE_ETC,
		BOOL bBackUpCache = FALSE )
	{
		m_type = type;
		m_mipMap = mipMap;
		m_d3dFormat = format;
		m_resizeType = resizeType;
        m_emCategory = emCategory;
		m_bBackUpCache = bBackUpCache;
	}

	bool operator == ( const TEXTURE_ADD_DATA& other ) const
	{
		return m_type == other.m_type &&
            m_mipMap == other.m_mipMap &&
            m_d3dFormat == other.m_d3dFormat &&
            m_resizeType == other.m_resizeType &&
			m_bBackUpCache == other.m_bBackUpCache;
	}

	bool operator != ( const TEXTURE_ADD_DATA& other ) const
    {
        return !operator == ( other );
    }

	TYPE m_type;
	DWORD m_mipMap;
	D3DFORMAT m_d3dFormat;
	TEXTURE_RESIZE m_resizeType;
    TEXTURE_CATEGORY m_emCategory;

	BOOL			m_bBackUpCache;	//해진씨가 기능 만들어 줄때까지 임시로 쓰도록 하자.
};