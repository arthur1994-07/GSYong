========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : WorldEdit
========================================================================


AppWizard has created this WorldEdit application for you.  This application
not only demonstrates the basics of using the Microsoft Foundation classes
but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your WorldEdit application.

WorldEdit.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

WorldEdit.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CWorldEditApp application class.

WorldEdit.cpp
    This is the main application source file that contains the application
    class CWorldEditApp.

WorldEdit.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.

WorldEdit.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

res\WorldEdit.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file WorldEdit.rc.

res\WorldEdit.rc2
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.



/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the CMainFrame
    class. Edit this toolbar bitmap using the resource editor, and
    update the IDR_MAINFRAME TOOLBAR array in WorldEdit.rc to add
    toolbar buttons.
/////////////////////////////////////////////////////////////////////////////

AppWizard creates one document type and one view:

WorldEditDoc.h, WorldEditDoc.cpp - the document
    These files contain your CWorldEditDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CWorldEditDoc::Serialize).

WorldEditView.h, WorldEditView.cpp - the view of the document
    These files contain your CWorldEditView class.
    CWorldEditView objects are used to view CWorldEditDoc objects.



/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named WorldEdit.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is 
in a language other than the operating system's current language, you
will need to copy the corresponding localized resources MFC42XXX.DLL
from the Microsoft Visual C++ CD-ROM onto the system or system32 directory,
and rename it to be MFCLOC.DLL.  ("XXX" stands for the language abbreviation.
For example, MFC42DEU.DLL contains resources translated to German.)  If you
don't do this, some of the UI elements of your application will remain in the
language of the operating system.

/////////////////////////////////////////////////////////////////////////////

//	TEST :
//
//static DWORD g_dwVertexCount = 0;
//static DWORD g_dwFaceCount = 0;
//static LPDIRECT3DVERTEXBUFFER8 g_pVertexBuffer = NULL;
//static LPDIRECT3DINDEXBUFFER8 g_pIndexBuffer = NULL;

//	Note : 대칼 테스트 코드.
//
//if ( g_bCollision )
//{
//	DxDecal::GetInstance().CreateDecal ( g_vCollision, 
//		D3DXVECTOR3(0,1,0), D3DXVECTOR3(1,0,0), D3DXVECTOR3(40,40,40),
//		m_pDxOctreeManager->GetOctree() );

//	g_dwFaceCount = DxDecal::GetInstance().GetFaceCount ();
//	g_dwVertexCount = DxDecal::GetInstance().GetVertexCount ();
//	g_pVertexBuffer = DxDecal::GetInstance().CreateVertexBuffer ( m_pd3dDevice );
//	g_pIndexBuffer = DxDecal::GetInstance().CreateIndexBuffer ( m_pd3dDevice );
//}

//if ( m_bHeightBaseCamera )
//{
//	LPDXFRAME pDxFrame = NULL;
//	BOOL bCollision;
//	D3DXVECTOR3 vCollision;

//	D3DXVECTOR3 vLookAt = DxViewPort::GetInstance().GetLookatPt ();
//	D3DXVECTOR3 vHeight = D3DXVECTOR3 ( vLookAt.x, vLookAt.y+2000.0f, vLookAt.z );
//	vLookAt.y -= 2000.0f;

//	m_pDxOctreeManager->IsCollision ( vHeight, vLookAt, vCollision, bCollision, pDxFrame );
//	
//	if ( bCollision )
//	{
//		DxViewPort::GetInstance().CameraHeight ( vCollision.y );
//	}
//}

//	TEST :
//
//#define BOXVERTEX (D3DFVF_XYZ)

//float fTreeRadius = 2.0f;
//D3DXVECTOR3 vTreeCenter;
//vTreeCenter = DxViewPort::GetInstance().GetLookatPt();

