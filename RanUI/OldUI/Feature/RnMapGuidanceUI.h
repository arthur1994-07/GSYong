#pragma	once

#include "../../../enginelib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;
class RnOldLargeMapWindow;
class CSwapImage;

class RnMapGuidanceUI : public CUIGroup
{
    struct GuidanceMarkInfo
    {
        D3DXVECTOR3 vMarkPos;
        CSwapImage* pMarkImage;
    };
    typedef std::vector<GuidanceMarkInfo> GUIDANCE_MARK_INFO_VECTOR;
    typedef GUIDANCE_MARK_INFO_VECTOR::iterator GUIDANCE_MARK_INFO_VECTOR_ITER;

public:
    RnMapGuidanceUI(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice, RnOldLargeMapWindow* pOldLargeMap);
    virtual	~RnMapGuidanceUI();

    void SetGuidanceAuthInfo( const NET_MSG_GENERIC* pMessage );
	void SetCTIAuthInfo ( const NET_MSG_GENERIC* pMsg );

    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
    virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice ) override;
protected:
    DWORD dwGuidanceMapID;
    GUIDANCE_MARK_INFO_VECTOR m_vecGuidanceMarkInfo;

private:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;
    RnOldLargeMapWindow* m_pOldLargeMap;
};