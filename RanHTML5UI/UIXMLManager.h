#pragma once

#include "UICompTexListXML.h"

namespace HTML5UI
{

class CUIXMLManager
{
public:
	/// Texture Info
	struct TEXTURE_INFO
	{
		std::string		textureID;
		GUIBase::UIRECT border;
		std::string		textureName;
		float			X_Size;
		float			Y_Size;
		GUIBase::UIRECT rcTexturePos;
	};

	typedef std::vector< TEXTURE_INFO >			COMPTEX_VECTOR;
	typedef COMPTEX_VECTOR::iterator			COMPTEX_VECTOR_ITER;
	typedef COMPTEX_VECTOR::const_iterator		COMPTEX_VECTOR_CITER;

public:
	static CUIXMLManager& GetInstance();

private:
	CUIXMLManager() { }
	~CUIXMLManager() { }

public:
	std::string GetText( const TCHAR* fileName );

	inline TCHAR* GetPath() { return m_szPath; }
	inline void SetPath( TCHAR* szPath ) { StringCchCopy( m_szPath, MAX_PATH, szPath ); }

	inline COMPTEX_VECTOR GetTextureList() { return m_componentTexVector; }

public:
	void ClearTexture();

	BOOL LoadTextureList( const TCHAR* fileName );

private:
	TCHAR m_szPath[ MAX_PATH ];

	UICompTexList::SCOMPONENT_VECTOR m_vectorTextureList;

	COMPTEX_VECTOR m_componentTexVector;
};

}