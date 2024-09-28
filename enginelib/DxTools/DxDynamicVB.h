#pragma once

#include "../../SigmaCore/Memory/ObjectPool.h"
#include "./DxVertexFVF.h"

class DxDynamicVB
{
public:
	struct BIGVB
	{
		LPDIRECT3DVERTEXBUFFERQ pVB;

		DWORD dwFVF;
		UINT nVertexSize;
		UINT nFullByte;

		UINT iStartVertex;
		UINT nStartToLock;
		UINT nOffsetToLock;

		BYTE* pSystemMemory;

		BIGVB();
		~BIGVB();

		VOID Clear();

		VOID CreateVB( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bCreateSysMem );
		VOID ReleaseVB();

        BOOL IsValidSize( UINT uiSize );
		BOOL IsValidCopy( UINT uiSize );
		VOID* LockVB( UINT uiSize );
		UINT UnlockVB( UINT uiSize, UINT uiVertice );
		UINT CopyToVB( const VOID* buffer, UINT uiSize, UINT uiVertice );

		VOID UpdateVB();
        VOID DiscardVB();

		UINT GetFVFSize() { return nVertexSize; }
	};

public:
	static BIGVB g_sVB_PDT;
	static BIGVB g_sVB_PDT_HARD_ALPHA;
    static BIGVB g_sVB_PNT;

    /// ---------------------------------------------------------------------------
	/// 여기를 기준으로 위쪽은 모아서 찍고 아래쪽은 바로 렌더링 함
	/// ---------------------------------------------------------------------------

    static BIGVB g_sVB_PNTI;
    static BIGVB g_sVB_I_24BYTE;
    static BIGVB g_sVB_PT2I;

    enum
    {
	    PNT_NUM				= 300,
	    PDT_NUM				= 6000,
		PDT_HARD_ALPHA_NUM	= 400,
	    P_NUM				= 4000,
	    PNTI_NUM			= 3000,
	    PWDTI_NUM			= 16,
	    PDTI_NUM			= 2000,
        PT2I_NUM			= 1000,
    };

public:
    static LPDIRECT3DDEVICE9 g_pd3dDevice;
	static LPDIRECT3DINDEXBUFFER9 g_pIBuffer;

public:
	static HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT InvalidateDeviceObjects();

public:
	static void FrameMove();	// 매 m_pVB 프레임 리셋. !!
};

class OPTMBase
{
protected:
	struct RenderState
	{
		BOOL	m_bChange;
		DWORD	m_dwValue;

		void SetValue( DWORD dwValue )
		{
			m_bChange = TRUE;
			m_dwValue = dwValue;
		}

		RenderState() :
			m_bChange(FALSE),
			m_dwValue(0L)
		{
		}
	};

	struct SamplerState
	{
		BOOL	m_bChange;
		DWORD	m_dwSampler;
		DWORD	m_dwValue;

		void SetValue( DWORD dwValue )
		{
			m_bChange = TRUE;
			m_dwValue = dwValue;
		}

		SamplerState() :
			m_bChange(FALSE),
			m_dwSampler(0L),
			m_dwValue(0L)
		{
		}
	};

	struct TextureStageState
	{
		BOOL	m_bChange;
		DWORD	m_dwStage;
		DWORD	m_dwValue;

		void SetValue( DWORD dwValue )
		{
			m_bChange = TRUE;
			m_dwValue = dwValue;
		}

		TextureStageState() :
			m_bChange(FALSE),
			m_dwStage(0L),
			m_dwValue(0L)
		{
		}
	};

protected:
	static LPDIRECT3DDEVICEQ		g_pd3dDevice;

protected:
	LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	LPDIRECT3DSTATEBLOCK9	m_pDrawSB;

protected:
	inline void UserMatrix( D3DXMATRIX* pIdentity, D3DXMATRIX* pWorld, BOOL& bChange );
	inline void UserRenderState( D3DRENDERSTATETYPE dwState, RenderState& sBase, const RenderState& sRS );
	inline void UserTextureStageState( D3DTEXTURESTAGESTATETYPE dwType, TextureStageState& sBase, const TextureStageState& sTSS );

protected:
	OPTMBase(void);

public:
	~OPTMBase(void);
};

class OPTMSingleBillboard : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		DATA* pNext;

		DATA()
			: nVertexCount(0)
			, nFaceCount(0)
			, pTex(NULL)
			, pNext(NULL)
		{
		}
	};

private:
	typedef sc::ObjectPool<DATA>	DATAPOOL;
	DATAPOOL*						m_pDataPool;

	DATA*	m_pDataHead;

private:
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->getMem(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );		// Insert Data
	void ReleaseData( DATA* pData );    // 풀에서 가져온거 그냥 릴리즈함

public:
	OPTMSingleBillboard(void);
	~OPTMSingleBillboard(void);
};