//D3DXVECTOR3 VertBox[8];
//VertBox[0] = vTreeCenter + D3DXVECTOR3(-fTreeRadius,+fTreeRadius+80,-fTreeRadius);
//VertBox[1] = vTreeCenter + D3DXVECTOR3(-fTreeRadius,+fTreeRadius+80,+fTreeRadius);
//VertBox[2] = vTreeCenter + D3DXVECTOR3(+fTreeRadius,+fTreeRadius+80,+fTreeRadius);
//VertBox[3] = vTreeCenter + D3DXVECTOR3(+fTreeRadius,+fTreeRadius+80,-fTreeRadius);

//VertBox[4] = vTreeCenter + D3DXVECTOR3(-fTreeRadius,-fTreeRadius-20,-fTreeRadius);
//VertBox[5] = vTreeCenter + D3DXVECTOR3(-fTreeRadius,-fTreeRadius-20,+fTreeRadius);
//VertBox[6] = vTreeCenter + D3DXVECTOR3(+fTreeRadius,-fTreeRadius-20,+fTreeRadius);
//VertBox[7] = vTreeCenter + D3DXVECTOR3(+fTreeRadius,-fTreeRadius-20,-fTreeRadius);

//WORD IndexBox[24] = { 0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7};

//m_pd3dDevice->SetVertexShader( BOXVERTEX );
//m_pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 8, 12,
//	IndexBox, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(D3DFVF_XYZ) );

//LPDXFRAME const pDxFrame = m_pFrameMesh->GetCollisionDetectedFrame();
//if ( pDxFrame )
//{
//	vTreeCenter = m_pFrameMesh->GetCollisionPos ();

//	fTreeRadius = 5.0f;
//	D3DXVECTOR3 VertBox[8];
//	VertBox[0] = vTreeCenter + D3DXVECTOR3(-fTreeRadius,+fTreeRadius,-fTreeRadius);
//	VertBox[1] = vTreeCenter + D3DXVECTOR3(-fTreeRadius,+fTreeRadius,+fTreeRadius);
//	VertBox[2] = vTreeCenter + D3DXVECTOR3(+fTreeRadius,+fTreeRadius,+fTreeRadius);
//	VertBox[3] = vTreeCenter + D3DXVECTOR3(+fTreeRadius,+fTreeRadius,-fTreeRadius);

//	VertBox[4] = vTreeCenter + D3DXVECTOR3(-fTreeRadius,-fTreeRadius,-fTreeRadius);
//	VertBox[5] = vTreeCenter + D3DXVECTOR3(-fTreeRadius,-fTreeRadius,+fTreeRadius);
//	VertBox[6] = vTreeCenter + D3DXVECTOR3(+fTreeRadius,-fTreeRadius,+fTreeRadius);
//	VertBox[7] = vTreeCenter + D3DXVECTOR3(+fTreeRadius,-fTreeRadius,-fTreeRadius);

//	WORD IndexBox[24] = { 0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7};

//	m_pd3dDevice->SetVertexShader( BOXVERTEX );
//	m_pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_LINELIST, 0, 8, 12,
//		IndexBox, D3DFMT_INDEX16, VertBox, D3DXGetFVFVertexSize(D3DFVF_XYZ) );
//}


//if ( g_pVertexBuffer )
//{
//	HRESULT hr;
//	hr = m_pd3dDevice->SetStreamSource ( 0, g_pVertexBuffer, D3DXGetFVFVertexSize(DECALVERTEX::FVF) );
//	if ( FAILED ( hr ) ) return E_FAIL;
//	
//	hr = m_pd3dDevice->SetVertexShader ( DECALVERTEX::FVF );
//	if ( FAILED ( hr ) ) return E_FAIL;

//	hr = m_pd3dDevice->SetIndices ( g_pIndexBuffer, 0 );
//	if ( FAILED ( hr ) ) return E_FAIL;
//	
//	m_pd3dDevice->SetTexture( 0, NULL );
//	m_pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST,
//			0,
//			g_dwVertexCount,
//			0,
//			g_dwFaceCount
//		);
//}