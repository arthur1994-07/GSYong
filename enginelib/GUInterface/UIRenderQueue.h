#ifndef _UI_RENDER_QUEUE_H
#define _UI_RENDER_QUEUE_H

class CUIControl;
struct UIVERTEX;

//--------------------------------------------------------------------
// UIRenderQueue
//--------------------------------------------------------------------
class CUIRenderQueue
{
public:
	static BOOL						m_bUsage;

protected:
	static CUIRenderQueue*			m_pInstance;

// 	LPDIRECT3DDEVICEQ				m_pd3dDevice;
// 	LPDIRECT3DTEXTUREQ				m_pTexture;
// 	LPDIRECT3DVERTEXBUFFERQ			m_pVertexBuffer;
// 
// 	UIVERTEX*						m_pVertex;
// 	UINT							m_uiOffset;
// 	UINT							m_uiVertex;
// 	UINT							m_uiPrimitiv;
// 	DWORD							m_dwLockFlag;

public:
//	CUIRenderQueue();
	virtual ~CUIRenderQueue() {}

	virtual VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual VOID RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual VOID InvalidateDeviceObjects() = 0;
	virtual VOID DeleteDeviceObjects() = 0;

	virtual BOOL Render( LPDIRECT3DTEXTUREQ pTexture, UIVERTEX* pVertex ) = 0;
	virtual VOID RenderImmediately() = 0;

	static VOID SetRenderQueue(CUIRenderQueue* pNewRenderQ);
	static CUIRenderQueue* Get();
	static VOID CleanUp();
};

#endif
