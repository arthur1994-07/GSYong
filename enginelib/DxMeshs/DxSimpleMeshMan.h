#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>
#include <algorithm>

#include "../../SigmaCore/File/FileFind.h"
#include "../DxTools/TextureManager.h"
#include "./DxSimpleMeshAddData.h"

class DxSimMesh
{
protected:
	enum
	{
		EM_BOUNDSPHERE		= 0x0001,
		EM_BOUNDBOX			= 0x0002,
	};

	D3DXVECTOR3			m_vCenter;
	float				m_fRadius;

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

public:
	std::string			m_strFileName;
	DWORD				m_dwRef;
	BOOL				m_dwFlag;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	DWORD						m_dwNumMaterials; // Materials for the mesh
	D3DXMATERIAL*				m_pMaterials;
	TextureResource* m_pTextureRes;
	TEXTURE_ALPHA* m_emTexType;
	BOOL						m_bUseMaterials;

	D3DXVECTOR3*		m_pVertex[3];
	D3DXVECTOR3*		m_pNormal[3];

private:
	LPD3DXMESH			m_pLocalMesh;
	LPD3DXMESH			m_pBlendMesh;

public:
	HRESULT FrameMove ( float fTime );	// -1 ~ 1

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture=NULL, D3DMATERIALQ* pMaterials=NULL, BOOL bBlendMesh=FALSE );

	HRESULT RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials=NULL );

private:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture, D3DMATERIALQ* pMaterials, LPD3DXMESH pLocalMesh );
	HRESULT RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials, LPD3DXMESH pLocalMesh );

public:
	LPD3DXMESH GetLocalMesh ()  { return m_pLocalMesh; }
	HRESULT GetBoundSphere ( D3DXVECTOR3 &vCenter, float &fRadius );
	HRESULT GetBoundBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	void    UseMeshMaterials ( BOOL bFlag ) { m_bUseMaterials = bFlag; }
	HRESULT SetFVF ( DWORD dwFVF );

 //   void	SetRemindData(
 //       LPDIRECT3DDEVICEQ pd3dDevice,
 //       BOOL bTexThread,
 //       TEXTURE_RESIZE uiResizeLevel = TEXTURE_RESIZE_NONE,
 //       TEXTURE_CATEGORY emCategory = TEXTURE_SIMPLEMESH );
	//BOOL	IsRemindData() { return m_dwFlag & EM_REMIND_DATA; }

    //BOOL IsCollision( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd ) const;

	HRESULT SetBlendMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxSimMesh* pSimMesh_0, DxSimMesh* pSimMesh_1 );
	void	ReleaseBlendMesh();

public:
	HRESULT Create (
        LPDIRECT3DDEVICEQ pd3dDevice,
		const TSTRING& strName,
		void* pBuf,
		UINT bufSize, 
        BOOL bTexThread = TRUE,
		TEXTURE_RESIZE uiResizeLevel = TEXTURE_RESIZE_NONE,
        TEXTURE_CATEGORY emCategory = TEXTURE_SIMPLEMESH );

	BOOL LoadFile( 
		LPDIRECT3DDEVICEQ pd3dDevice,
		void* pBuf,
		UINT bufSize, 
		BOOL bTexThread,
		TEXTURE_RESIZE uiResizeLevel,
		TEXTURE_CATEGORY emCategory );

	//HRESULT LoadFile( LPDIRECT3DDEVICEQ pd3dDevice, sc::BaseStream& stream );

	HRESULT Destroy ();

	void SaveFile();

public:
	DxSimMesh ();
	virtual ~DxSimMesh ();
};

class DxSimpleMeshMan : public OnceRefAndEnableAlgorithm<DxSimMesh,SIMPLEMESH_ADD_DATA,OnceRefNTime>
{
public:
	struct Resource : public ResourceManager::Resource
	{
		explicit Resource(DxSimMesh* pEffPropGroup) {	m_spCompletedData = SP_COMPLETED_DATA( pEffPropGroup ); }
		Resource(const ResourceManager::Resource& res) : ResourceManager::Resource(res) {}
		Resource() {}

		DxSimMesh* get()  { return Update() ? m_spCompletedData.get() : NULL; }
	};

public:
	void			SetPath(const TSTRING& path);
	const TSTRING&	GetPath()							{ return m_path; }

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	TSTRING				m_path;
	sc::CFileFindTree	m_sFileTree;

public:
	Resource	Load (
        const TSTRING& strName,
		BOOL bMeshThread,
        BOOL bTexThread,
        TEXTURE_RESIZE uiResizeLevel = TEXTURE_RESIZE_NONE,
        TEXTURE_CATEGORY emCategory = TEXTURE_CATEGORY_SIZE );

	BOOL ExistFile( const TSTRING& strName );

    //DxSimMesh*	BlendLoad (
    //    LPDIRECT3DDEVICEQ pd3dDevice,
    //    char** szFile,
    //    DWORD dwFVF,
    //    BOOL bRemindData );

	//HRESULT		Release ( const char* szFile, DWORD dwFVF, BOOL bMorph=FALSE );
	//HRESULT		Delete ( const char* szFile, DWORD dwFVF, BOOL bMorph=FALSE );

	//DxSimMesh*	Find ( const char* szFile, BOOL bMorph=FALSE );

//protected:
//	void		ConvertFileName( char* szOut, int nStrLen, const char* szFile, DWORD dwFVF );

//public:
//	HRESULT CleanUp ();

private:
	virtual void ProcessCompleteData(
		SP_QUEUE_DATA_VECTOR& dataVec,
		NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
		DWORD dwPrevTick, 
		DWORD nWorkTime );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	//HRESULT DeleteDeviceObjects();

protected:
	DxSimpleMeshMan(void);

public:
	static DxSimpleMeshMan& GetInstance();
};

//
//class DxSimpleMeshManMorph : public OnceRefAndEnableAlgorithm<DxSimMesh,EMPTY_ADD_DATA,OnceRefNTime>
//{
//public:
//	void			SetPath(const TSTRING& path)		{ m_path = path; }
//	const TSTRING&	GetPath()							{ return m_path; }
//
//protected:
//	LPDIRECT3DDEVICEQ	m_pd3dDevice;
//
//public:
//	DxSimMesh*	Load (
//		LPDIRECT3DDEVICEQ pd3dDevice,
//		const char* szFile,
//		DWORD dwFVF,
//		BOOL bTexThread,
//		TEXTURE_RESIZE uiResizeLevel = TEXTURE_RESIZE_NONE,
//		TEXTURE_CATEGORY emCategory = TEXTURE_CATEGORY_SIZE );
//
//	//DxSimMesh*	BlendLoad (
//	//    LPDIRECT3DDEVICEQ pd3dDevice,
//	//    char** szFile,
//	//    DWORD dwFVF );
//
//	HRESULT		Release ( const char* szFile, DWORD dwFVF, BOOL bMorph=FALSE );
//	HRESULT		Delete ( const char* szFile, DWORD dwFVF, BOOL bMorph=FALSE );
//
//	DxSimMesh*	Find ( const char* szFile, BOOL bMorph=FALSE );
//
//protected:
//	void		ConvertFileName( char* szOut, int nStrLen, const char* szFile, DWORD dwFVF );
//
//public:
//	HRESULT CleanUp ();
//
//public:
//	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
//	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
//
//protected:
//	DxSimpleMeshManMorph(void);
//
//public:
//	static DxSimpleMeshManMorph& GetInstance();
//};

typedef DxSimpleMeshMan::Resource DxSimMeshResource;