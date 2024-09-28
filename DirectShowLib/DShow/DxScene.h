#pragma once

class DxScene  
{
public :
    DxScene();
    virtual ~DxScene();

private :
	float m_fScene[ 4 ][ 6 ];

public :
    void Init();

    void RenderScene( LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DTEXTURE9 pTexture,
					  float fLeft, float fTop, float fRight, float fBottom );
};

