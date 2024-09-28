#include "pch.h"

#include "MeshLoader.h"
//
//
//LPDIRECT3DDEVICE9 MeshLoader::s_pDevice = 0;
//MeshLoader::MeshLoader(void)
//{
//	m_pRootFrame = NULL;
//	m_pCurrentMeshFrame = NULL;
//	m_pVerts = NULL;
//	m_ppIndices = NULL;
//	m_pFaceMatIndices = NULL;
//	m_pMaterials = NULL;
//
//	m_bStartedLoadingMesh = false;
//
//	m_dwNumFaces = 0;
//	m_dwNumVerts = 0;
//	m_dwMaterialsWritten = 0;
//	m_dwTexturesWritten  = 0;
//}
//
//MeshLoader::~MeshLoader(void)
//{
//	if( m_pVerts )
//	{
//		delete[] m_pVerts;
//		m_pVerts = NULL;
//	}
//
//	if( m_ppIndices && (m_dwNumFaces>0) )
//	{	
//		for(unsigned int i = 0; i < m_dwNumFaces; i++)
//		{
//			delete[] m_ppIndices[i];
//		}
//		delete[] m_ppIndices;
//
//		m_ppIndices = NULL;
//	}
//
//	if( m_pMaterials )
//	{
//		delete []m_pMaterials;
//		m_pMaterials = NULL;
//	}
//
//	if( m_pRootFrame )
//	{
//		delete m_pRootFrame;
//		m_pRootFrame = NULL;
//	}
//}
//
//
//bool MeshLoader::Init(LPDIRECT3DDEVICE9 pDevice, std::string sFileFolder)
//{
//	assert( pDevice );
//
//	s_pDevice = pDevice;
//	m_sFileFolder = sFileFolder;
//
//	return true;
//}
//
//Mesh* MeshLoader::LoadMesh( string sFilename )
//{
//	string name = m_sFileFolder + "\\" + sFilename;
//
//	Parse( const_cast<char*>( name.c_str()), NULL );
//
//	return NULL;
//}
//
//BOOL MeshLoader::ParseObject(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference)
//{
//	const GUID *Type = GetObjectGUID( pDataObj );
//
//	if(*Type == TID_D3DRMFrame && Reference == FALSE )
//	{
//		ParseFrame(pDataObj, pParentDataObj, Depth, Data, Reference);
//	}
//
//	else if(*Type == TID_D3DRMFrameTransformMatrix && Reference == FALSE )
//	{
//		ParseFrameTransformationMatrix(pDataObj, pParentDataObj, Depth, Data, Reference);
//	}
//
//	else if(*Type == TID_D3DRMMesh && Reference == FALSE )
//	{
//		ParseMesh(pDataObj, pParentDataObj, Depth, Data, Reference);
//	}
//	else if(*Type == TID_D3DRMMeshNormals && Reference == FALSE )
//	{
//		ParseMeshNormals(pDataObj, pParentDataObj, Depth, Data, Reference);
//	}
//
//	else if(*Type == TID_D3DRMMeshTextureCoords && Reference == FALSE )
//	{
//		ParseMeshTextureCoords(pDataObj, pParentDataObj, Depth, Data, Reference);
//	}
//
//	else if(*Type == TID_D3DRMMeshMaterialList && Reference == FALSE )
//	{
//		ParseMeshMaterialList(pDataObj, pParentDataObj, Depth, Data, Reference);
//	}
//	else if(*Type == TID_D3DRMMaterial && Reference == FALSE )
//	{
//		ParseMeshMaterial(pDataObj, pParentDataObj, Depth, Data, Reference);
//	}
//	else if(*Type == TID_D3DRMTextureFilename && Reference == FALSE )
//	{
//		ParseMeshMaterialTextureFilename(pDataObj, pParentDataObj, Depth, Data, Reference);
//	}
//
//	return ParseChildObjects(pDataObj, Depth, Data, Reference);
//}
//
//bool MeshLoader::ConvertIndicesToTriangles()
//{
//	assert( m_ppIndices );
//	assert( m_dwNumFaces != 0 );
//
//	DWORD** ppNewIndices = NULL;
//	DWORD* pIndices;
//
//	ppNewIndices = new DWORD*[2*m_dwNumFaces];
//	::ZeroMemory( ppNewIndices, sizeof( sizeof(DWORD)*2*m_dwNumFaces));
//
//	unsigned int nCurNewFace = 0;
//	for( unsigned int i = 0; i < m_dwNumFaces; i++ )
//	{
//		pIndices = m_ppIndices[i];
//
//		if( pIndices[0] == 3 )
//		{
//			ppNewIndices[nCurNewFace] = new DWORD[3];
//
//			ppNewIndices[nCurNewFace][0] = pIndices[1];
//			ppNewIndices[nCurNewFace][1] = pIndices[2];
//			ppNewIndices[nCurNewFace][2] = pIndices[3];
//
//			nCurNewFace++;
//		}
//		else if( pIndices[0] == 4 )
//		{
//			ppNewIndices[nCurNewFace] = new DWORD[3];
//
//			ppNewIndices[nCurNewFace][0] = pIndices[1];
//			ppNewIndices[nCurNewFace][1] = pIndices[2];
//			ppNewIndices[nCurNewFace][2] = pIndices[3];
//
//			nCurNewFace++;
//
//			ppNewIndices[nCurNewFace] = new DWORD[3];
//
//			ppNewIndices[nCurNewFace][0] = pIndices[1];
//			ppNewIndices[nCurNewFace][1] = pIndices[3];
//			ppNewIndices[nCurNewFace][2] = pIndices[4];
//
//			nCurNewFace++;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	for( unsigned int i = 0; i < m_dwNumFaces; i++ )
//	{
//		delete []m_ppIndices[i];
//	}
//	delete [] m_ppIndices;
//
//	m_ppIndices = ppNewIndices;
//	m_dwNumFaces = nCurNewFace;
//
//
//
//	return true;
//	//check
//}
//
//bool MeshLoader::ConstructMesh()
//{
//	return true;
//}
//
//
//
//
//BOOL MeshLoader::ParseFrame(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void ***Data, BOOL Reference)
//{
//	DxFrame *pFrame = new DxFrame();
//
//	if( (pFrame->Name = GetObjectName( pDataObj ) ) == NULL )
//		pFrame->Name = strdup( "NoNameFrame");
//
//	//Link as root sibling
//	if( Data == NULL )
//	{
//		pFrame->pFrameSibling = m_pRootFrame;
//		m_pRootFrame = pFrame;
//
//		Data = (void**)&m_pRootFrame;
//	}
//	//Link as child of supplied frame
//	else
//	{
//		DxFrame *pParentFrame = (DxFrame*)*Data;
//		pFrame->pFrameSibling = pParentFrame->pFrameFirstChild;
//		pParentFrame->pFrameFirstChild = pFrame;
//
//		Data = (void**)&pParentFrame->pFrameFirstChild;
//	}
//
//	pFrame = NULL;
//
//	return TRUE;
//}
//BOOL MeshLoader::ParseFrameTransformationMatrix(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference)
//{
//	//get current frame
//	DxFrame *pFrame = (DxFrame*)*Data;
//
//
//	float *pDataPtr = NULL;
//	DWORD dwDataSize;
//	pDataObj->GetData( NULL, &dwDataSize, (void**)&pDataPtr);
//
//	for( unsigned int i = 0; i < 16; i++ )
//		pFrame->matOriginal[i]=*pDataPtr++;
//
//	return TRUE;
//}
//BOOL MeshLoader::ParseMesh(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference)
//{
//	//started loading new mesh, construct the current one
//	//after done, start gathering data for the new one
//	if( m_bStartedLoadingMesh )
//	{
//		bool bResult = ConstructMesh();
//		m_bStartedLoadingMesh = false;
//
//		if( m_pVerts )
//		{
//			delete[] m_pVerts;
//			m_pVerts = NULL;
//		}
//
//		if( m_ppIndices && (m_dwNumFaces>0) )
//		{	
//			for(unsigned int i = 0; i < m_dwNumFaces; i++)
//			{
//				delete[] m_ppIndices[i];
//			}
//			delete[] m_ppIndices;
//			m_ppIndices = NULL;
//		}
//
//		if( m_pMaterials )
//		{
//			delete []m_pMaterials;
//			m_pMaterials = NULL;
//		}
//		m_dwMaterialsWritten = 0;
//		m_dwTexturesWritten  = 0;
//
//		if(bResult == false )
//			return FALSE;
//	}
//
//	m_bStartedLoadingMesh = true;
//
//	m_dwNumFaces = 0;
//	m_dwNumVerts = 0;
//
//	//get current frame
//	DxFrame *pFrame = (DxFrame*)*Data;
//	m_pCurrentMeshFrame = pFrame;
//
//	pFrame->pMeshContainer = new D3DXMESHCONTAINER();
//	ID3DXMesh* pMesh = pFrame->pMeshContainer->MeshData.pMesh = NULL;
//
//	DWORD dwDataSize;
//	DWORD *pDwDataPtr = NULL;
//	float *pFDataPtr = NULL;
//	D3DXVECTOR3 *pVDataPtr = NULL;
//
//	pDataObj->GetData( NULL, &dwDataSize, (void**)&pDwDataPtr );
//
//	m_dwNumVerts = *pDwDataPtr++;
//	pFDataPtr = (float*)pDwDataPtr;
//	pDwDataPtr += (m_dwNumVerts * 3);
//
//
//	m_pVerts = new VERTEX[ m_dwNumVerts ];
//	for( unsigned int i = 0; i < m_dwNumVerts; i++ )
//	{
//
//		m_pVerts[i].x = *pFDataPtr++;
//		m_pVerts[i].y = *pFDataPtr++;
//		m_pVerts[i].z = *pFDataPtr++;
//	}
//
//	m_dwNumFaces = *pDwDataPtr++;
//	m_ppIndices = new DWORD*[m_dwNumFaces];
//
//	for( unsigned int i = 0; i < m_dwNumFaces; i++ )
//	{
//		DWORD nFaceVerts = *pDwDataPtr++;
//
//		if( (nFaceVerts != 3) && (nFaceVerts != 4 ) )
//		{
//			MessageBox(0,"MeshLoader Only accepts triangles and quads!", "MeshLoader ERROR", 0);
//			//TO-DO release
//			return FALSE;
//		}
//
//		m_ppIndices[i] = new DWORD[nFaceVerts+1];
//		m_ppIndices[i][0] = nFaceVerts;
//
//		for( unsigned int j = 0; j < nFaceVerts; j++)
//		{
//			m_ppIndices[i][j+1] = *pDwDataPtr++;
//		}
//	}	
//	return TRUE;
//}
//BOOL MeshLoader::ParseMeshNormals(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference)
//{
//	DWORD dwDataSize;
//	DWORD* pDwDataPtr = NULL;
//
//	pDataObj->GetData( NULL, &dwDataSize, (void**)&pDwDataPtr );
//
//	//skip number of normals
//	*pDwDataPtr++;
//
//	float* pFDataPtr = (float*)pDwDataPtr;
//
//	for( unsigned int i = 0; i < m_dwNumVerts; i++ )
//	{
//
//		m_pVerts[i].nx = *pFDataPtr++;
//		m_pVerts[i].ny = *pFDataPtr++;
//		m_pVerts[i].nz = *pFDataPtr++;
//	}
//
//	return TRUE;
//}
//BOOL MeshLoader::ParseMeshTextureCoords(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference)
//{
//	DWORD dwDataSize;
//	DWORD* pDwDataPtr = NULL;
//
//	pDataObj->GetData( NULL, &dwDataSize, (void**)&pDwDataPtr );
//
//	//skip number of normals
//	*pDwDataPtr++;
//
//	float* pFDataPtr = (float*)pDwDataPtr;
//
//	for( unsigned int i = 0; i < m_dwNumVerts; i++ )
//	{
//
//		m_pVerts[i].u = *pFDataPtr++;
//		m_pVerts[i].v = *pFDataPtr++;
//	}
//
//	return TRUE;
//}
//BOOL MeshLoader::ParseMeshMaterialList(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference)
//{
//	if( m_pMaterials )
//		delete []m_pMaterials;
//
//	DWORD dwDataSize;
//	DWORD* pDwDataPtr = NULL;
//
//	DWORD dwNumMaterials = 0;
//	DWORD dwNumFaces = 0;
//
//
//	pDataObj->GetData( NULL, &dwDataSize, (void**)&pDwDataPtr );
//
//	dwNumMaterials = *pDwDataPtr++;
//	dwNumFaces	   = *pDwDataPtr++;
//
//	m_pMaterials = new Material[dwNumMaterials];
//
//	if( dwNumFaces != m_dwNumFaces )
//	{
//		//TO-DO log release
//		return false;
//	}
//
//	if( m_pFaceMatIndices )
//		delete [] m_pFaceMatIndices;
//
//	m_pFaceMatIndices = new DWORD[dwNumFaces];
//
//	for( unsigned int i =0; i< dwNumFaces; i++)
//	{
//		m_pFaceMatIndices[i] = *pDwDataPtr++;
//	}
//
//	return TRUE;
//}
//
//BOOL MeshLoader::ParseMeshMaterial(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference)
//{
//	DWORD* pDwDataPtr = NULL;
//	float* pFDataPtr = NULL;
//
//	DWORD dwDataSize;
//	DWORD dwNumMaterials = 0;
//	DWORD dwNumFaces = 0;
//
//
//	pDataObj->GetData( NULL, &dwDataSize, (void**)&pDwDataPtr );
//	pFDataPtr = (float*)pDwDataPtr;
//
//	D3DMATERIAL9* pMat = m_pMaterials[m_dwMaterialsWritten].GetMaterial();
//	pMat->Ambient.r  = pMat->Diffuse.r  = *pFDataPtr++;
//	pMat->Ambient.g  = pMat->Diffuse.g  = *pFDataPtr++;
//	pMat->Ambient.b  = pMat->Diffuse.b  = *pFDataPtr++;
//	pMat->Ambient.a  = pMat->Diffuse.a  = *pFDataPtr++;
//
//	pMat->Power      = *pFDataPtr++;
//
//	pMat->Specular.r = *pFDataPtr++;
//	pMat->Specular.g = *pFDataPtr++;
//	pMat->Specular.b = *pFDataPtr++;
//
//	return TRUE;
//}
//
//
//BOOL MeshLoader::ParseMeshMaterialTextureFilename(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, DWORD Depth, void **Data, BOOL Reference)
//{
//	DWORD dwDataSize;
//	DWORD* pDwDataPtr = NULL;
//	char* pChDataPtr = NULL;
//
//	pDataObj->GetData( NULL, &dwDataSize, (void**)&pDwDataPtr );
//
//	pChDataPtr = (char*)*pDwDataPtr;
//	Material* pMat = &m_pMaterials[m_dwMaterialsWritten];
//
//	HTexture hTex;
//	hTex = g_TexManager.LoadFromFile( pChDataPtr );
//
//
//
//	pMat->SetTextureNumber( m_dwTexturesWritten+1 );
//	pMat->SetTexture( hTex, m_dwTexturesWritten );
//	m_dwTexturesWritten++;
//
//	return TRUE;
//}