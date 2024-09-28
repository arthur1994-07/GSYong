#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"
#include "../../DxCommon/xrmxftmpl.h"
#include "../../DxEffect/Mesh/DxStaticMeshLightMap.h"
#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../NaviMesh/navigationmesh.h"
#include "../../DxTools/Material/DxStaticMaterialMeshLightMap.h"
#include "../DxPiece/DxSingleTexMesh.h"
#include "../DxPiece/DxPieceManagerWLD.h"
#include "../DxLandMan.h"

#include "NSExportXFile.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Max Export 가 아닌, WorldEdit 데이터를 X-File 로 만들기 위한 것.
namespace NSExportXFile
{
	int g_nFrameCount(0);
	std::string strEmptyName("");

#define POWER_DEFAULT   0.0

#define XSKINEXP_TEMPLATES_MAX \
	"xof 0303txt 0032\
	template XSkinMeshHeader \
	{ \
	<3CF169CE-FF7C-44ab-93C0-F78F62D172E2> \
	WORD nMaxSkinWeightsPerVertex; \
	WORD nMaxSkinWeightsPerFace; \
	WORD nBones; \
	} \
	template VertexDuplicationIndices \
	{ \
	<B8D65549-D7C9-4995-89CF-53A9A8B031E3> \
	DWORD nIndices; \
	DWORD nOriginalVertices; \
	array DWORD indices[nIndices]; \
	} \
	template SkinWeights \
	{ \
	<6F0D123B-BAD2-4167-A0D0-80224F25FABB> \
	STRING transformNodeName;\
	DWORD nWeights; \
	array DWORD vertexIndices[nWeights]; \
	array float weights[nWeights]; \
	Matrix4x4 matrixOffset; \
	} \
	template Patch \
	{ \
	<A3EB5D44-FC22-429D-9AFB-3221CB9719A6> \
	DWORD nControlIndices; \
	array DWORD controlIndices[nControlIndices]; \
	} \
	template PatchMesh \
	{ \
	<D02C95CC-EDBA-4305-9B5D-1820D7704BBF> \
	DWORD nVertices; \
	array Vector vertices[nVertices]; \
	DWORD nPatches; \
	array Patch patches[nPatches]; \
	[...] \
	} "

	// Macros for saving data to memory at DWORD* pbCur (this pointer is incremented)
#define WRITE_PTCHAR(pbCur, ptchar) {TCHAR** __pptchar = (TCHAR**)pbCur; *(__pptchar++) = ptchar;\
	pbCur = (PBYTE)__pptchar;}

#define WRITE_STRING(pbCur, pstring) {TCHAR* __pCurrDestChar = (TCHAR*)pbCur; TCHAR* __pCurrOrgChar = pstring;\
	while(NULL != *__pCurrOrgChar) { *(__pCurrDestChar++) = *(__pCurrOrgChar++); }\
	*(__pCurrDestChar++) = _T('\0'); pbCur = __pCurrDestChar;}\

#define WRITE_WORD(pbCur, word) {WORD* __pword = (WORD*)pbCur; *(__pword++) = word;\
	pbCur = (PBYTE)__pword;}

#define WRITE_DWORD(pbCur, dword) {DWORD* __pdword = (DWORD*)pbCur; *(__pdword++) = dword;\
	pbCur = (PBYTE)__pdword;}

#define WRITE_FLOAT(pbCur, _float) {float* __pfloat = (float*)pbCur; *(__pfloat++) = _float;\
	pbCur = (PBYTE)__pfloat;}

#define WRITE_POINT3(pbCur, _point3) {Point3 _temp = (Point3)_point3; float __tempVal;\
	__tempVal = _temp[0]; WRITE_FLOAT(pbCur, __tempVal);\
	__tempVal = _temp[1]; WRITE_FLOAT(pbCur, __tempVal);\
	__tempVal = _temp[2]; WRITE_FLOAT(pbCur, __tempVal);}

#define WRITE_COLOR(pbCur, _color) {D3DXCOLOR _temp = (D3DXCOLOR)_color; float __tempVal;\
	__tempVal = _temp.r; WRITE_FLOAT(pbCur, __tempVal);\
	__tempVal = _temp.g; WRITE_FLOAT(pbCur, __tempVal);\
	__tempVal = _temp.b; WRITE_FLOAT(pbCur, __tempVal);\
	__tempVal = _temp.a; WRITE_FLOAT(pbCur, __tempVal);}

