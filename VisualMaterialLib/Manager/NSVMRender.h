#pragma once

#include <boost/tr1/memory.hpp>

class DxTangentMesh;

//------------------------------------------------------------------
//                      NSVMRender
// Translucent ���� ���� ���߿� ������ �Ǿ�� �ؼ� �̰����� ��Ƽ� �ѹ��� �������Ѵ�.
namespace NSVMRender
{
	enum RENDER_TYPE
	{
		RT_LIGHTMAP         = 0,
		RT_COLOR            = 1,
		RT_COLOR_LIGHTMAP   = 2,
	};

    struct DATA
    {
		RENDER_TYPE			m_emLightMapMode;
        BYTE*               m_pPointer;
        DxTangentMesh*      m_pTangentMesh;
        DWORD               m_dwAttrib;
        BOOL                m_bRealPL;
        D3DXMATRIX          m_matWorld;
        LPDIRECT3DTEXTUREQ  m_pTextureDay;
        LPDIRECT3DTEXTUREQ  m_pTextureNight;
        LPDIRECT3DTEXTUREQ  m_pTextureAvgDay;
        LPDIRECT3DTEXTUREQ  m_pTextureAvgNight;
        D3DXVECTOR2         m_vLightMapUV_Offset;
        float               m_fAlpha;

        DATA( RENDER_TYPE emLightMapMode,
			BYTE* pPointer, 
            DxTangentMesh* pTangentMesh, 
            DWORD dwAttrib, 
            BOOL bRealPL, 
            const D3DXMATRIX& matWorld, 
            LPDIRECT3DTEXTUREQ pTextureDay, 
            LPDIRECT3DTEXTUREQ pTextureNight,
            LPDIRECT3DTEXTUREQ pTextureAvgDay,
            LPDIRECT3DTEXTUREQ pTextureAvgNight,
            const D3DXVECTOR2& vLightMapUV_Offset,
            float fAlpha )

            : m_emLightMapMode(emLightMapMode)
			, m_pPointer(pPointer)
            , m_pTangentMesh(pTangentMesh)
            , m_dwAttrib(dwAttrib)
            , m_bRealPL(bRealPL)
            , m_matWorld(matWorld)
            , m_pTextureDay(pTextureDay)
            , m_pTextureNight(pTextureNight)
            , m_pTextureAvgDay(pTextureAvgDay)
            , m_pTextureAvgNight(pTextureAvgNight)
            , m_vLightMapUV_Offset(vLightMapUV_Offset)
            , m_fAlpha(fAlpha)
        {
        }
    };

	class DxVMStack
	{
	public:
		void Insert( RENDER_TYPE emLightMapMode,
					BYTE* pPointer, 
					DxTangentMesh* pTangentMesh, 
					DWORD dwAttrib, 
					BOOL bRealPL, 
					const D3DXMATRIX& matWorld,
					LPDIRECT3DTEXTUREQ pTextureDay, 
					LPDIRECT3DTEXTUREQ pTextureNight,
					LPDIRECT3DTEXTUREQ pTextureAvgDay,
					LPDIRECT3DTEXTUREQ pTextureAvgNight,
					const D3DXVECTOR2& vLightMapUV_Offset,
					float fAlpha );

		void CleanUp();

		DWORD GetTranslucentNum();                  // Translucent �� �� �ִ� ����
		const DATA* GetTranslucentData( size_t i ); // Translucent Data �� �����´�.

	private:
		typedef std::vector<std::tr1::shared_ptr<DATA>>   VEC_TRANSLUCENT_DATA;
		VEC_TRANSLUCENT_DATA    g_vecTranslucentData;

	public:
		DxVMStack()	{}
		~DxVMStack() {}
	};

	extern DxVMStack g_sTranslucent;	// ������ �����͸� ����� ���´�.
	extern DxVMStack g_sStarGlow;		// StarGlow �����͸� ����� ���´�.
};
