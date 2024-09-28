#pragma once

class DxLandMan;

namespace NSExportXFile
{
	extern int g_nFrameCount;

	HRESULT ExportXFile( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR *szFilename, DxLandMan* pLandMan );

	HRESULT ExportDxStaticMesh_SingleTexMesh( LPDIRECTXFILESAVEOBJECT pxofsave, 
												LPDIRECTXFILEDATA pDataObjectRoot, 
												const std::vector<VERTEXNORCOLORTEX4>& vecVertices,
												const TSTRING& strFrameName,
												const TSTRING& strTexName,
												const D3DXMATRIX& matWorld );
}