#define WRITE_MATRIX4_FROM_MATRIX3(pbCur, _matrix3) { \
	Point3 __tempRow = ((Matrix3)_matrix3).GetRow(0); WRITE_POINT3(pbCur, __tempRow); WRITE_FLOAT(pbCur, 0);\
						__tempRow = _matrix3.GetRow(1); WRITE_POINT3(pbCur, __tempRow); WRITE_FLOAT(pbCur, 0);\
						__tempRow = _matrix3.GetRow(2); WRITE_POINT3(pbCur, __tempRow); WRITE_FLOAT(pbCur, 0);\
						__tempRow = _matrix3.GetRow(3); WRITE_POINT3(pbCur, __tempRow); WRITE_FLOAT(pbCur, 1);}

#define WRITE_MATRIX(pbCur, mat) { *(D3DXMATRIX*)pbCur = mat;\
	pbCur = (PBYTE)pbCur + sizeof(D3DXMATRIX);}

	void WRITE_VECTOR3( BYTE** ppbCur, const D3DXVECTOR3& vPoint )
	{
		WRITE_FLOAT( *ppbCur, vPoint.x );
		WRITE_FLOAT( *ppbCur, vPoint.y );
		WRITE_FLOAT( *ppbCur, vPoint.z );
	};
	
	const GUID* aIds[] = {&DXFILEOBJ_XSkinMeshHeader,
		&DXFILEOBJ_VertexDuplicationIndices,
		&DXFILEOBJ_SkinWeights};

	const int x_cbStringBufferMax = 4088;

	struct SStringBlock
	{
		SStringBlock* m_psbNext;
		DWORD m_cbData;

		TCHAR szData[ x_cbStringBufferMax ];

		SStringBlock()
			: m_psbNext( NULL )
			, m_cbData( 0 ) 
		{
		}

		~SStringBlock()
		{
			delete m_psbNext;
		}
	};

	class CStringTable
	{
	private:
		SStringBlock* m_psbHead;

	public:
		CStringTable()
			: m_psbHead( NULL )
		{
		}

		~CStringTable()
		{
			delete m_psbHead;
		}

		// allocate a string out of the data blocks to be free'd later, and make it a valid
		//   x-file name at the same time
		TCHAR *CreateNiceString(TCHAR *szString)
		{
			TCHAR* szNewString = NULL;
			BOOL bFirstCharIsDigit;
			DWORD cbLength;
			SStringBlock *psbNew;

			if (szString == NULL)
				return NULL;

			cbLength = (DWORD)_tcslen(szString) + 1;

			bFirstCharIsDigit = _istdigit(*szString);
			if (bFirstCharIsDigit)
			{
				cbLength += 1;
			}

			// if no string blocks or the current doesn't have enough space, then allocate one
			if ((m_psbHead == NULL) || ((x_cbStringBufferMax - m_psbHead->m_cbData) < cbLength))
			{
				psbNew = new SStringBlock();
				if (psbNew == NULL)
					return NULL;

				psbNew->m_psbNext = m_psbHead;
				m_psbHead = psbNew;
			}

			// allocate a string out of the data block
			szNewString = m_psbHead->szData + m_psbHead->m_cbData;
			m_psbHead->m_cbData += cbLength;

			// deal with the fact that the string can't start with digits
			*szNewString = _T('\0');
			if( bFirstCharIsDigit ) 
			{
				_tcscat(szNewString, _T("_"));
			}

			_tcscat(szNewString, szString);

			TCHAR* pchCur = szNewString;
			while( NULL != *pchCur )
			{
				if( *pchCur != _T('_') && !_istalnum(*pchCur) )
				{
					*pchCur = _T('_');
				}
				pchCur++;
			}
			return szNewString;
		}

		// Allocate a new string with '\\' in place of '\' characters
		TCHAR* CreateNiceFilename(TCHAR *szString)
		{
			TCHAR* szNewString = NULL;
			DWORD cbNameLength;
			DWORD cbLength;
			TCHAR* pchCur;
			TCHAR* pchOrig;
			SStringBlock *psbNew;

			if( NULL == szString )
			{
				return NULL;
			}

			cbNameLength = (DWORD)_tcslen(szString);
			cbLength = cbNameLength*2 + 1;

			// if no string blocks or the current doesn't have enough space, then allocate one
			if ((m_psbHead == NULL) || ((x_cbStringBufferMax - m_psbHead->m_cbData) < cbLength))
			{
				psbNew = new SStringBlock();
				if (psbNew == NULL)
					return NULL;

				psbNew->m_psbNext = m_psbHead;
				m_psbHead = psbNew;
			}

			// allocate a string out of the data block
			szNewString = m_psbHead->szData + m_psbHead->m_cbData;
			m_psbHead->m_cbData += cbLength;

			pchCur = szNewString;
			pchOrig = szString;
			while (NULL != *pchOrig)
			{
				if( _T('\\') == *pchOrig )
				{
					*(pchCur++) = _T('\\');
					*(pchCur++) = _T('\\');
				}
				else
				{
					*(pchCur++) = *pchOrig;
				}
				pchOrig++;
			}
			*pchCur = _T('\0');

			return szNewString;
		}

		// Allocate a new string without fiddling with the '\' characters
		TCHAR* CreateNormalFilename(TCHAR *szString)
		{
			TCHAR* szNewString = NULL;
			DWORD cbNameLength;
			DWORD cbLength;
			SStringBlock *psbNew;

			if( NULL == szString )
			{
				return NULL;
			}

			cbNameLength = (DWORD)_tcslen(szString);
			cbLength = cbNameLength + 1;

			// if no string blocks or the current doesn't have enough space, then allocate one
			if ((m_psbHead == NULL) || ((x_cbStringBufferMax - m_psbHead->m_cbData) < cbLength))
			{
				psbNew = new SStringBlock();
				if (psbNew == NULL)
					return NULL;

				psbNew->m_psbNext = m_psbHead;
				m_psbHead = psbNew;
			}

			// allocate a string out of the data block
			szNewString = m_psbHead->szData + m_psbHead->m_cbData;
			m_psbHead->m_cbData += cbLength;

			memcpy(szNewString, szString, cbLength);

			return szNewString;
		}
	};

	HRESULT AddTransform( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pParent, const D3DXMATRIX& matWorld )
	{
		HRESULT hr = S_OK;
		PBYTE pbData = NULL;
		PBYTE pbCur;
		DWORD cbSize;
		LPDIRECTXFILEDATA pDataObject = NULL;

		cbSize = 16*sizeof(float);

		pbCur = pbData = new BYTE[cbSize];
		if (pbData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		WRITE_MATRIX( pbCur, matWorld );


		hr = pxofsave->CreateDataObject(TID_D3DRMFrameTransformMatrix,
			NULL,
			NULL,
			cbSize,
			pbData,
			&pDataObject
			);
		if (FAILED(hr))
			goto e_Exit;

		hr = pParent->AddDataObject(pDataObject);
		if (FAILED(hr))
			goto e_Exit;

e_Exit:
		delete []pbData;
		SAFE_RELEASE(pDataObject);

		return hr;
	}

	HRESULT AddNormals( LPDIRECTXFILESAVEOBJECT pxofsave, const std::vector<VERTEXNORCOLORTEX4>& vecVertices, LPDIRECTXFILEDATA pParent )
	{
		HRESULT hr = S_OK;
		LPDIRECTXFILEDATA pDataObject = NULL;
		PBYTE         pbData = NULL;
		PBYTE         pbCur;        
		DWORD          cbSize;
		DWORD cNormals;
		DWORD cFaces;
		DWORD iFace;
		DWORD iVertex;

		assert(pxofsave != NULL);
		assert(pParent != NULL);

		cNormals = static_cast<DWORD> (vecVertices.size());
		cFaces = static_cast<DWORD> (vecVertices.size()/3);

		cbSize = sizeof(DWORD) // nNormals
			+ 3*sizeof(float)*cNormals // normals
			+ sizeof(DWORD) // nFaces
			+ cFaces* // MeshFace array
			(sizeof(DWORD) //nFaceVertexIndices (number of normal indices)
			+ 3*sizeof(DWORD)); // faceVertexIndices (normal indices)

		pbCur = pbData = new BYTE[cbSize];
		if (pbData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// nNormals
		WRITE_DWORD(pbCur, cNormals);

		// normals
		for (iVertex = 0; iVertex < vecVertices.size(); iVertex++)
		{
			WRITE_FLOAT( pbCur, vecVertices[ iVertex ].vNor.x );
			WRITE_FLOAT( pbCur, vecVertices[ iVertex ].vNor.y );
			WRITE_FLOAT( pbCur, vecVertices[ iVertex ].vNor.z );

			//WRITE_VECTOR3( &pbCur, vecVertices[ iVertex ].vNor );
		}

		// nFaces
		WRITE_DWORD(pbCur, cFaces);


		// MeshFace array
		for( iFace = 0; iFace < cFaces; iFace++ )
		{
			WRITE_DWORD(pbCur, 3); // nFaceVertexIndices (number of normal indices)

			WRITE_DWORD(pbCur, iFace*3+0);
			WRITE_DWORD(pbCur, iFace*3+1);
			WRITE_DWORD(pbCur, iFace*3+2);
		}

		hr = pxofsave->CreateDataObject(TID_D3DRMMeshNormals,
			NULL,
			NULL,
			cbSize,
			pbData,
			&pDataObject
			);
		if (FAILED(hr))
		{
			OutputDebugString("Failed to create x file data object!");
			goto e_Exit;
		}

		hr = pParent->AddDataObject(pDataObject);
		if (FAILED(hr))
		{
			OutputDebugString("Failed to add x file data object!");
			goto e_Exit;
		}

		// falling through
e_Exit:
		SAFE_RELEASE(pDataObject);
		delete []pbData;

		return hr;
	}

	HRESULT AddTextureCoordinates( LPDIRECTXFILESAVEOBJECT pxofsave, const std::vector<VERTEXNORCOLORTEX4>& vecVertices, LPDIRECTXFILEDATA pParent )
	{
		LPDIRECTXFILEDATA pDataObject = NULL;
		PBYTE         pbData = NULL;
		PBYTE         pbCur = NULL;
		DWORD         cbSize;
		DWORD         cTexCoords;
		DWORD         iVertex;
		HRESULT       hr = S_OK;

		assert( pxofsave );
		assert( pParent );

		cTexCoords = static_cast<DWORD> (vecVertices.size());

		cbSize = sizeof(DWORD) //nTextureCoords
			+ cTexCoords*2*sizeof(float); //texture coords

		pbCur = pbData = new BYTE[cbSize];
		if (pbData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		WRITE_DWORD(pbCur, cTexCoords); //nTextureCoords

		for (iVertex = 0; iVertex < vecVertices.size(); iVertex++)
		{
			WRITE_FLOAT(pbCur, vecVertices[iVertex].vTex[0].x); //u
			WRITE_FLOAT(pbCur, vecVertices[iVertex].vTex[0].y); //v
		}

		hr = pxofsave->CreateDataObject(TID_D3DRMMeshTextureCoords,
			NULL,
			NULL,
			cbSize,
			pbData,
			&pDataObject
			);
		if (FAILED(hr))
		{
			OutputDebugString("Failed to create x file data object!");
			goto e_Exit;
		}

		hr = pParent->AddDataObject(pDataObject);
		if (FAILED(hr))
		{
			OutputDebugString("Failed to add x file data object!");
			goto e_Exit;
		}

		// falling through
e_Exit:
		SAFE_RELEASE(pDataObject);

		delete []pbData;
		return hr;
	}

	HRESULT AddTextureFilename( LPDIRECTXFILESAVEOBJECT pxofsave, TCHAR *szName, LPDIRECTXFILEDATA pParent )
	{
		HRESULT hr = S_OK;
		LPDIRECTXFILEDATA pDataObject = NULL;
		DWORD cbSize;

		cbSize = sizeof(TCHAR**);

		hr = pxofsave->CreateDataObject(TID_D3DRMTextureFilename,
			NULL,
			NULL,
			cbSize,
			&szName,
			&pDataObject
			);
		if (FAILED(hr))
			goto e_Exit;

		hr = pParent->AddDataObject(pDataObject);
		if (FAILED(hr))
			goto e_Exit;

e_Exit:
		SAFE_RELEASE(pDataObject);

		return hr;
	}

	HRESULT AddMaterial( LPDIRECTXFILESAVEOBJECT pxofsave, const TSTRING& strName, LPDIRECTXFILEDATA pParent )
	{
		HRESULT hr = S_OK;
		LPDIRECTXFILEDATA pDataObject = NULL;
		PBYTE pbData = NULL;
		PBYTE pbCur;
		DWORD cbSize;

		cbSize = 4*sizeof(float) // colorRGBA
				 + sizeof(float) //power
				 + 3*sizeof(float) //specularColor
				 + 3*sizeof(float); //emissiveColor

		pbData = pbCur = new BYTE[cbSize];
		if (pbCur == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// 'Ambient color' is specified in Max docs as "the color of the object in shadows," and 
		//                 is usually a darker version of the diffuse color.
		// 'Diffuse color' is specified in the Max docs as "the color of the object in 
		//                  good lighting," usually referred to as the objects' color.
		// 'Specular color' is specified as the color of reflection highlights in direct lighting,
		//                  and according to Max docs is usually the same as diffuse.

		// diffuse - write white RGBA
		WRITE_FLOAT(pbCur, 1.0f);
		WRITE_FLOAT(pbCur, 1.0f);
		WRITE_FLOAT(pbCur, 1.0f);
		WRITE_FLOAT(pbCur, 1.0f);

		// specular power
		WRITE_FLOAT(pbCur, POWER_DEFAULT);

		// specular - write white RGBA
		WRITE_FLOAT(pbCur, 1.0f);
		WRITE_FLOAT(pbCur, 1.0f);
		WRITE_FLOAT(pbCur, 1.0f);

		// emmissive - write white RGBA
		WRITE_FLOAT(pbCur, 0.0f);
		WRITE_FLOAT(pbCur, 0.0f);
		WRITE_FLOAT(pbCur, 0.0f);

		hr = pxofsave->CreateDataObject(TID_D3DRMMaterial,
												NULL,
												NULL,
												cbSize,
												pbData,
												&pDataObject
												);
		if (FAILED(hr))
			goto e_Exit;

		// See if there is a valid bitmap texture
		hr = AddTextureFilename( pxofsave, const_cast<TCHAR*>( strName.c_str() ), pDataObject);
		if (FAILED(hr))
			goto e_Exit;

		hr = pParent->AddDataObject(pDataObject);
		if (FAILED(hr))
			goto e_Exit;

	e_Exit:
		delete []pbData;
		SAFE_RELEASE(pDataObject);

		return hr;
	}

	HRESULT AddMeshMaterials( LPDIRECTXFILESAVEOBJECT pxofsave,
								const std::vector<VERTEXNORCOLORTEX4>& vecVertices,
								const TSTRING& strName,
								LPDIRECTXFILEDATA pParent )
	{
		HRESULT hr = S_OK;
		LPDIRECTXFILEDATA pDataObject = NULL;
		DWORD iFace;
		DWORD cFaces;
		PBYTE pbCur;
		PBYTE pbData = NULL;
		DWORD cbSize;
		DWORD iCurMaterial;
		DWORD cUniqueMaterials;

		cFaces = static_cast<DWORD> (vecVertices.size()/3);

		// first go through and remove redundant materials (might not have had the same crop info!)
		cUniqueMaterials = 1;

		cbSize = sizeof(DWORD) // nMaterials
			+ sizeof(DWORD) // nFaceIndexes
			+ cFaces*sizeof(DWORD); // face indexes

		pbData = pbCur = new BYTE[cbSize];
		if (pbCur == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		WRITE_DWORD(pbCur, cUniqueMaterials);
		WRITE_DWORD(pbCur, cFaces);


		// For each face, output the index of the material which applies to it, 
		// starting from  0

		for (iFace=0; iFace < cFaces; iFace++)
		{
			// don't forget to remap the matrerial before writing it to the file
			WRITE_DWORD(pbCur, 0);
		} 

		// now finally create the mesh material list
		hr = pxofsave->CreateDataObject(TID_D3DRMMeshMaterialList,
			NULL,
			NULL,
			cbSize,
			pbData,
			&pDataObject
			);
		if (FAILED(hr))
			goto e_Exit;

		hr = pParent->AddDataObject(pDataObject);
		if (FAILED(hr))
			goto e_Exit;

		for (iCurMaterial = 0; iCurMaterial < cUniqueMaterials; iCurMaterial++)
		{
			hr = AddMaterial( pxofsave, strName, pDataObject);
			if (FAILED(hr))
				goto e_Exit;
		} 

e_Exit:
		delete []pbData;
		SAFE_RELEASE(pDataObject);


		return hr;
	} 

	HRESULT AddMesh( LPDIRECTXFILESAVEOBJECT pxofsave, const TSTRING& strName, const std::vector<VERTEXNORCOLORTEX4>& vecVertices, LPDIRECTXFILEDATA pParent )
	{
		HRESULT hr = S_OK;
		BOOL bDeleteTriObject = false;
		//TriObject *pTriObject = NULL;
		//Mesh *pMesh;
		//BOOL bSwapTriOrder;
		PBYTE pbData = NULL;
		PBYTE pbCur;
		DWORD cbSize;
		DWORD cVertices;
		DWORD cFaces;
		DWORD iFace;
		//Matrix3 matNodeTM;
		//SMeshData MeshData;
		LPDIRECTXFILEDATA pDataObject = NULL;
		DWORD iVertex;
		//DWORD *rgdwMeshMaterials = NULL;
		//SCropInfo *rgCropInfo = NULL;
		//D3DXMATERIAL *rgMaterials = NULL;

		cVertices = static_cast<DWORD> (vecVertices.size());
		cFaces = static_cast<DWORD> (vecVertices.size()/3);
		cbSize = sizeof(DWORD) // nVertices
			+ cVertices*sizeof(float)*3 // vertices
			+ sizeof(DWORD) // nFaces
			+ cFaces*(sizeof(DWORD) /*nFaceVertexIndices*/ 
					+ sizeof(DWORD)*3 /*faceVertexIndices*/); // faces

		pbCur = pbData = new BYTE[cbSize];
		if (pbData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// write nVertices
		WRITE_DWORD(pbCur, cVertices);

		// write vertices
		for ( iVertex = 0; iVertex < vecVertices.size(); iVertex++)
		{
			WRITE_FLOAT( pbCur, vecVertices[ iVertex ].vPos.x );
			WRITE_FLOAT( pbCur, vecVertices[ iVertex ].vPos.y );
			WRITE_FLOAT( pbCur, vecVertices[ iVertex ].vPos.z );

			//WRITE_VECTOR3( &pbCur, vecVertices[ iVertex ].vPos );
		}

		// write nFaces
		WRITE_DWORD(pbCur, cFaces);

		// write faces
		for( iFace = 0; iFace < cFaces; iFace++ )
		{
			WRITE_DWORD(pbCur, 3); //nFaceVertexIndices

			WRITE_DWORD(pbCur, iFace*3+0);
			WRITE_DWORD(pbCur, iFace*3+1);
			WRITE_DWORD(pbCur, iFace*3+2);
		}

		hr = pxofsave->CreateDataObject(TID_D3DRMMesh,
			NULL,
			NULL,
			cbSize,
			pbData,
			&pDataObject
			);
		if (FAILED(hr))
			goto e_Exit;

		// add the normals to the file
		hr = AddNormals(pxofsave, vecVertices, pDataObject);
		if (FAILED(hr))
			goto e_Exit;

		// write texture coordinates
		hr = AddTextureCoordinates(pxofsave, vecVertices, pDataObject);
		if (FAILED(hr))
			goto e_Exit;

		hr = AddMeshMaterials(pxofsave, vecVertices, strName, pDataObject);
		if (FAILED(hr))
			goto e_Exit;

		hr = pParent->AddDataObject(pDataObject);
		if (FAILED(hr))
			goto e_Exit;

e_Exit:
		SAFE_RELEASE(pDataObject);
		//delete []rgdwMeshMaterials;
		//delete []rgMaterials;
		//delete []rgCropInfo;
		return hr;
	}

	HRESULT ExportDxStaticMesh_SingleTexMesh( LPDIRECTXFILESAVEOBJECT pxofsave, 
											LPDIRECTXFILEDATA pDataObjectRoot, 
											const std::vector<VERTEXNORCOLORTEX4>& vecVertices,
											const TSTRING& strFrameName,
											const TSTRING& strTexName,
											const D3DXMATRIX& matWorld )
	{
		HRESULT hr(S_OK);

		// "."가 들어가면 정상동작을 안하게 된다. 이걸 방지하기 위함. ( .pie )
		TSTRING strFrameNameNEW = sc::util::GetName_ExtErase( strFrameName );

		std::stringstream strStringStream;
		strStringStream << strFrameNameNEW;
		strStringStream << "_";
		strStringStream << g_nFrameCount++;

		OutputDebugString( strStringStream.str().c_str());
		OutputDebugString("\n");

		LPDIRECTXFILEDATA pDataObject = NULL;
		hr = pxofsave->CreateDataObject( TID_D3DRMFrame,
										strStringStream.str().c_str(),
										NULL,
										0,
										NULL,
										&pDataObject );
		if (FAILED(hr))
			goto e_Exit;

		hr = AddTransform(pxofsave, pDataObject, matWorld );
		if (FAILED(hr))
			goto e_Exit;

		//hr = AddObjectOffsetTransform(psc, pDataObject, &pMeshParent);
		//if (FAILED(hr))
		//	goto e_Exit;

		hr = AddMesh(pxofsave, strTexName, vecVertices, pDataObject);
		if (FAILED(hr))			goto e_Exit;


		hr = pDataObjectRoot->AddDataObject(pDataObject);
		if (FAILED(hr))
			goto e_Exit;

e_Exit:
		SAFE_RELEASE(pDataObject);

		return hr;
	}

	HRESULT EnumTree( LPDIRECT3DDEVICEQ pd3dDevice,LPDIRECTXFILESAVEOBJECT pxofsave, DxLandMan* pLandMan, LPDIRECTXFILEDATA *ppDataObjectRoot )
	{
		HRESULT hr = S_OK;

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		std::stringstream strStringStream;
		strStringStream << "Scene_Root";

		OutputDebugString( strStringStream.str().c_str() );
		OutputDebugString("\n");

		hr = pxofsave->CreateDataObject( TID_D3DRMFrame,
												strStringStream.str().c_str(),
												NULL,
												0,
												NULL,
												ppDataObjectRoot );
		if (FAILED(hr))
			goto e_Exit;

		hr = AddTransform( pxofsave, *ppDataObjectRoot, matWorld );
		if (FAILED(hr))
			goto e_Exit;

		if ( pLandMan )
		{
			// DxStaticMesh
			if ( pLandMan->GetStaticMesh() )
			{
				pLandMan->GetStaticMesh()->ExportXFile( pxofsave, *ppDataObjectRoot, "StaticMesh", matWorld );
			}

			// DxStaticMaterialMesh
			if ( pLandMan->GetStaticMaterialMesh() )
			{
				pLandMan->GetStaticMaterialMesh()->ExportXFile( pxofsave, *ppDataObjectRoot, "StaticMesh", matWorld );
			}

			// Frame 이름이 Piece 이름으로 들어간다.
			// 하지만 이름 첫 글자가 숫자면 x file 로딩 안되는 문제가 있어서 내부적으로 _T("_") 가 추가됨.
			// 또한 "." 있으면 안되서 확장자 자체를 없앤다.
			//
			// DxPieceManager
			if ( pLandMan->GetPieceManager() )
			{
				pLandMan->GetPieceManager()->ExportXFile( pxofsave, *ppDataObjectRoot );
			}

			// NavigationMesh
			if ( pLandMan->GetNaviMesh() )
			{
				pLandMan->GetNaviMesh()->ExportXFile( pxofsave, *ppDataObjectRoot, "NaviMesh", strEmptyName );
			}

			// DXLANDSKINOBJ
			pLandMan->ExportXFileForLandSkin( pd3dDevice, pxofsave, *ppDataObjectRoot, "SkinObject", strEmptyName );
		}

e_Exit:
		return hr;
	}

	//HRESULT ExportXFile( const TCHAR *szFilename, DxStaticMesh* pStaticMesh, BOOL bSuppressPrompts, BOOL bSaveSelection, HWND hwndParent )
	HRESULT ExportXFile( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR *szFilename, DxLandMan* pLandMan )
	{
		g_nFrameCount = 0;
		LPDIRECTXFILE pxofapi = NULL;
		LPDIRECTXFILEDATA pRootData = NULL;
		LPDIRECTXFILESAVEOBJECT pxofsave = NULL; 
		HRESULT hr = S_OK;
		//INode *pRootNode = NULL;
		//LPDIRECTXFILESAVEOBJECT m_pxofsave;
		//SPreprocessContext pc;
		//SDialogOptions DlgOptions;
		bool bInterfaceStart = false;

		//assert(szFilename && pExportInterface && pInterface);
		assert( szFilename );

		ExportProgress::EndPos = 4;
		ExportProgress::CurPos = 0;

		//// Extract scene information
		//bInterfaceStart = true;
		//pInterface->ProgressStart(_T("Extracting skinning data"),TRUE,dummyFn,NULL);
		//pInterface->ProgressUpdate(0);
		////pInterface->ProgressUpdate(100);

		//// first find the root node
		//hr = FindRootNode(pExportInterface->theScene, &pRootNode);
		//if (FAILED(hr))
		//	goto e_Exit;

		//// figure out bone counts, etc.
		//hr = Preprocess(&pc, pRootNode);
		//if (FAILED(hr))
		//	goto e_Exit;
		//pInterface->ProgressUpdate(25);

		//if( bInterfaceStart )
		//{
		//	bInterfaceStart = false;
		//	pInterface->ProgressEnd();
		//}

		//// if prompts not suppressed, then check with the user on options
		//if (!bSuppressPrompts)
		//{
		//	DialogBoxParam(
		//		g_hInstance,
		//		MAKEINTRESOURCE( IDD_PANEL ),
		//		hwndParent,
		//		XSkinExpOptionsDlgProc,
		//		(LPARAM)&DlgOptions );

		//	if (!DlgOptions.m_bProceedWithExport)
		//		goto e_Exit;
		//}

		//bInterfaceStart = true;
		//pInterface->ProgressStart(_T("Exporting data"),TRUE,dummyFn,NULL);
		//pInterface->ProgressUpdate(25);

		// Create xofapi object.
		hr = DirectXFileCreate(&pxofapi);
		if (FAILED(hr))
			goto e_Exit;

		// Register templates for d3drm.
		hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
			D3DRM_XTEMPLATE_BYTES);
		if (FAILED(hr))
			goto e_Exit;

		hr = pxofapi->RegisterTemplates(
            (LPVOID)XSKINEXP_TEMPLATES_MAX,
			static_cast<DWORD> (strlen(XSKINEXP_TEMPLATES_MAX)));
		if (FAILED(hr))
			goto e_Exit;

		// Create save object.
		hr = pxofapi->CreateSaveObject(szFilename,    // filename
			DXFILEFORMAT_TEXT,	//DXFILEFORMAT_BINARY,  // binary or text
			&pxofsave);
		if (FAILED(hr))
			goto e_Exit;

		hr = pxofsave->SaveTemplates(3, aIds);
		if (FAILED(hr))
			goto e_Exit;

		// then write the whole tree out into file data's
		hr = EnumTree( pd3dDevice, pxofsave, pLandMan, &pRootData );
		if (FAILED(hr))
			goto e_Exit;

		//if( bInterfaceStart )
		//{
		//	bInterfaceStart = false;
		//	pInterface->ProgressEnd();
		//}

		//bInterfaceStart = true;
		//pInterface->ProgressStart(_T("Saving data to file"),TRUE,dummyFn,NULL);
		//pInterface->ProgressUpdate(50);

		ExportProgress::CurPos = 1;

		// now save that file data to the file
		hr = pxofsave->SaveData(pRootData);
		if (FAILED(hr))
			goto e_Exit;

		//pInterface->ProgressUpdate(75);

		ExportProgress::CurPos = 2;

e_Exit:
		//if (FAILED(hr))
		//{
		//	OutputDebugString("File was not successfully exported.");
		//	{
		//		TCHAR errstr[500 + _MAX_PATH];
		//		_stprintf(errstr,"Could not write to file: %s.\n"\
		//			"Try checking the file's permissions, or if it is currently open.",szFilename);
		//		MessageBox(hwndParent,errstr,_T("Error!"),MB_OK);
		//	}
		//}

		// falling through
		// Free up outstanding interfaces
		SAFE_RELEASE(pxofsave);
		SAFE_RELEASE(pRootData);
		SAFE_RELEASE(pxofapi);

		ExportProgress::CurPos = 3;

		//if( bInterfaceStart )
		//{
		//	pInterface->ProgressUpdate( 100 );
		//	pInterface->ProgressEnd();
		//}

		return hr;
	}
}