#pragma once

#include <boost/tr1/memory.hpp>

#include "../Define/DxVMDefine.h"


// VisualMaterial 의 Edit 용 Box

//--------------------------------------------------------------------------------------------------------------------- DxEditVMText
struct DxEditVMText
{
    TSTRING     m_strText;
    LPD3DXMESH  m_pMesh;
    D3DXVECTOR4 m_sMeshRect;

    float GetWidth();
    float GetHeight();
    void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos, float fWidth, float fHeight, DWORD dwFlags );

    DxEditVMText();
    ~DxEditVMText();
};


//--------------------------------------------------------------------------------------------------------------------- DxEditVM
class DxEditVM
{
private:
    typedef std::vector< std::tr1::shared_ptr<DxEditVMText> >  VEC_VM_TEXT;
    typedef VEC_VM_TEXT::iterator                           VEC_VM_TEXT_ITER;

private:
    static LPDIRECT3DSTATEBLOCK9	m_pSavedBaseSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawBaseSB;
    static LPDIRECT3DSTATEBLOCK9	m_pSavedImageSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawImageSB;

private:
    LPDIRECT3DDEVICEQ   m_pd3dDevice;       //
    BOOL                m_bImage;           // Iamge 사용하는가~?
    BOOL                m_bTitle;           // 타이틀인가~?
    BOOL                m_bOutputColor;     // Output이 Color를 사용하는가~?
    BOOL                m_bSelectBaseBox;   // BaseBox 선택
    DWORD               m_dwCommandID;      // Command Index - Title이 아닐 경우 몇번째 Index 인가~?
    DWORD               m_dwColorBG;        // BackGround Color
    D3DXCOLOR           m_cColorText;       // Text Color
    VEC_VM_TEXT         m_vecChannelText;   // Channel
    VM_CAN_SELECT_INDEX m_sYellowBox;       // Yellow Box 가 그려지는 곳.
    VEC_VECTOR3         m_vecInputPoint;    // Input Point
    VEC_VECTOR3         m_vecOutputPoint;   // Output Point
    D3DXVECTOR2         m_vMin;             // 박스 사이즈 Min
    D3DXVECTOR2         m_vMax;             // 박스 사이즈 Max

public:
	static void OnResetDeviceSTATIC( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnLostDeviceSTATIC();

    // Base
public:
    void Create( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTitle );

    void AddChannel( const TSTRING& strTitleName );
    void AddImage();
    void AddOutputColor();

    void ChangeChannel( const TSTRING& strTitleName );

    void SetYellowBox( VM_CAN_SELECT_INDEX& sCanSelectIndex );
    void SetSelectBaseBox( BOOL bSelect );

    void Render( DWORD dwID, 
                VM_CAN_SELECT_INDEX& sCanSelectIndex,
                const D3DXVECTOR2& vPos, 
                float fWidth, 
                float fHeight, 
                DWORD dwFlags, 
                const D3DXVECTOR2& sMousePt,
                ID3DXEffect* pFX );

    float GetCalculateWidth( float fWidth );
    float GetCalculateHeight( float fHeight );

    void GetPointInput( D3DXVECTOR3& vPoint, int nIndexBox );
    void GetPointOutput( D3DXVECTOR3& vPoint, int nIndexBox );

    BOOL IsSelectAtBox( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax );

private:
    HRESULT CreateD3DXTextMesh( IDirect3DDevice9* pd3dDevice, 
                                LPD3DXMESH* ppMesh, 
                                const TCHAR* pName,
                                DWORD dwSize,
                                BOOL bBold, 
                                BOOL bItalic );

    // RenderBox & Line
    void CalMeshRect( LPD3DXMESH pMesh, D3DXVECTOR4& sMeshRect );

    // RenderBox & Line
    void RenderBaseBox( const D3DXVECTOR2& vPos, float fWidth, float fHeight );
    void RenderLine( const D3DXVECTOR2& vPos, float fWidth, float fHeight );

    // InputBox
    void ActionInputBox( VM_CAN_SELECT_INDEX& sCanSelectIndex, 
                        DWORD dwID,  
                        const D3DXVECTOR2& vPos, 
                        float fWidth, 
                        const D3DXVECTOR2& sMousePt );

    void CreateInputBox( vm::VERTEXCOLOR vBoxPos[], const D3DXVECTOR2& vPos, float fWidth, DWORD dwIndex );

    void RenderInputBox( vm::VERTEXCOLOR vBoxPos[] );


    // OutputBox
    void ActionOutputColor( VM_CAN_SELECT_INDEX& sCanSelectIndex, 
                            DWORD dwID,  
                            const D3DXVECTOR2& vPos, 
                            const D3DXVECTOR2& sMousePt );

    void ActionOutputOne( VM_CAN_SELECT_INDEX& sCanSelectIndex, 
                            DWORD dwID,  
                            const D3DXVECTOR2& vPos, 
                            const D3DXVECTOR2& sMousePt );

    void RenderOutputBox( vm::VERTEXCOLOR vBoxPos[], const D3DXVECTOR2& vPos, DWORD dwColor, DWORD dwIndex );


    // RenderImage
    void RenderImage( const D3DXVECTOR2& vPos, ID3DXEffect* pFX );


    // SetCanSelectBox
    void SetCanSelectBox( VM_CAN_SELECT_INDEX& sCanSelectIndex, 
                        DWORD dwID, 
                        vm::VERTEXCOLOR vBoxPos[], 
                        const D3DXVECTOR2& sMousePt, 
                        int nIndex,
                        VM_CAN_SELECT_BOX emMode );


    // Mouse Action
public:
    BOOL IsMouseCanSelect( const D3DXVECTOR3& vTargetPt, const D3DXVECTOR3& vFromPt );

private:
    BOOL IsMouseCanSelect_InputBox( const D3DXVECTOR3& vTargetPt, const D3DXVECTOR3& vFromPt );
    BOOL IsMouseCanSelect_OutputBox( const D3DXVECTOR3& vTargetPt, const D3DXVECTOR3& vFromPt );

public:
	DxEditVM(void);
	~DxEditVM(void);
};