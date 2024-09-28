#pragma once

#include "../DxVertexFVF.h"

namespace NSTexture16Bit
{
	struct DATA
	{
		WORDWORD			m_wwColorSize;
		WORD*				m_pwColorARRAY;

		DATA()
			: m_pwColorARRAY(NULL)
		{

		}

		~DATA()
		{
			SAFE_DELETE_ARRAY( m_pwColorARRAY );
		}
	};

	DATA* Insert( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, LPDIRECT3DTEXTURE9 pTexture );
	void CleanUp();

	D3DXVECTOR4 GetColor( const NSTexture16Bit::DATA* pData, const D3DXVECTOR2& vUV );
};