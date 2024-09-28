#pragma once

//#include "stdafx.h"
#include "./XParser.h"
//#include "Animation\Direct3D.h"

//#include "Graphics\Mesh.h"

struct DxFrame;

using namespace std;
//
//class MeshLoader : public cXParser
//{
//public:
//	MeshLoader(void);
//	~MeshLoader(void);
//
//	bool Init( LPDIRECT3DDEVICE9 pDevice, string sFileFolder );
//
//	//Mesh* LoadMesh( string sFilename );
//
//protected:
//
//	static LPDIRECT3DDEVICE9 s_pDevice; 
//	string m_sFileFolder;
//
//	DxFrame *m_pRootFrame;
//
//	virtual BOOL ParseObject( IDirectXFileData *pDataObj,          
//		IDirectXFileData *pParentDataObj,    
//		DWORD Depth,                         
//		void **Data, BOOL Reference);
//
//	struct VERTEX
//	{
//		float x,y,z;
//		float nx, ny, nz;
//		float u, v;
//	};
//
//	bool m_bStartedLoadingMesh;
//	DxFrame *m_pCurrentMeshFrame;
//	VERTEX *m_pVerts;
//	DWORD **m_ppIndices;
//	DWORD  *m_pFaceMatIndices;
//	DWORD m_dwNumFaces;
//	DWORD m_dwNumVerts;
//
//	Material* m_pMaterials;
//	DWORD	  m_dwMaterialsWritten;
//	DWORD	  m_dwTexturesWritten;
//
//
//
//	bool ConvertIndicesToTriangles();
//	bool ConstructMesh();
//
//	BOOL ParseFrame(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void  **Data, BOOL Reference);
//	BOOL ParseFrameTransformationMatrix(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference);
//	BOOL ParseMesh(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference);
//	BOOL ParseMeshNormals(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference);
//	BOOL ParseMeshTextureCoords(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference);
//	BOOL ParseMeshMaterialList(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference);
//	BOOL ParseMeshMaterial(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference);
//	BOOL ParseMeshMaterialTextureFilename(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference);
//};
