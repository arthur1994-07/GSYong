#ifndef	ENCRYPT_DATA_DEF_H
#define ENCRYPT_DATA_DEF_H

#pragma once

enum 
{
    TEX_HEADER_SIZE = 12,
	TEX_VERSION = 0x100,
	TEX_XOR_DATA = 0x47,
	TEX_DIFF_DATA = 0x03,

	TEXTURE_DDS = 0,
	TEXTURE_TGA,
	TEXTURE_TYPE,
};

enum
{
    SKINMESH_HEADER_SIZE = 12,
	SKINMESH_VERSION = 0x100,
	SKINMESH_XOR_DATA = 0x63,
	SKINMESH_DIFF_DATA = 0x17,

	SKINMESH_XFILE = 0,
    SKINMESH_TYPE,
};

enum
{
	WIDGET_SCRIPT_HEADER_SIZE = 12,
	WIDGET_SCRIPT_VERSION = 0x100,
	WIDGET_SCRIPT_XOR_DATA = 0xAE,
	WIDGET_SCRIPT_DIFF_DATA = 0xC3,
	WIDGET_SCRIPT_DIFF2_DATA = 0xF5,
};

namespace ENCRYPT_DATA
{
	const char g_szTexFileExt[ TEXTURE_TYPE ][ MAX_PATH ] = { ".dds", ".tga" };
	const char g_szSkinFileExt[ SKINMESH_TYPE ][ MAX_PATH ] = { ".x" };
	const char g_szWidgetScriptExt[ MAX_PATH ] = _T(".lua");

	void InitKey();
    bool Encrypt( char* pBuffer, int nSize );
    bool Decrypt( char* pBuffer, int nSize );
}

#endif