class OPTMSingleSequence : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		BOOL				bChangeMatrix;
		D3DXMATRIX			matWorld;

		RenderState			sCULLMODE;
		RenderState			sDESTBLEND;
		RenderState			sZWRITEENABLE;
		RenderState			sALPHABLENDENABLE;
		TextureStageState	sCOLOROP;

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			pNext(NULL),
			bChangeMatrix(FALSE)
		{
		}
	};

private:
    typedef sc::ObjectPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	BOOL				m_bBaseMatrix;
	RenderState			m_sBaseCullMode;
	RenderState			m_sBaseDestBlend;
	RenderState			m_sBaseZWriteEnable;
	RenderState			m_sBaseAlphaBlendEnable;
	TextureStageState	m_sBaseColorOP;
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->getMem(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );		// Insert Data
    void ReleaseData( DATA* pData );    // 풀에서 가져온거 그냥 릴리즈함

public:
	OPTMSingleSequence(void);
	~OPTMSingleSequence(void);
};

class OPTMSingleGround : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		D3DXMATRIX			matWorld;
		DWORD				dwFVF;
		D3DMATERIAL9*		pMaterial;

		RenderState			sLIGHTING;
		RenderState			sDESTBLEND;
		RenderState			sZWRITEENABLE;
		RenderState			sALPHABLENDENABLE;
		RenderState			sALPHAREF;
		TextureStageState	sCOLOROP;

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			pMaterial(NULL),
			pNext(NULL)
		{
		}
	};

private:
    typedef sc::ObjectPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	RenderState			m_sBaseALPHAREF;
	RenderState			m_sBaseLighting;
	RenderState			m_sBaseDestBlend;
	RenderState			m_sBaseZWriteEnable;
	RenderState			m_sBaseAlphaBlendEnable;
	TextureStageState	m_sBaseColorOP;
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->getMem(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data
    void ReleaseData( DATA* pData );    // 풀에서 가져온거 그냥 릴리즈함

public:
	OPTMSingleGround(void);
	~OPTMSingleGround(void);
};

class OPTMSingleBlurSys : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		D3DXMATRIX*			pmatWorld;

		RenderState			sDESTBLEND;
		RenderState			sZWRITEENABLE;
		RenderState			sALPHABLENDENABLE;
		TextureStageState	sCOLOROP;

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			pmatWorld(NULL),
			pNext(NULL)
		{
		}
	};

private:
    typedef sc::ObjectPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	BOOL				m_bMatrixChange;
	RenderState			m_sBaseDestBlend;
	RenderState			m_sBaseZWriteEnable;
	RenderState			m_sBaseAlphaBlendEnable;
	TextureStageState	m_sBaseColorOP;
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->getMem(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data
    void ReleaseData( DATA* pData );    // 풀에서 가져온거 그냥 릴리즈함

public:
	OPTMSingleBlurSys(void);
	~OPTMSingleBlurSys(void);
};

class OPTMSingleParticle : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		BOOL				bChangeMatrix;
		D3DXMATRIX			matWorld;

		RenderState			sCULLMODE;
		RenderState			sSRCBLEND;
		RenderState			sDESTBLEND;
		RenderState			sZWRITEENABLE;
		RenderState			sALPHABLENDENABLE;
		TextureStageState	sCOLOROP;

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			pNext(NULL),
			bChangeMatrix(FALSE)
		{
		}
	};

private:
    typedef sc::ObjectPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	BOOL				m_bBaseMatrix;
	RenderState			m_sBaseCullMode;
	RenderState			m_sBaseDestBlend;
	RenderState			m_sBaseZWriteEnable;
	RenderState			m_sBaseAlphaBlendEnable;
	TextureStageState	m_sBaseColorOP;
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->getMem(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data
    void ReleaseData( DATA* pData );    // 풀에서 가져온거 그냥 릴리즈함

public:
	OPTMSingleParticle(void);
	~OPTMSingleParticle(void);
};

//--------------------------------------------------------------------------------------------------------------------------
// Note : DxEffCharParticle, DxEffCharBonePosEff, DxCharBoneListEff	적용
//--------------------------------------------------------------------------------------------------------------------------
class OPTMCharParticle : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류
		BOOL				bAlphaTex;		// 알파 텍스쳐 사용.

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			bAlphaTex(FALSE),
			pNext(NULL)
		{
		}
	};

private:
    typedef sc::ObjectPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->getMem(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data
    void ReleaseData( DATA* pData );    // 풀에서 가져온거 그냥 릴리즈함

public:
	OPTMCharParticle(void);
	~OPTMCharParticle(void);
};

class OPTMManager
{
public:
    LPDIRECT3DDEVICEQ   m_pd3dDevice;
	OPTMSingleParticle	m_sSingleParticle;
	OPTMSingleBillboard	m_sSingleBillboard;
	OPTMSingleSequence	m_sSingleSequence;
	OPTMSingleGround	m_sSingleGround;
	OPTMSingleBlurSys	m_sSingleBlurSys;
	OPTMCharParticle	m_sCharParticle;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

    void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render();
	void Render_HardAlpha();

protected:
	OPTMManager(void);

public:
	~OPTMManager(void);

public:
	static OPTMManager& GetInstance();
};