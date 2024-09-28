//-----------------------------------------------------------------------------
// File: ExportXFile.cpp
//
// Desc: Functions used to export max data to an X File
//
// Copyright (C) 1998-2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "pch.h"
#include "XSkinExp.h"
#include "MeshData.h"

#include <initguid.h>

#include "XSkinExpTemplates.h"
#include "dxfile.h"
#include "rmxfguid.h"
#include "rmxftmpl.h"

#ifdef _MAX_2012
    #include "../Max12_sdk/include/decomp.h"
#else
    #include "../Max11_sdk/include/decomp.h"
#endif

#include "../EngineLib/DxTools/EncryptDataDef.h"

#include <share.h>

#include "./ExportXFile.h"

bool IsSelectedChild( INode *pNode )
{
	if( !pNode )
		return false;

	if( pNode->Selected() )
		return true;

	int nChildren = pNode->NumberOfChildren();
    for( int i=0; i<nChildren; ++i )
    {
		if( IsSelectedChild( pNode->GetChildNode( i ) ) )
			return true;
	}

	return false;
}

void MatToMsg ( Matrix3 &matset, const char* pszCont )
{
	//
	AffineParts ap;
	decomp_affine ( matset, &ap );

	float rotAngle;
	Point3 rotAxis;
	AngAxisFromQ(ap.q, &rotAngle, rotAxis);

	//
	float rx, ry, rz;
	ap.q.GetEuler ( &rx, &ry, &rz );


	char szTemp[512];
	sprintf ( szTemp, "회전  %f, %f, %f,  스케일  %f, %f, %f  이동  %f, %f, %f",
		rx, ry, rz, ap.k.x, ap.k.y, ap.k.z, ap.t.x, ap.t.y, ap.t.z );

	::MessageBox ( NULL, szTemp, pszCont, MB_OK );
}

void RHMatToLHMat ( Matrix3 &matRH, Matrix3 &matLH )
{
	//	EDITED : matRH 성분을 분해.
	//
	AffineParts ap;
	decomp_affine ( matRH, &ap );

	//	Note : Position 성분 - y와 z를 뒤바꾼다.
	//
	Point3 Trans;
	Trans.x = ap.t.x;
	Trans.y = ap.t.z;
	Trans.z = ap.t.y;

	ap.t.x = Trans.x;
	ap.t.y = Trans.y;
	ap.t.z = Trans.z;

	//	Note : Rotation 성분 - y와 z를 뒤바꾼다.
	//
	Quat Q;
	Q.x = ap.q.x;
	Q.y = ap.q.z;
	Q.z = ap.q.y;
	Q.w = -ap.q.w;
	ap.q = Q;

	Q.x = ap.u.x;
	Q.y = ap.u.z;
	Q.z = ap.u.y;
	Q.w = -ap.u.w;
	ap.u = Q;

	Point3 Scale;
	Scale.x = ap.k.x;
	Scale.y = ap.k.z;
	Scale.z = ap.k.y;

	ap.k.x = Scale.x;
	ap.k.y = Scale.y;
	ap.k.z = Scale.z;

	//
	Matrix3 srtm, rtm, ptm, stm, ftm;

	ptm.IdentityMatrix ();
	ptm.SetTrans ( ap.t );

	ap.q.MakeMatrix ( rtm );
	ap.u.MakeMatrix ( srtm );

	stm = ScaleMatrix ( ap.k );
	ftm = ScaleMatrix ( Point3(ap.f,ap.f,ap.f) );

	matLH = Inverse(srtm) * stm * srtm * rtm * ftm * ptm;
}

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

#define WRITE_MATRIX4_FROM_MATRIX3(pbCur, _matrix3) {Point3 __tempRow = ((Matrix3)_matrix3).GetRow(0);\
                                WRITE_POINT3(pbCur, __tempRow); WRITE_FLOAT(pbCur, 0);\
                                __tempRow = _matrix3.GetRow(1); WRITE_POINT3(pbCur, __tempRow); WRITE_FLOAT(pbCur, 0);\
                                __tempRow = _matrix3.GetRow(2); WRITE_POINT3(pbCur, __tempRow); WRITE_FLOAT(pbCur, 0);\
                                __tempRow = _matrix3.GetRow(3); WRITE_POINT3(pbCur, __tempRow); WRITE_FLOAT(pbCur, 1);}

#define WRITE_MATRIX(pbCur, mat) { *(D3DXMATRIX*)pbCur = mat;\
                               pbCur = (PBYTE)pbCur + sizeof(D3DXMATRIX);}

const GUID* aIds[] = {&DXFILEOBJ_XSkinMeshHeader,
                &DXFILEOBJ_VertexDuplicationIndices,
                &DXFILEOBJ_SkinWeights};

INT_PTR CALLBACK XSkinExpOptionsDlgProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam ) 
{
    static SDialogOptions *pDialogOptions = NULL;
    TCHAR buff[8];

    switch(message) 
    {
    case WM_INITDIALOG:
        pDialogOptions = (SDialogOptions *)lParam;

        CenterWindow(hWnd,GetParent(hWnd));

		//
        switch (pDialogOptions->m_ExportDataOption)
        {
        case EXPD_FRAME|EXPD_ANIMATION:
            CheckRadioButton(hWnd,IDC_ANIMATIONANDFRAME,IDC_ONLYANIMATION,IDC_ANIMATIONANDFRAME);
            break;

        case EXPD_FRAME:
            CheckRadioButton(hWnd,IDC_ANIMATIONANDFRAME,IDC_ONLYANIMATION,IDC_ONLYFRAME);
            break;

        case EXPD_ANIMATION:
            CheckRadioButton(hWnd,IDC_ANIMATIONANDFRAME,IDC_ONLYANIMATION,IDC_ONLYANIMATION);
            break;

        case EXPD_KEY_ANI:
            CheckRadioButton(hWnd,IDC_ANIMATIONANDFRAME,IDC_ONLYANIMATION,IDC_ONLYKEYANI);
            break;
        }

        CheckDlgButton(hWnd, IDC_LOOPINGANIMATION, 
                pDialogOptions->m_bLoopingAnimationData ? BST_CHECKED : BST_UNCHECKED);

        CheckDlgButton(hWnd, IDC_PATCHDATA, 
                pDialogOptions->m_bSavePatchData ? BST_CHECKED : BST_UNCHECKED);

        if (pDialogOptions == NULL)
            return FALSE;

        switch (pDialogOptions->m_xFormat)
        {
        case DXFILEFORMAT_BINARY:
            CheckRadioButton(hWnd,IDC_TEXT,IDC_BINARYCOMPRESSED,IDC_BINARY);
            break;

        case DXFILEFORMAT_TEXT:
            CheckRadioButton(hWnd,IDC_TEXT,IDC_BINARYCOMPRESSED,IDC_TEXT);
            break;

        case DXFILEFORMAT_BINARY | DXFILEFORMAT_COMPRESSED:
            CheckRadioButton(hWnd,IDC_TEXT,IDC_BINARYCOMPRESSED,IDC_BINARYCOMPRESSED);
            break;
        }

        _stprintf(buff,_T("%d"),pDialogOptions->m_cMaxBonesPerVertex);
        SetDlgItemText(hWnd,IDC_MAX_BONES_PER_VERTEX,buff);

        _stprintf(buff,_T("%d"),pDialogOptions->m_cMaxBonesPerFace);
        SetDlgItemText(hWnd,IDC_MAX_BONES_PER_FACE,buff);

        _stprintf(buff,_T("%d"),pDialogOptions->m_iAnimSamplingRate);
        SetDlgItemText(hWnd,IDC_SAMPLERATE,buff);

		_stprintf(buff,_T("%d"),pDialogOptions->m_iAnimStart);
		SetDlgItemText(hWnd,IDC_EDIT_ANISTART,buff);

		_stprintf(buff,_T("%d"),pDialogOptions->m_iAnimEnd);
		SetDlgItemText(hWnd,IDC_EDIT_ANIEND,buff);

		//
        CheckDlgButton(hWnd, IDC_CHECK_LEFTHAND, 
                pDialogOptions->m_bLeftHandAxis ? BST_CHECKED : BST_UNCHECKED);

		CheckDlgButton(hWnd, IDC_CHECK_HIDEBIPED,
			pDialogOptions->m_bHideBiped ? BST_CHECKED : BST_UNCHECKED);

		CheckDlgButton(hWnd, IDC_CHECK_ENCRYPT,
			pDialogOptions->m_bTexEncrypt ? BST_CHECKED : BST_UNCHECKED);

        return TRUE;

    case WM_CLOSE:
        pDialogOptions->m_bProceedWithExport = FALSE;

        EndDialog(hWnd, 0);
        return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_GO:
            pDialogOptions->m_bProceedWithExport = TRUE;

            EndDialog(hWnd,0);
            break;

        case IDC_CANCEL:
            pDialogOptions->m_bProceedWithExport = FALSE;

            EndDialog(hWnd,0);
            break;

        case IDC_TEXT:
            pDialogOptions->m_xFormat = DXFILEFORMAT_TEXT;
            break;

        case IDC_BINARY:
            pDialogOptions->m_xFormat = DXFILEFORMAT_BINARY;
            break;

        case IDC_BINARYCOMPRESSED:
            pDialogOptions->m_xFormat = DXFILEFORMAT_BINARY | DXFILEFORMAT_COMPRESSED;
            break;

			//	EDITED : IDC_ANIMATION이 삭제 되고 아래와 같이 고침.
			//
		case IDC_ANIMATIONANDFRAME:
			pDialogOptions->m_ExportDataOption = EXPD_FRAME|EXPD_ANIMATION;
			break;

		case IDC_ONLYFRAME:
			pDialogOptions->m_ExportDataOption = EXPD_FRAME;
			break;

		case IDC_ONLYANIMATION:
			pDialogOptions->m_ExportDataOption = EXPD_ANIMATION;
			break;

        case IDC_ONLYKEYANI:
			pDialogOptions->m_ExportDataOption = EXPD_KEY_ANI;
			break;

        case IDC_LOOPINGANIMATION:
            pDialogOptions->m_bLoopingAnimationData = !pDialogOptions->m_bLoopingAnimationData;
            break;

        case IDC_PATCHDATA:
            pDialogOptions->m_bSavePatchData = !pDialogOptions->m_bSavePatchData;
            break;

        case IDC_SAMPLERATE:
            GetDlgItemText(hWnd,IDC_SAMPLERATE,buff,sizeof(buff) / sizeof(TCHAR));
            pDialogOptions->m_iAnimSamplingRate = _ttoi(buff);
            break;

		case IDC_EDIT_ANISTART:
			GetDlgItemText(hWnd,IDC_EDIT_ANISTART,buff,sizeof(buff) / sizeof(TCHAR));
			pDialogOptions->m_iAnimStart = _ttoi(buff);
			break;

		case IDC_EDIT_ANIEND:
			GetDlgItemText(hWnd,IDC_EDIT_ANIEND,buff,sizeof(buff) / sizeof(TCHAR));
			pDialogOptions->m_iAnimEnd = _ttoi(buff);
			break;

			//	EDITED : 추가 설정.
			//
		case IDC_CHECK_LEFTHAND:
			{
				HWND hWndLeftHand = GetDlgItem ( hWnd, IDC_CHECK_LEFTHAND );
				if ( SendMessage ( hWndLeftHand, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
					pDialogOptions->m_bLeftHandAxis = TRUE;
				else
					pDialogOptions->m_bLeftHandAxis = FALSE;
			}
			break;

		case IDC_CHECK_HIDEBIPED:
			{
				HWND hWndHideBiped = GetDlgItem ( hWnd, IDC_CHECK_LEFTHAND );
				if ( SendMessage ( hWndHideBiped, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
					pDialogOptions->m_bHideBiped = TRUE;
				else
					pDialogOptions->m_bHideBiped = FALSE;
			}
			break;
		case IDC_CHECK_ENCRYPT:
			{
				HWND hWndTexEncrypt = GetDlgItem ( hWnd, IDC_CHECK_ENCRYPT );
				if ( SendMessage ( hWndTexEncrypt, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
					pDialogOptions->m_bTexEncrypt = TRUE;
				else
					pDialogOptions->m_bTexEncrypt = FALSE;
			}

        default:
            break;
        }
    }

    return FALSE;
}

// ================================================== FindSkinModifier()
Modifier* FindSkinModifier( INode* nodePtr )
{
    // Get object from node. Abort if no object.
    Object* ObjectPtr = nodePtr->GetObjectRef();
	if( !ObjectPtr )
        return NULL;

    // Is derived object ?
    while( ObjectPtr && ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID )
    {
        // Yes -> Cast.
        IDerivedObject* DerivedObjectPtr = (IDerivedObject*)(ObjectPtr);

        // Iterate over all entries of the modifier stack.
        int ModStackIndex = 0;
        while( ModStackIndex < DerivedObjectPtr->NumModifiers() )
        {
            // Get current modifier.
            Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

            // Is this Skin ?
            if( ModifierPtr->ClassID() == SKIN_CLASSID )
            {
                // Yes -> Exit.
                return ModifierPtr;
            }

            // Next modifier stack entry.
            ModStackIndex++;
        }

        ObjectPtr = DerivedObjectPtr->GetObjRef();
    }

    // Not found.
    return NULL;
}

// ================================================== FindPhysiqueModifier()
// Find if a given node contains a Physique Modifier
// DerivedObjectPtr requires you include "modstack.h" from the MAX SDK
Modifier* FindPhysiqueModifier (INode* nodePtr)
{
    // Get object from node. Abort if no object.
    Object* ObjectPtr = nodePtr->GetObjectRef();
    if( !ObjectPtr )
        return NULL;

    // Is derived object ?
    while( ObjectPtr && ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID )
    {
        // Yes -> Cast.
        IDerivedObject* DerivedObjectPtr = (IDerivedObject*)(ObjectPtr);

        // Iterate over all entries of the modifier stack.
        int ModStackIndex = 0;
        while( ModStackIndex < DerivedObjectPtr->NumModifiers() )
        {
            // Get current modifier.
            Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

            // Is this Physique ?
            if( ModifierPtr->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
            {
                // Yes -> Exit.
                return ModifierPtr;
            }

            // Next modifier stack entry.
            ModStackIndex++;
        }

        ObjectPtr = DerivedObjectPtr->GetObjRef();
    }

    // Not found.
    return NULL;
}

// ================================================== GetTriObjectFromObjRef
// Return a pointer to a TriObject given an object reference or return NULL
// if the node cannot be converted to a TriObject
TriObject *GetTriObjectFromObjRef(
	Object* pObj, 
	BOOL *pbDeleteIt ) 
{
	TriObject *pTri;

	assert(pObj != NULL);
	assert(pbDeleteIt != NULL);

    *pbDeleteIt = FALSE;

    if (pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
	{ 
        pTri = (TriObject *) pObj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));

        // Note that the TriObject should only be deleted
        // if the pointer to it is not equal to the object
        // pointer that called ConvertToType()
        if (pObj != pTri) 
			*pbDeleteIt = TRUE;

        return pTri;
    } 
	else 
	{
        return NULL;
    }
}

// ================================================== GetTriObjectFromObjRef
// Return a pointer to a TriObject given an object reference or return NULL
// if the node cannot be converted to a TriObject
PatchObject *GetPatchObjectFromObjRef(
	Object* pObj, 
	BOOL *pbDeleteIt ) 
{
	PatchObject *pPatchObject;

	assert(pObj != NULL);
	assert(pbDeleteIt != NULL);

    *pbDeleteIt = FALSE;

    if (pObj->CanConvertToType(Class_ID(PATCHOBJ_CLASS_ID, 0))) 
	{ 
        pPatchObject = (PatchObject *) pObj->ConvertToType(0, Class_ID(PATCHOBJ_CLASS_ID, 0));

        // Note that the PatchObject should only be deleted
        // if the pointer to it is not equal to the object
        // pointer that called ConvertToType()
        if (pObj != pPatchObject) 
			*pbDeleteIt = TRUE;

        return pPatchObject;
    } 
	else 
	{
        return NULL;
    }
}

// ================================================== IsExportableMesh()
BOOL IsExportableMesh(INode* pNode, Object* &pObj)
{
    ULONG superClassID;

    assert(pNode);
    pObj = pNode->GetObjectRef();

    if (pObj == NULL)
    {
        return FALSE;
    }

    superClassID = pObj->SuperClassID();
    //find out if mesh is renderable

    if( !pNode->Renderable() || pNode->IsNodeHidden())
    {
        return FALSE;
    }

    BOOL bFoundGeomObject = FALSE;
    //find out if mesh is renderable (more)
    switch(superClassID)
    {
    case GEN_DERIVOB_CLASS_ID:

        do
        {
            pObj = ((IDerivedObject*)pObj)->GetObjRef();
            superClassID = pObj->SuperClassID();
        }
        while( superClassID == GEN_DERIVOB_CLASS_ID );

        switch(superClassID)
        {
        case GEOMOBJECT_CLASS_ID:
            bFoundGeomObject = TRUE;
            break;
        }
        break;

    case GEOMOBJECT_CLASS_ID:
        bFoundGeomObject = TRUE;

    default:
        break;
    }

    return bFoundGeomObject;
}

// ================================================== IsExportablePatchMesh()
BOOL IsExportablePatchMesh(INode* pNode, Object* &pObj)
{
    ULONG superClassID;
	Object *pObjBase;

    assert(pNode != NULL);

    pObj = pNode->GetObjectRef();
    if (pObj == NULL)
        return FALSE;

    if( !pNode->Renderable() || pNode->IsNodeHidden())
        return FALSE;

	pObjBase = pObj;

    superClassID = pObjBase->SuperClassID();
    switch(superClassID)
    {
    case GEN_DERIVOB_CLASS_ID:

        do
        {
            pObjBase = ((IDerivedObject*)pObjBase)->GetObjRef();
            superClassID = pObjBase->SuperClassID();
        }
        while( superClassID == GEN_DERIVOB_CLASS_ID );

        switch(superClassID)
        {
        case GEOMOBJECT_CLASS_ID:
            break;
        }
        break;

    case GEOMOBJECT_CLASS_ID:
		break;

    default:
        break;
    }

	pObj = pObjBase;

	// if the base object is a patch based object
	if ((pObjBase->ClassID() == Class_ID(PATCHOBJ_CLASS_ID, 0))
		|| (pObjBase->ClassID() == Class_ID(PATCHGRID_CLASS_ID, 0)))
	{
		// then check to make sure that the intervening steps don't disallow
		//   a patch object to be converted to
		return pObjBase->CanConvertToType(Class_ID(PATCHOBJ_CLASS_ID, 0));
	}
	else
		return FALSE;
}

// ================================================== dummyFn()
// used by 3DS progress bar
DWORD WINAPI dummyFn(LPVOID arg)
{
    return(0);
}

// -------------------------------------------------------------------------------
//   class      CFindRootNode
//
//   devnote    Helper class for FindRootNode, used to implement callback function
//                  for scene traversal.  Finds the root node by aborting the search
//                  after the first node is returned.  It takes the first node and
//                  walks to the root from there.
//
class CFindRootNode : public ITreeEnumProc
{
public:
    CFindRootNode()
        :m_pNodeRoot(NULL) {}

    virtual int callback(INode *pNode)
    {
        INode *pNodeCur = pNode;

        while (!pNodeCur->IsRootNode())
        {
            pNodeCur = pNodeCur->GetParentNode();
        }
        m_pNodeRoot = pNodeCur;

        return TREE_ABORT;
    }

    INode *m_pNodeRoot;
};

// -------------------------------------------------------------------------------
//  function    FindRootNode
//
//   devnote    Finds the root node of the scene by aborting a tree walk after the first node
//
//   returns    S_OK if a node is found, E_FAIL if not
//
HRESULT FindRootNode
    (
    IScene *pScene, 
    INode **ppNode
    )
{
    CFindRootNode RootNode;

    // grab the first node of the scene graph
    pScene->EnumTree(&RootNode);

    *ppNode = RootNode.m_pNodeRoot;

    return RootNode.m_pNodeRoot != NULL ? S_OK : E_FAIL;
}

//	NEED EDIT :
//
HRESULT AddNormals
    (
    SSaveContext *psc,
    SMeshData *pMeshData, 
    BOOL bSwapTriOrder,
    LPDIRECTXFILEDATA pParent
    )
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

    assert(psc != NULL);
    assert(pParent != NULL);
    assert(pMeshData != NULL);

    cNormals = pMeshData->m_cVertices;
    cFaces = pMeshData->m_cFaces;

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
    for (iVertex = 0; iVertex < pMeshData->m_cVertices; iVertex++)
    {
		//	EDITED : 오른손좌표를 왼손좌표로.
		//
		if ( psc->m_bLeftHandAxis )
		{
			Point3 Vert;
			Vert.x = pMeshData->m_rgVertices[iVertex].vNormal.x;
			Vert.y = pMeshData->m_rgVertices[iVertex].vNormal.z;
			Vert.z = pMeshData->m_rgVertices[iVertex].vNormal.y;

			Vert = Normalize ( Vert );

			WRITE_POINT3 ( pbCur, Vert );
		}
		else
		{
			Point3 Vert;
			Vert.x = pMeshData->m_rgVertices[iVertex].vNormal.x;
			Vert.y = pMeshData->m_rgVertices[iVertex].vNormal.y;
			Vert.z = pMeshData->m_rgVertices[iVertex].vNormal.z;

			Vert = Normalize ( Vert );

			WRITE_POINT3 ( pbCur, Vert );
		}
    }

    // nFaces
    WRITE_DWORD(pbCur, cFaces);


    // MeshFace array
    for( iFace = 0; iFace < cFaces; iFace++ )
    {
        WRITE_DWORD(pbCur, 3); // nFaceVertexIndices (number of normal indices)

		//	EDITED : 
		//
        // faceVertexIndices (indices to normals - same as indices to vertices for us)
		if ( psc->m_bLeftHandAxis )
		{
			if( !bSwapTriOrder )
			{
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[0]);
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[2]);
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[1]);
			}
			else
			{
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[0]);
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[1]);
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[2]);
			}
		}
		else
		{
			if( bSwapTriOrder )
			{
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[0]);
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[2]);
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[1]);
			}
			else
			{
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[0]);
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[1]);
				WRITE_DWORD(pbCur, pMeshData->m_rgFaces[iFace].index[2]);
			}
		}
    }

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMeshNormals,
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
    RELEASE(pDataObject);
    delete []pbData;

    return hr;
}

HRESULT AddTextureCoordinates
    (
    SSaveContext *psc,
    SMeshData *pMeshData, 
    SCropInfo *rgCropInfo,
    Mesh* pMesh,
    LPDIRECTXFILEDATA pParent
    )
{
    LPDIRECTXFILEDATA pDataObject = NULL;
    PBYTE         pbData = NULL;
    PBYTE         pbCur = NULL;
    DWORD         cbSize;
    DWORD         cTexCoords;
    DWORD         iVertex;
    DWORD         iTextureIndex;
    HRESULT       hr = S_OK;
    float         fX;
    float         fY;
    DWORD         iMaterial;

    assert( psc );
    assert( pParent );
    assert( pMesh );
    assert( pMeshData );

    // if no tex coords, then don't add them
    if( !pMeshData->m_bTexCoordsPresent )
	{
        goto e_Exit;
	}

    cTexCoords = pMeshData->m_cVertices;

    cbSize = sizeof(DWORD) //nTextureCoords
             + cTexCoords*2*sizeof(float); //texture coords

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    WRITE_DWORD(pbCur, cTexCoords); //nTextureCoords

    for (iVertex = 0; iVertex < pMeshData->m_cVertices; iVertex++)
    {
        iTextureIndex  = pMeshData->m_rgVertices[iVertex].iTextureIndex;
        iMaterial = pMeshData->m_rgVertices[iVertex].iMaterial;
        if( iTextureIndex == 0xFFFFFFFF ) // none present, or bad data
        {
            WRITE_FLOAT(pbCur, 0); //u
            WRITE_FLOAT(pbCur, 0); //v
        }
        else
        {
            fX = pMesh->tVerts[iTextureIndex].x;
            fY = (1.0f - pMesh->tVerts[iTextureIndex].y);

			//	EDITED : 
			//
			//fX = (fX > 1.0f) ? 1.0f : ((fX < 0.0f) ? 0.0f : fX);
			//fY = (fY > 1.0f) ? 1.0f : ((fY < 0.0f) ? 0.0f : fY);

            fX = (fX * rgCropInfo[iMaterial].fClipW) + rgCropInfo[iMaterial].fClipU;
            fY = (fY * rgCropInfo[iMaterial].fClipH) + rgCropInfo[iMaterial].fClipV;

            WRITE_FLOAT(pbCur, fX); //u
            WRITE_FLOAT(pbCur, fY); //v
        }
    }

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMeshTextureCoords,
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
    RELEASE(pDataObject);

    delete []pbData;
    return hr;
}

//	NEED EDIT :
//
HRESULT AddPatchTextureCoordinates
    (
    SSaveContext *psc,
    SPatchMeshData* pPatchMeshData, 
    PatchMesh* pPatchMesh,
    LPDIRECTXFILEDATA pParent
    )
{
    LPDIRECTXFILEDATA pDataObject = NULL;
    PBYTE         pbData = NULL;
    PBYTE         pbCur = NULL;
    DWORD         cbSize;
    DWORD         cTexCoords;
    DWORD         iVertex;
    DWORD         iTextureIndex;
    HRESULT    hr = S_OK;

    assert( psc );
    assert( pParent );
    assert( pPatchMesh );
    assert( pPatchMeshData );

    // if no tex coords, then don't add them
    if( !pPatchMeshData->m_bTexCoordsPresent )
	{
        goto e_Exit;
	}

    cTexCoords = pPatchMeshData->m_cVertices;

    cbSize = sizeof(DWORD) //nTextureCoords
             + cTexCoords*2*sizeof(float); //texture coords

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    WRITE_DWORD(pbCur, cTexCoords); //nTextureCoords

    for (iVertex = 0; iVertex < pPatchMeshData->m_cVertices; iVertex++)
    {
        iTextureIndex  = pPatchMeshData->m_rgVertices[iVertex].iTextureIndex;
        if( iTextureIndex == 0xFFFFFFFF ) // none present, or bad data
        {
            WRITE_FLOAT(pbCur, 0); //u
            WRITE_FLOAT(pbCur, 0); //v
        }
        else
        {
            WRITE_FLOAT(pbCur, pPatchMesh->tVerts[1][iTextureIndex].p.x); //u // 29 jan 01 : rjc - changed from .x to .p.x for UVVert p
            WRITE_FLOAT(pbCur, pPatchMesh->tVerts[1][iTextureIndex].p.y * -1); //v // 29 jan 01 : rjc - changed from .y to .p.y for UVVert p
        }
    }

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMeshTextureCoords,
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
    RELEASE(pDataObject);

    delete []pbData;
    return hr;
}

HRESULT AddVertexDuplicationIndices
    (
    SSaveContext *psc,
    SMeshData *pMeshData, 
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT    hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    PBYTE         pbData = NULL;
    PBYTE         pbCur = NULL;
    DWORD         cbSize;
    DWORD         cIndices;
    DWORD         cVerticesBeforeDuplication;
    DWORD         iVertex;

    assert(psc != NULL);
    assert(pParent != NULL);
    assert(pMeshData != NULL);

    cIndices = pMeshData->m_cVertices;
    cVerticesBeforeDuplication = pMeshData->m_cVerticesBeforeDuplication;

	// if no new vertices, then don't add a record to the file
	if (pMeshData->m_cVerticesBeforeDuplication == pMeshData->m_cVertices)
		goto e_Exit;

    cbSize = sizeof(DWORD) //nIndices
             + sizeof(DWORD) //nVerticesBeforeDuplication
             + cIndices*sizeof(DWORD); // array of indices

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    WRITE_DWORD(pbCur, cIndices) // nIndices;
    WRITE_DWORD(pbCur, cVerticesBeforeDuplication) // nVerticesBeforeDuplication
    
    for (iVertex = 0; iVertex < cIndices; iVertex++)
    {
        WRITE_DWORD(pbCur, pMeshData->m_rgVertices[iVertex].iPointRep); // index to original vertex without duplication.
    }

    hr = psc->m_pxofsave->CreateDataObject(DXFILEOBJ_VertexDuplicationIndices,
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
    RELEASE(pDataObject);

    delete []pbData;
    return hr;
}

HRESULT AddPatchVertexDuplicationIndices
    (
    SSaveContext *psc,
    SPatchMeshData *pPatchMeshData, 
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT    hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    PBYTE         pbData = NULL;
    PBYTE         pbCur = NULL;
    DWORD         cbSize;
    DWORD         cIndices;
    DWORD         cVerticesBeforeDuplication;
    DWORD         iVertex;

    assert(psc != NULL);
    assert(pParent != NULL);
    assert(pPatchMeshData != NULL);

    cIndices = pPatchMeshData->m_cVertices;
    cVerticesBeforeDuplication = pPatchMeshData->m_cVerticesBeforeDuplication;

	// if no new vertices, then don't add a record to the file
	if (pPatchMeshData->m_cVerticesBeforeDuplication == pPatchMeshData->m_cVertices)
		goto e_Exit;

    cbSize = sizeof(DWORD) //nIndices
             + sizeof(DWORD) //nVerticesBeforeDuplication
             + cIndices*sizeof(DWORD); // array of indices

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    WRITE_DWORD(pbCur, cIndices) // nIndices;
    WRITE_DWORD(pbCur, cVerticesBeforeDuplication) // nVerticesBeforeDuplication
    
    for (iVertex = 0; iVertex < cIndices; iVertex++)
    {
        WRITE_DWORD(pbCur, pPatchMeshData->m_rgVertices[iVertex].iPointRep); // index to original vertex without duplication.
    }

    hr = psc->m_pxofsave->CreateDataObject(DXFILEOBJ_VertexDuplicationIndices,
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
    RELEASE(pDataObject);

    delete []pbData;
    return hr;
}

HRESULT
AddWireframeMaterial(
    SSaveContext *psc,
    INode *pNode,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT hr = S_OK;
    DWORD cbSize;
    LPDIRECTXFILEDATA pDataObject = NULL;
    PBYTE pbData = NULL;
    PBYTE pbCur;
    DWORD dwWFColor;
    D3DXCOLOR colorWF;

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
    
    // get the wireframe color
    dwWFColor = pNode->GetWireColor();
    dwWFColor |= 0xff000000;  // set alpha to fully opaque

    // convert to floating point
    colorWF = D3DXCOLOR(dwWFColor);

    //RGBA
    WRITE_COLOR(pbCur, colorWF);

    // Wireframe doesn't have an explicit specular power, so output our default.
    WRITE_FLOAT(pbCur, POWER_DEFAULT);

    // Set the specular color identical to diffuse, as recommended in 3DSMax docs.
    WRITE_FLOAT(pbCur, colorWF.r);
    WRITE_FLOAT(pbCur, colorWF.g);
    WRITE_FLOAT(pbCur, colorWF.b);

    // Set the luminence to 0: the material isn't glowing.
    WRITE_FLOAT(pbCur, 0.0f);
    WRITE_FLOAT(pbCur, 0.0f);
    WRITE_FLOAT(pbCur, 0.0f);

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMaterial,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    delete []pbData;
    RELEASE(pDataObject);

    return hr;
}

HRESULT
AddTextureFilename(
    SSaveContext *psc,
    TCHAR *szName,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    DWORD cbSize;

    cbSize = sizeof(TCHAR**);

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMTextureFilename,
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
    RELEASE(pDataObject);

    return hr;
}

// ================================================== FindTextureFileName
// callback called by 3DSMAX
class FindTextureFilename : public AssetEnumCallback
{
public:
    FindTextureFilename()
        : m_strTextureName( _T( "" ) )
    {
    }

    virtual void RecordAsset(const MaxSDK::AssetManagement::AssetUser& asset)
    {
        m_strTextureName = asset.GetFileName();
    }

    MSTR m_strTextureName;    
};

HRESULT
AddMaterial(
    SSaveContext *psc,
    Mtl *pMtl,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    PBYTE pbData = NULL;
    PBYTE pbCur;
    DWORD cbSize;
    TCHAR *szNiceFilename;
    Texmap *pTexMap;
    FindTextureFilename findTextureFilename;
    BOOL bDetailMap = FALSE;

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

    pTexMap = pMtl->GetSubTexmap(ID_DI);
    if ((pTexMap != NULL) && (pTexMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)))
    {
        bDetailMap = TRUE;
    }
    
    // 'Ambient color' is specified in Max docs as "the color of the object in shadows," and 
    //                 is usually a darker version of the diffuse color.
    // 'Diffuse color' is specified in the Max docs as "the color of the object in 
    //                  good lighting," usually referred to as the objects' color.
    // 'Specular color' is specified as the color of reflection highlights in direct lighting,
    //                  and according to Max docs is usually the same as diffuse.

    if (!bDetailMap)
    {
        // We're going to use the 'Diffuse' color as the object color for DirectX
        WRITE_POINT3(pbCur, pMtl->GetDiffuse());
        //Alpha
        WRITE_FLOAT(pbCur, 1 - pMtl->GetXParency());

        // 3DSMax specular power is comprised of shininess, and shininess strength.
        // TODO - figure out a mapping from shininess to power
        WRITE_FLOAT(pbCur, POWER_DEFAULT);

        // Specular
        WRITE_POINT3(pbCur, pMtl->GetSpecular());

        // Emmissive
        WRITE_POINT3(pbCur, pMtl->GetSelfIllumColor());

    }
    else  // if a detail map, then don't write the color
    {
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
    }

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMaterial,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
        goto e_Exit;

    // See if there is a valid bitmap texture
    if (bDetailMap)
    {
        pTexMap->EnumAuxFiles(findTextureFilename, FILE_ENUM_ALL);
        char* szTexFile = findTextureFilename.m_strTextureName.data();

        if (findTextureFilename.m_strTextureName.Length() <= 0 || !szTexFile)
        {
            OutputDebugString("AddMaterial: Bitmap texture found, but no texture name.  Skipping texture name\n");
            goto e_NoDetailMap;
        }

        if (psc->m_xFormat == DXFILEFORMAT_TEXT)
        {
            szNiceFilename = psc->m_stStrings.CreateNiceFilename(szTexFile); /*expand \ char to \\ */
			if (szNiceFilename == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}
/*
			if ( psc->m_bTexEncrypt )
			{
				//	텍스쳐 암호화 
				char szTextureOutName[MAX_PATH] = {0};

				strcpy( szTextureOutName, szNiceFilename );
				char* szTextureFileExt = strrchr ( szTextureOutName, '.' );
				if ( szTextureFileExt ) strcpy( szTextureFileExt, ".mtf" );

				EncryptTexture( szNiceFilename, szTextureOutName );
			}
*/
			//	EDITED : 텍스쳐 이름 수정.
			//
			char *szTextureFileName = strrchr ( szNiceFilename, '\\' );
			if ( szTextureFileName )
			{
				szTextureFileName = szTextureFileName + 1;	//	'\\' 부분 삭제.
			}
			else
			{
				szTextureFileName = szNiceFilename;
			}

			hr = AddTextureFilename(psc, szTextureFileName, pDataObject);
            if (FAILED(hr))
                goto e_Exit;
        }
        else
        {
/*
			if ( psc->m_bTexEncrypt )
			{
				//	텍스쳐 암호화 
				char szTextureOutName[MAX_PATH] = {0};

				strcpy( szTextureOutName, szTexFile );
				char* szTextureFileExt = strrchr ( szTextureOutName, '.' );
				if ( szTextureFileExt ) strcpy( szTextureFileExt, ".mtf" );

				EncryptTexture( szTexFile, szTextureOutName );
			}
*/

			//	EDITED : 텍스쳐 이름 수정.
			//
			char *szTextureFileName = strrchr ( szTexFile, '\\' );
			if ( szTextureFileName )
			{
				szTextureFileName = szTextureFileName + 1;	//	'\\' 부분 삭제.
			}
			else
			{
				szTextureFileName = szTexFile;
			}

			hr = AddTextureFilename(psc, szTextureFileName, pDataObject);
            if (FAILED(hr))
                goto e_Exit;
        }
    }
e_NoDetailMap:

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    delete []pbData;
    RELEASE(pDataObject);

    return hr;
}

HRESULT
AddMaterial(
    SSaveContext *psc,
    D3DXMATERIAL *pMaterial,
    LPDIRECTXFILEDATA pParent
    )
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
    
    // diffuse - write white RGBA
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Diffuse.r);
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Diffuse.g);
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Diffuse.b);
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Diffuse.a);

    // specular power
    WRITE_FLOAT(pbCur, POWER_DEFAULT);

    // specular - write white RGBA
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Specular.r);
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Specular.g);
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Specular.b);

    // emmissive - write white RGBA
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Emissive.r);
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Emissive.g);
    WRITE_FLOAT(pbCur, pMaterial->MatD3D.Emissive.b);

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMaterial,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
        goto e_Exit;

    // if there is a bitmap texture, then add the filename
    if (pMaterial->pTextureFilename != NULL)
    {
/*		
		if ( psc->m_bTexEncrypt )
		{
			// 텍스쳐 암호화 
			char szTextureOutName[MAX_PATH] = {0};

			strcpy( szTextureOutName, pMaterial->pTextureFilename );
			char* szTextureFileExt = strrchr ( szTextureOutName, '.' );
			if ( szTextureFileExt ) strcpy( szTextureFileExt, ".mtf" );

			EncryptTexture( pMaterial->pTextureFilename, szTextureOutName );
		}
*/
		//	EDITED : 텍스쳐 이름 수정.
		//
		char *szTextureFileName = strrchr ( pMaterial->pTextureFilename, '\\' );
		if ( szTextureFileName )	
		{
			szTextureFileName = szTextureFileName + 1;	//	'\\' 부분 삭제.
		}
		else
		{
			szTextureFileName = pMaterial->pTextureFilename;
		}

		hr = AddTextureFilename(psc, szTextureFileName, pDataObject);
        if (FAILED(hr))
            goto e_Exit;
    }

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    delete []pbData;
    RELEASE(pDataObject);

    return hr;
}

HRESULT
GatherMeshMaterials(
    SSaveContext *psc,
    INode *pNode,
    Mesh *pMesh,
    DWORD **prgdwMeshMaterials,
    D3DXMATERIAL **prgMaterials,
    SCropInfo **prgCropInfo,
    DWORD *pcMaterials
    )
{
    HRESULT hr = S_OK;
    Mtl *pMtlMain;
    Mtl *pMtlCur;
    DWORD iCurMaterial;
    DWORD cSubMaterials;
    DWORD iFace;
    DWORD cFaces;
    BOOL bNoSubMaterials;
    BOOL bWireframeColor;
    DWORD dwCurMatID;
    DWORD *rgdwMeshMaterials = NULL;
    SCropInfo *rgCropInfo = NULL;
    IParamBlock2 *pParamBlock;
    Texmap *pTexMap;
    D3DXMATERIAL *rgMaterials = NULL;
    TCHAR *szFilename;
    DWORD dwWFColor;
    D3DXCOLOR colorWF;
    BOOL bDetailMap;
    FindTextureFilename findTextureFilename;

    pMtlMain = pNode->GetMtl();
    cFaces = pMesh->getNumFaces();
    cSubMaterials = 0;
    bNoSubMaterials = FALSE;
    bWireframeColor = FALSE;

    rgdwMeshMaterials = new DWORD[cFaces];
    if (rgdwMeshMaterials == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // The color of a given mesh can be provided by three different sources:
    //   1) applied texture maps, as part of a material
    //   2) explicitly defined & applied materials without texture maps
    //   3) a 'wireframe' color.
    
    // For our purposes, we will output these in this order of preference, ignoring
    // processing past the first one we find.

    if (pMtlMain != NULL)
    {
        // There is at least one material. We're in case 1) or 2)

        cSubMaterials = pMtlMain->NumSubMtls();
        if (cSubMaterials < 1)
        {
            // Count the material itself as a submaterial.
            cSubMaterials = 1;
            bNoSubMaterials = TRUE;
        }
    }
    else  // no material, then we'll create a material corresponding to the default wire color.
    {
        bWireframeColor = TRUE;
        cSubMaterials = 1;
    }

    for (iFace=0; iFace < cFaces; iFace++)
    {
        if (bWireframeColor || bNoSubMaterials) 
        {
            // If we're using wireframe color, it's our only material
            rgdwMeshMaterials[iFace] = 0;
        }
        else
        {
            // Otherwise we have at least one material to process.

            dwCurMatID = pMesh->faces[iFace].getMatID();

            if (cSubMaterials == 1)
            {
                dwCurMatID = 0;
            }
            else
            {
                // SDK recommends mod'ing the material ID by the valid # of materials, 
                // as sometimes a material number that's too high is returned.
                dwCurMatID %= cSubMaterials;
            }

            // output the appropriate material color

            rgdwMeshMaterials[iFace] = dwCurMatID;

        } 

    } 

    rgCropInfo = new SCropInfo[cSubMaterials];
    rgMaterials = new D3DXMATERIAL[cSubMaterials];
    if ((rgCropInfo == NULL) || (rgMaterials == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(rgMaterials, 0, sizeof(D3DXMATERIAL) * cSubMaterials);

    for (iCurMaterial = 0; iCurMaterial < cSubMaterials; iCurMaterial++)
    {
        rgCropInfo[iCurMaterial].fClipH = 0.0f;
        rgCropInfo[iCurMaterial].fClipV = 0.0f;
        rgCropInfo[iCurMaterial].fClipW = 1.0f;
        rgCropInfo[iCurMaterial].fClipH = 1.0f;
    }

    if (!bWireframeColor)
    {
        // 3DSMax allows multiple materials to be used on a single mesh via
        // 'submaterials'. When the first submaterial is defined, the main material
        // is copied into submaterial #1, and the new submaterial because submaterial #2.
        // 
        // We have to catch the case where there's a material, but no submaterials. For this
        // case, set NumSubMaterials to 1 which would never happen otherwise. It's imperative
        // that the first material be set to MtlMain, rather than trying to GetSubMtl() to 
        // allow this logic to work.

        // Loop through the materials (if any) and output them.

        // first collect the crop info and the list of materials
        for (iCurMaterial = 0; iCurMaterial < cSubMaterials; iCurMaterial++)
        {
            if (bNoSubMaterials)
            {
                // We're on the parent material, possibly the ONLY material.
                // We won't be able to get it with GetSubMtl() if it's the only one, and
                // the data in the first submaterial is identical to the main material,
                // so just use the main material.

                pMtlCur = pMtlMain;
            }
            else
            {
                // We're into the true submaterials. Safe to get with 'GetSubMtl'

                pMtlCur = pMtlMain->GetSubMtl(iCurMaterial);
            }

            // if using texture as a detail map, get the crop info
			bDetailMap = FALSE;
            pTexMap = pMtlCur->GetSubTexmap(ID_DI);
            if ((pTexMap != NULL) && (pTexMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)))
            {
                pParamBlock = pTexMap->GetParamBlock(0);

                if (pParamBlock != NULL)
                {
                    pParamBlock->GetValue(bmtex_clipu, 0, rgCropInfo[iCurMaterial].fClipU, FOREVER);
                    pParamBlock->GetValue(bmtex_clipv, 0, rgCropInfo[iCurMaterial].fClipV, FOREVER);
                    pParamBlock->GetValue(bmtex_clipw, 0, rgCropInfo[iCurMaterial].fClipW, FOREVER);
                    pParamBlock->GetValue(bmtex_cliph, 0, rgCropInfo[iCurMaterial].fClipH, FOREVER);       
                }

                bDetailMap = TRUE;
            }

            // 'Ambient color' is specified in Max docs as "the color of the object in shadows," and 
            //                 is usually a darker version of the diffuse color.
            // 'Diffuse color' is specified in the Max docs as "the color of the object in 
            //                  good lighting," usually referred to as the objects' color.
            // 'Specular color' is specified as the color of reflection highlights in direct lighting,
            //                  and according to Max docs is usually the same as diffuse.

            if (!bDetailMap)
            {
                // We're going to use the 'Diffuse' color as the object color for DirectX
                rgMaterials[iCurMaterial].MatD3D.Diffuse.r = pMtlCur->GetDiffuse().r;
                rgMaterials[iCurMaterial].MatD3D.Diffuse.g = pMtlCur->GetDiffuse().g;
                rgMaterials[iCurMaterial].MatD3D.Diffuse.b = pMtlCur->GetDiffuse().b;

                //Alpha
                rgMaterials[iCurMaterial].MatD3D.Diffuse.a = 1.0f - pMtlCur->GetXParency();

                // 3DSMax specular power is comprised of shininess, and shininess strength.
                // TODO - figure out a mapping from shininess to power
                rgMaterials[iCurMaterial].MatD3D.Power = POWER_DEFAULT;

                // Specular
                rgMaterials[iCurMaterial].MatD3D.Specular.r = pMtlCur->GetSpecular().r;
                rgMaterials[iCurMaterial].MatD3D.Specular.g = pMtlCur->GetSpecular().g;
                rgMaterials[iCurMaterial].MatD3D.Specular.b = pMtlCur->GetSpecular().b;
                rgMaterials[iCurMaterial].MatD3D.Specular.a = 1.0f;

                // Emmissive
                rgMaterials[iCurMaterial].MatD3D.Emissive.r = pMtlCur->GetSelfIllumColor().r;
                rgMaterials[iCurMaterial].MatD3D.Emissive.g = pMtlCur->GetSelfIllumColor().g;
                rgMaterials[iCurMaterial].MatD3D.Emissive.b = pMtlCur->GetSelfIllumColor().b;
                rgMaterials[iCurMaterial].MatD3D.Emissive.a = 1.0f;
            }
            else  // if a detail map, then don't write the color
            {
                // diffuse - write white RGBA
                rgMaterials[iCurMaterial].MatD3D.Diffuse.r = 1.0f;
                rgMaterials[iCurMaterial].MatD3D.Diffuse.g = 1.0f;
                rgMaterials[iCurMaterial].MatD3D.Diffuse.b = 1.0f;
                rgMaterials[iCurMaterial].MatD3D.Diffuse.a = 1.0f;

                rgMaterials[iCurMaterial].MatD3D.Power = POWER_DEFAULT;

                // specular - write white RGBA
                rgMaterials[iCurMaterial].MatD3D.Specular.r = 1.0f;
                rgMaterials[iCurMaterial].MatD3D.Specular.g = 1.0f;
                rgMaterials[iCurMaterial].MatD3D.Specular.b = 1.0f;
                rgMaterials[iCurMaterial].MatD3D.Specular.a = 1.0f;

                // emmissive - write white RGBA
                rgMaterials[iCurMaterial].MatD3D.Emissive.r = 0.0f;
                rgMaterials[iCurMaterial].MatD3D.Emissive.g = 0.0f;
                rgMaterials[iCurMaterial].MatD3D.Emissive.b = 0.0f;
                rgMaterials[iCurMaterial].MatD3D.Emissive.a = 1.0f;
            }

            if (bDetailMap)
            {
                pTexMap->EnumAuxFiles(findTextureFilename, FILE_ENUM_ALL);
                char* szTexFile = findTextureFilename.m_strTextureName.data();

                if (findTextureFilename.m_strTextureName.Length() <= 0 || !szTexFile)
                {
                    OutputDebugString("AddMaterial: Bitmap texture found, but no texture name\n");
                    szFilename = NULL;
                }
                else // texture filename found
                {
                    // allocate a new string, doing the '\' fixup if neccessary
                    if (psc->m_xFormat == DXFILEFORMAT_TEXT)
                        szFilename = psc->m_stStrings.CreateNiceFilename(szTexFile); /*expand \ char to \\ */
                    else
                        szFilename = psc->m_stStrings.CreateNormalFilename(szTexFile); /*DON'T! expand \ char to \\ */

			        if (szFilename == NULL)
			        {
				        hr = E_OUTOFMEMORY;
				        goto e_Exit;
			        }
                }

                rgMaterials[iCurMaterial].pTextureFilename = szFilename;
            }
        } 
    }
    else // wireframe color
    {
       // get the wireframe color
        dwWFColor = pNode->GetWireColor();
        dwWFColor |= 0xff000000;  // set alpha to fully opaque

        // convert to floating point
        colorWF = D3DXCOLOR(dwWFColor);

        //RGBA
        rgMaterials[0].MatD3D.Diffuse.r = colorWF.r;
        rgMaterials[0].MatD3D.Diffuse.g = colorWF.g;
        rgMaterials[0].MatD3D.Diffuse.b = colorWF.b;
        rgMaterials[0].MatD3D.Diffuse.a = 1.0f;

        // Wireframe doesn't have an explicit specular power, so output our default.
        rgMaterials[0].MatD3D.Power = POWER_DEFAULT;

        // Set the specular color identical to diffuse, as recommended in 3DSMax docs.
        rgMaterials[0].MatD3D.Specular.r = colorWF.r;
        rgMaterials[0].MatD3D.Specular.g = colorWF.g;
        rgMaterials[0].MatD3D.Specular.b = colorWF.b;
        rgMaterials[0].MatD3D.Specular.a = 1.0f;

        // Set the luminence to 0: the material isn't glowing.
        rgMaterials[0].MatD3D.Emissive.r = 0.0f;
        rgMaterials[0].MatD3D.Emissive.g = 0.0f;
        rgMaterials[0].MatD3D.Emissive.b = 0.0f;
        rgMaterials[0].MatD3D.Emissive.a = 1.0f;

    }

    *prgdwMeshMaterials = rgdwMeshMaterials;
    rgdwMeshMaterials = NULL;

    *prgCropInfo = rgCropInfo;
    rgCropInfo = NULL;

    *prgMaterials = rgMaterials;
    rgMaterials = NULL;

    *pcMaterials = cSubMaterials;

e_Exit:
    delete []rgdwMeshMaterials;
    delete []rgMaterials;
    delete []rgCropInfo;
    return hr;
}

HRESULT
AddMeshMaterials(
    SSaveContext *psc,
    INode *pNode,
    Mesh *pMesh,
    DWORD *rgdwMeshMaterials,
    D3DXMATERIAL *rgMaterials,
    DWORD cMaterials,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    DWORD iFace;
    DWORD cFaces;
    PBYTE pbCur;
    PBYTE pbData = NULL;
    DWORD cbSize;
    DWORD iCurMaterial;
    BOOL bFound;
    DWORD cUniqueMaterials;
    DWORD iSearchMaterial;
    DWORD iPrevMaterial;
    DWORD iDestMaterial;
    DWORD *rgdwMaterialRemap = NULL;

    cFaces = pMesh->getNumFaces();

    rgdwMaterialRemap = new DWORD[cMaterials];
    if (rgdwMaterialRemap == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // first go through and remove redundant materials (might not have had the same crop info!)
    cUniqueMaterials = 1;
    rgdwMaterialRemap[0] = 0;
    for (iCurMaterial = 1; iCurMaterial < cMaterials; iCurMaterial++)
    {
        bFound = FALSE;
        for (iSearchMaterial = 0; iSearchMaterial < iCurMaterial; iSearchMaterial++)
        {
            if (iSearchMaterial == iCurMaterial)
                continue;

			//	EDITED :  오류 수정.
			//

            // if both the material and texture name are the same, then it is a redundant material
            if ( (memcmp(&(rgMaterials[iCurMaterial].MatD3D), &(rgMaterials[iSearchMaterial].MatD3D), sizeof(D3DMATERIAL9)) == 0) )
			{
				if ( rgMaterials[iCurMaterial].pTextureFilename && rgMaterials[iSearchMaterial].pTextureFilename )
				{
					if ( (strcmp(rgMaterials[iCurMaterial].pTextureFilename, rgMaterials[iSearchMaterial].pTextureFilename) == 0) )
					{
						bFound = TRUE;
						break;
					}
				}
				else if ( !rgMaterials[iCurMaterial].pTextureFilename && !rgMaterials[iSearchMaterial].pTextureFilename )
				{
					bFound = TRUE;
					break;
				}
			}
        }

        // if found, just remap to the material that was found
        if (bFound)
        {
            rgdwMaterialRemap[iCurMaterial] = rgdwMaterialRemap[iSearchMaterial];
        }
        else  // not found, another unique material
        {
            rgdwMaterialRemap[iCurMaterial] = cUniqueMaterials;
            cUniqueMaterials += 1;
        }
    }

    // now remap the materials
    iPrevMaterial = 0;
    for (iCurMaterial = 1; iCurMaterial < cMaterials; iCurMaterial++)
    {
        iDestMaterial = rgdwMaterialRemap[iCurMaterial];

        // if a unique material, then move it
        if (iDestMaterial > iPrevMaterial)
        {
            iPrevMaterial += 1;
            assert(iDestMaterial == iPrevMaterial);

            // if not staying in the same place, then copy it
            if (iCurMaterial != iDestMaterial)
            {
                memcpy(&rgMaterials[iDestMaterial], &rgMaterials[iCurMaterial], sizeof(D3DXMATERIAL));
            }
        }
    }


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
        WRITE_DWORD(pbCur, rgdwMaterialRemap[rgdwMeshMaterials[iFace]];);
    } 

    // now finally create the mesh material list
    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMeshMaterialList,
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
        hr = AddMaterial(psc, &rgMaterials[iCurMaterial], pDataObject);
        if (FAILED(hr))
            goto e_Exit;
    } 

e_Exit:
    delete []pbData;
    delete []rgdwMaterialRemap;
    RELEASE(pDataObject);


    return hr;
} 

// structe used by AddSkinData to convert skin data from per vertex to per bone
struct SBoneState
{
    SBoneState()
        :m_pbData(NULL), m_szBoneName(NULL) {}
    ~SBoneState()
        { 
            delete []m_pbData; 

			// Bone name is allocated out of a string pool because it has to be deleted after
			//   the data has been saved to disk
            //delete []m_szBoneName; 
        }

    // info computed up front (note m_rgdwIndices, m_rgfWeights and m_pmatOFfset point into pbData)
    INode *m_pBoneNode;
    DWORD m_cVertices;
    DWORD m_cbData;
    PBYTE m_pbData;
    char *m_szBoneName;
    DWORD *m_rgdwIndices;
    float *m_rgfWeights;
    D3DXMATRIX *m_pmatOffset;

    // current index of last added vertex index
    DWORD m_iCurIndex;
};

SBoneState *FindBoneData
    (
    INode *pBoneNode, 
    SBoneState *rgbsBoneData, 
    DWORD cBones
    )
{
    DWORD iBone;
    SBoneState *pbs = NULL;

    for (iBone = 0; iBone < cBones; iBone++)
    {
        if (rgbsBoneData[iBone].m_pBoneNode == pBoneNode)
        {
            pbs = &rgbsBoneData[iBone];
            break;
        }
    }

    return pbs;
}

HRESULT AddSkinData
	(
		SSaveContext *psc,
		INode *pNode,
		SMeshData *pMeshData,
		LPDIRECTXFILEDATA pParent
	)
{
	HRESULT hr = S_OK;
	IPhyVertexExport *pVertexExport;
	IPhyRigidVertex* pRigidVertexExport;
	IPhyBlendedRigidVertex *pBlendedRigidVertexExport;
	INode* pBoneNode;
	SSkinMap *psmSkinMap = NULL;
	Modifier* pPhyMod = NULL;
    Modifier* pSkinMod = NULL;
	IPhysiqueExport* pPhyExport = NULL;
	IPhyContextExport* pPhyContextExport = NULL;
	SBoneState *rgbsBoneData = NULL;
	SBoneState *rgbsBoneDataDups = NULL;
	SBoneState *pbsCurData = NULL;
	SBoneState *pbsOldData = NULL;
	DWORD iVertex;
	DWORD cVertices;
	DWORD iVertexType;
	DWORD cTotalBones;
	DWORD iBone;
	PBYTE pbCur;
	float fWeight;
	LPDIRECTXFILEDATA pDataObject = NULL;
	DWORD iIndex;
	DWORD iNextWedge;
	Matrix3 matMesh;
	Matrix3 matOffset;
	Matrix3 matZero;
	PBYTE pbHeaderData = NULL;
	DWORD cbHeaderData;
	static bool shownError = false;

	matZero.Zero();
	pPhyMod = FindPhysiqueModifier(pNode);
	if (pPhyMod != NULL)
	{
		// Get a Physique Export interface
		pPhyExport = (IPhysiqueExport *)pPhyMod->GetInterface(I_PHYINTERFACE);
		if (pPhyExport == NULL)
		{
			// not all interfaces present, so ignore
			goto e_NoPhysique;
		}

		// get the init matrix for the mesh (used to mult by the bone matrices)
		int rval = pPhyExport->GetInitNodeTM(pNode, matMesh);
		if (rval || matMesh.Equals(matZero, 0.0))
		{
			if (!shownError)
			{
				shownError = true;
				MessageBox(NULL, "This mesh was skinned using an older version of Character Studio. Mesh may not be exported correctly",
						"X File export Warning", MB_OK);
			}
		}

		// For a given Object's INode get a
		// ModContext Interface from the Physique Export Interface:
		pPhyContextExport = (IPhyContextExport *)pPhyExport->GetContextInterface(pNode);
		if(pPhyContextExport == NULL)
		{
			// not all interfaces present, so ignore
			goto e_NoPhysique;
		}

		// should already have been converted to rigid with blending by Preprocess
		// convert to rigid with blending
		pPhyContextExport->ConvertToRigid(TRUE);
		pPhyContextExport->AllowBlending(TRUE);

		psmSkinMap = psc->GetSkinMap(pNode);
		if (psmSkinMap == NULL)
		{
			OutputDebugString("AddSkinData: Found physique info at save time, but not preprocess\n");
			hr = E_FAIL;
			goto e_Exit;
		}

		// now setup state used to fill arrays for output 
		rgbsBoneData = new SBoneState[psmSkinMap->m_cbiBones];
		if (rgbsBoneData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// intialize each of the bone structures 
		for (iBone = 0; iBone < psmSkinMap->m_cbiBones; iBone++)
		{
			pbsCurData = &rgbsBoneData[iBone];

			pbsCurData->m_iCurIndex = 0;
			pbsCurData->m_cVertices = psmSkinMap->m_rgbiBones[iBone].m_cVertices;
			pbsCurData->m_pBoneNode = psmSkinMap->m_rgbiBones[iBone].m_pBoneNode;

			// allocate memory to pass to D3DXOF lib
			pbsCurData->m_cbData = sizeof(TCHAR*)
									+ sizeof(DWORD) // numWeights
									+ sizeof(DWORD) * pbsCurData->m_cVertices // array of vertex indices
									+ sizeof(float) * pbsCurData->m_cVertices // array of weights
									+ sizeof(float) * 16; // offset matrix

			pbCur = pbsCurData->m_pbData = new BYTE[pbsCurData->m_cbData];
			if (pbsCurData->m_pbData == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			// fill first few entries, and remember pointers to the other arrays/matrix
			WRITE_PTCHAR(pbCur, pbsCurData->m_szBoneName);
			WRITE_DWORD(pbCur, pbsCurData->m_cVertices);

			pbsCurData->m_rgdwIndices = (DWORD*)pbCur;
			pbCur += sizeof(DWORD) * pbsCurData->m_cVertices;

			pbsCurData->m_rgfWeights = (float*)pbCur;
			pbCur += sizeof(float) * pbsCurData->m_cVertices;

			pbsCurData->m_pmatOffset = (D3DXMATRIX*)pbCur;

			// compute the mat offset
			int rval = pPhyExport->GetInitNodeTM(pbsCurData->m_pBoneNode, matOffset);
			if (rval)
			{
				MessageBox(NULL, "This mesh was skinned using an older version of Character Studio. Mesh may not be exported correctly",
						"X File export Warning", MB_OK);
			}
			
			//	EDITED :
			//
			if ( psc->m_bLeftHandAxis )
			{
				Matrix3 matOffsetLHTM, matMeshLH;
				RHMatToLHMat ( matOffset, matOffsetLHTM );
				RHMatToLHMat ( matMesh, matMeshLH );

				matOffsetLHTM.Invert();
				matOffsetLHTM = matMeshLH * matOffsetLHTM;

				WRITE_MATRIX4_FROM_MATRIX3(pbCur, matOffsetLHTM);
			}
			else
			{
				matOffset.Invert();
				matOffset = matMesh * matOffset;

				WRITE_MATRIX4_FROM_MATRIX3(pbCur, matOffset);
			}
		}

		cVertices = pPhyContextExport->GetNumberVertices();
		for (iVertex = 0; iVertex < cVertices; iVertex++ )
		{
			pVertexExport = (IPhyVertexExport *)pPhyContextExport->GetVertexInterface(iVertex);    
			if (pVertexExport == NULL)
			{
				OutputDebugString("Couldn't get export interface!");
				hr = E_FAIL;
				goto e_Exit;
			}

			// What kind of vertices are these?
			iVertexType = pVertexExport->GetVertexType();

			pPhyContextExport->ReleaseVertexInterface( pVertexExport );    

			if( iVertexType == RIGID_TYPE )
			{
				pRigidVertexExport = (IPhyRigidVertex *)pPhyContextExport->GetVertexInterface(iVertex);
				if (pRigidVertexExport == NULL)
				{
					OutputDebugString("Couldn't get rigid vertex export interface!");
					hr = E_FAIL;
					goto e_Exit;
				}

				// Get the vertex info!
				pBoneNode = pRigidVertexExport->GetNode();

				pbsCurData = FindBoneData(pBoneNode, rgbsBoneData, psmSkinMap->m_cbiBones);
				if (pbsCurData == NULL)
				{
					assert(0);
					OutputDebugString("AddSkinData: Bone node not found on second pass\n");
					hr = E_FAIL;
					goto e_Exit;
				}

				pbsCurData->m_rgdwIndices[pbsCurData->m_iCurIndex] = iVertex;
				pbsCurData->m_rgfWeights[pbsCurData->m_iCurIndex] = 1.0f;
				pbsCurData->m_iCurIndex += 1;

				pPhyContextExport->ReleaseVertexInterface( pRigidVertexExport);
			}
			else
			{
				assert( iVertexType == RIGID_BLENDED_TYPE );

				pBlendedRigidVertexExport = (IPhyBlendedRigidVertex *)pPhyContextExport->GetVertexInterface(iVertex);
				if (pBlendedRigidVertexExport == NULL)
				{
					OutputDebugString("Couldn't get blended rigid vertex export interface!");
					hr = E_FAIL;
					goto e_Exit;
				}

				// How many nodes affect his vertex?
				cTotalBones = pBlendedRigidVertexExport->GetNumberNodes();
				for (iBone = 0; iBone < cTotalBones; iBone++ )
				{
					pBoneNode = pBlendedRigidVertexExport->GetNode(iBone);

					pbsCurData = FindBoneData(pBoneNode, rgbsBoneData, psmSkinMap->m_cbiBones);
					if (pbsCurData == NULL)
					{
						assert(0);
						OutputDebugString("AddSkinData: Bone node not found on second pass\n");
						hr = E_FAIL;
						goto e_Exit;
					}

					fWeight = pBlendedRigidVertexExport->GetWeight(iBone);

					// first see if it is a repeat weight, is so just add to previous
					if ((pbsCurData->m_iCurIndex > 0) && (pbsCurData->m_rgdwIndices[pbsCurData->m_iCurIndex-1] == iVertex))
					{
						pbsCurData->m_rgfWeights[pbsCurData->m_iCurIndex-1] += fWeight;
					}
					else
					{
						pbsCurData->m_rgdwIndices[pbsCurData->m_iCurIndex] = iVertex;
						pbsCurData->m_rgfWeights[pbsCurData->m_iCurIndex] = fWeight;
						pbsCurData->m_iCurIndex += 1;
					}                    
				}

				pPhyContextExport->ReleaseVertexInterface( pBlendedRigidVertexExport);
			}
		}
	}

e_NoPhysique:

    pSkinMod = FindSkinModifier( pNode );
	if( pSkinMod )
	{
        ISkin* pSkin = (ISkin*)pSkinMod->GetInterface( I_SKIN );
        if( !pSkin )
            goto e_NoSkin;

        if( pSkin->GetNumBones() <= 0 )
            goto e_NoSkin;

        ISkinContextData* pSkinData = (ISkinContextData*)pSkin->GetContextInterface( pNode );
        if( !pSkinData )
            goto e_NoSkin;

        int rval = pSkin->GetSkinInitTM( pNode, matMesh );
        if( rval != SKIN_OK || matMesh.Equals( matZero, 0.0f ) )
        {
            if( !shownError )
            {
                shownError = true;
                MessageBox( NULL, "pSkin->GetSkinInitTM", "Export Error", MB_OK );
            }
        }

        psmSkinMap = psc->GetSkinMap( pNode );
        if( !psmSkinMap )
        {
            OutputDebugString("AddSkinData: Found skin info at save time, but not preprocess\n");
			hr = E_FAIL;
			goto e_Exit;
        }

        rgbsBoneData = new SBoneState[ psmSkinMap->m_cbiBones ];
		if( !rgbsBoneData )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

        for (iBone = 0; iBone < psmSkinMap->m_cbiBones; iBone++)
		{
			pbsCurData = &rgbsBoneData[iBone];

			pbsCurData->m_iCurIndex = 0;
			pbsCurData->m_cVertices = psmSkinMap->m_rgbiBones[iBone].m_cVertices;
			pbsCurData->m_pBoneNode = psmSkinMap->m_rgbiBones[iBone].m_pBoneNode;

			// allocate memory to pass to D3DXOF lib
			pbsCurData->m_cbData = sizeof(TCHAR*)
									+ sizeof(DWORD) // numWeights
									+ sizeof(DWORD) * pbsCurData->m_cVertices // array of vertex indices
									+ sizeof(float) * pbsCurData->m_cVertices // array of weights
									+ sizeof(float) * 16; // offset matrix

			pbCur = pbsCurData->m_pbData = new BYTE[pbsCurData->m_cbData];
			if (pbsCurData->m_pbData == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			// fill first few entries, and remember pointers to the other arrays/matrix
			WRITE_PTCHAR(pbCur, pbsCurData->m_szBoneName);
			WRITE_DWORD(pbCur, pbsCurData->m_cVertices);

			pbsCurData->m_rgdwIndices = (DWORD*)pbCur;
			pbCur += sizeof(DWORD) * pbsCurData->m_cVertices;

			pbsCurData->m_rgfWeights = (float*)pbCur;
			pbCur += sizeof(float) * pbsCurData->m_cVertices;

			pbsCurData->m_pmatOffset = (D3DXMATRIX*)pbCur;

			// compute the mat offset
            int rval = pSkin->GetBoneInitTM( pbsCurData->m_pBoneNode, matOffset );
            if ( rval != SKIN_OK )
            {
                if( !shownError )
                {
                    shownError = true;
                    MessageBox( NULL, "pSkin->GetBoneInitTM", "Export Error", MB_OK );
                }
            }

			//	EDITED :
			//
			if ( psc->m_bLeftHandAxis )
			{
				Matrix3 matOffsetLHTM, matMeshLH;
				RHMatToLHMat ( matOffset, matOffsetLHTM );
				RHMatToLHMat ( matMesh, matMeshLH );

				matOffsetLHTM.Invert();
				matOffsetLHTM = matMeshLH * matOffsetLHTM;

				WRITE_MATRIX4_FROM_MATRIX3(pbCur, matOffsetLHTM);
			}
			else
			{
				matOffset.Invert();
				matOffset = matMesh * matOffset;

				WRITE_MATRIX4_FROM_MATRIX3(pbCur, matOffset);
			}
		}

        cVertices = pSkinData->GetNumPoints();
		for (iVertex = 0; iVertex < cVertices; iVertex++ )
		{
			// How many nodes affect his vertex?
			cTotalBones = pSkinData->GetNumAssignedBones( iVertex );
            if( cTotalBones > 4 )
                continue;

			for (iBone = 0; iBone < cTotalBones; iBone++ )
			{
                int	boneNr = pSkinData->GetAssignedBone( iVertex, iBone );
                if( boneNr < 0 )
	                continue;

                pBoneNode = pSkin->GetBone( boneNr );
                if( !pBoneNode )
                    continue;

				pbsCurData = FindBoneData(pBoneNode, rgbsBoneData, psmSkinMap->m_cbiBones);
				if (pbsCurData == NULL)
				{
					assert(0);
					OutputDebugString("AddSkinData: Bone node not found on second pass\n");
					hr = E_FAIL;
					goto e_Exit;
				}

				fWeight = pSkinData->GetBoneWeight( iVertex, iBone );

				// first see if it is a repeat weight, is so just add to previous
				if ((pbsCurData->m_iCurIndex > 0) && (pbsCurData->m_rgdwIndices[pbsCurData->m_iCurIndex-1] == iVertex))
				{
					pbsCurData->m_rgfWeights[pbsCurData->m_iCurIndex-1] += fWeight;
				}
				else
				{
					pbsCurData->m_rgdwIndices[pbsCurData->m_iCurIndex] = iVertex;
					pbsCurData->m_rgfWeights[pbsCurData->m_iCurIndex] = fWeight;
					pbsCurData->m_iCurIndex += 1;
				}                    
			}
		}

e_NoSkin:
        pSkinMod->ReleaseInterface( I_SKIN, pSkin );
    }

	if (rgbsBoneData != NULL)
	{
		assert(psmSkinMap != NULL);

		// now deal with the wonderful world of duplicated indices
		rgbsBoneDataDups = new SBoneState[psmSkinMap->m_cbiBones];
		if (rgbsBoneDataDups == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// first calculate new lengths for the bone weight arrays
		for (iBone = 0; iBone < psmSkinMap->m_cbiBones; iBone++)
		{
			pbsOldData = &rgbsBoneData[iBone];
			pbsCurData = &rgbsBoneDataDups[iBone];

			pbsCurData->m_cVertices = pbsOldData->m_cVertices;
			for (iIndex = 0; iIndex < pbsOldData->m_cVertices; iIndex++)
			{
				iVertex = pbsOldData->m_rgdwIndices[iIndex];

				iNextWedge = pMeshData->m_rgVertices[iVertex].iWedgeList;
				while (iVertex != iNextWedge)
				{
					pbsCurData->m_cVertices += 1;

					iNextWedge = pMeshData->m_rgVertices[iNextWedge].iWedgeList;
				}
			}
		}

		// next build 
		for (iBone = 0; iBone < psmSkinMap->m_cbiBones; iBone++)
		{
			pbsOldData = &rgbsBoneData[iBone];
			pbsCurData = &rgbsBoneDataDups[iBone];

			pbsCurData->m_pBoneNode = pbsOldData->m_pBoneNode;
			pbsCurData->m_iCurIndex = 0;

			// allocate memory to pass to D3DXOF lib
			pbsCurData->m_cbData = sizeof(TCHAR*)
									+ sizeof(DWORD) // numWeights
									+ sizeof(DWORD) * pbsCurData->m_cVertices // array of vertex indices
									+ sizeof(float) * pbsCurData->m_cVertices // array of weights
									+ sizeof(float) * 16; // offset matrix

			pbCur = pbsCurData->m_pbData = new BYTE[pbsCurData->m_cbData];
			pbsCurData->m_szBoneName = psc->m_stStrings.CreateNiceString(pbsCurData->m_pBoneNode->GetName());
			if ((pbsCurData->m_pbData == NULL) || (pbsCurData->m_szBoneName == NULL))
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			// fill first few entries, and remember pointers to the other arrays/matrix
			WRITE_PTCHAR(pbCur, pbsCurData->m_szBoneName);
			WRITE_DWORD(pbCur, pbsCurData->m_cVertices);

			pbsCurData->m_rgdwIndices = (DWORD*)pbCur;
			pbCur += sizeof(DWORD) * pbsCurData->m_cVertices;

			pbsCurData->m_rgfWeights = (float*)pbCur;
			pbCur += sizeof(float) * pbsCurData->m_cVertices;

			pbsCurData->m_pmatOffset = (D3DXMATRIX*)pbCur;

			//	EDITED :
			//
			{
			}
	
			// already loaded above, copy from the original state data
			*pbsCurData->m_pmatOffset = *pbsOldData->m_pmatOffset;


			// now that we have the new states set up, copy the data from the old states
			for (iIndex = 0; iIndex < pbsOldData->m_cVertices; iIndex++)
			{
				iVertex = pbsOldData->m_rgdwIndices[iIndex];

				pbsCurData->m_rgdwIndices[pbsCurData->m_iCurIndex] = iVertex;
				pbsCurData->m_rgfWeights[pbsCurData->m_iCurIndex] = pbsOldData->m_rgfWeights[iIndex];
				pbsCurData->m_iCurIndex += 1;

				iNextWedge = pMeshData->m_rgVertices[iVertex].iWedgeList;
				while (iVertex != iNextWedge)
				{
					pbsCurData->m_rgdwIndices[pbsCurData->m_iCurIndex] = iNextWedge;
					pbsCurData->m_rgfWeights[pbsCurData->m_iCurIndex] = pbsOldData->m_rgfWeights[iIndex];
					pbsCurData->m_iCurIndex += 1;

					iNextWedge = pMeshData->m_rgVertices[iNextWedge].iWedgeList;
				}
			}
		}

		// now that we do have skin data to put in the file, add a skinning header record
		cbHeaderData = sizeof(DWORD) * 3;
		pbCur = pbHeaderData = new BYTE[cbHeaderData];
		if (pbHeaderData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		WRITE_WORD(pbCur, (WORD)psc->m_cMaxWeightsPerVertex);
		WRITE_WORD(pbCur, (WORD)psc->m_cMaxWeightsPerFace);
		WRITE_WORD(pbCur, (WORD)psmSkinMap->m_cbiBones);

		hr = psc->m_pxofsave->CreateDataObject(DXFILEOBJ_XSkinMeshHeader,
										NULL,
										NULL,
										cbHeaderData,
										pbHeaderData,
										&pDataObject
										);
		hr = pParent->AddDataObject(pDataObject);
		if (FAILED(hr))
			goto e_Exit;

		RELEASE(pDataObject);

		// now actually output the prepared buffers
		for (iBone = 0; iBone < psmSkinMap->m_cbiBones; iBone++)
		{
			assert(rgbsBoneData[iBone].m_iCurIndex == rgbsBoneData[iBone].m_cVertices);
			assert(rgbsBoneDataDups[iBone].m_iCurIndex == rgbsBoneDataDups[iBone].m_cVertices);

			hr = psc->m_pxofsave->CreateDataObject(DXFILEOBJ_SkinWeights,
											NULL,
											NULL,
											rgbsBoneDataDups[iBone].m_cbData,
											rgbsBoneDataDups[iBone].m_pbData,
											&pDataObject
											);
			if (FAILED(hr))
				goto e_Exit;

			hr = pParent->AddDataObject(pDataObject);
			if (FAILED(hr))
				goto e_Exit;

			RELEASE(pDataObject);
		}
	}

e_Exit:
	delete []pbHeaderData;
	delete []rgbsBoneData;
	delete []rgbsBoneDataDups;
	return hr;
}

HRESULT AddMesh
    (
    SSaveContext *psc,
    INode *pNode, 
    Object* pObj,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT hr = S_OK;
    BOOL bDeleteTriObject = false;
    TriObject *pTriObject = NULL;
    Mesh *pMesh;
    BOOL bSwapTriOrder;
    PBYTE pbData = NULL;
    PBYTE pbCur;
    DWORD cbSize;
    DWORD cVertices;
    DWORD cFaces;
    DWORD iFace;
    Matrix3 matNodeTM;
    SMeshData MeshData;
    LPDIRECTXFILEDATA pDataObject = NULL;
    DWORD iVertex;
    DWORD *rgdwMeshMaterials = NULL;
    SCropInfo *rgCropInfo = NULL;
    D3DXMATERIAL *rgMaterials = NULL;
    DWORD cMaterials;

    // Retrieve the TriObject from the node

    pTriObject = GetTriObjectFromObjRef(pObj, &bDeleteTriObject);

    // If no TriObject then skip
    if (pTriObject == NULL) 
        goto e_Exit;

    pMesh = &(pTriObject->mesh);
    pMesh->checkNormals(TRUE); // TODO: Is this necessary?
    matNodeTM = pNode->GetNodeTM(psc->m_iTime);
    bSwapTriOrder = matNodeTM.Parity();


    hr = GatherMeshMaterials(psc, pNode, pMesh, &rgdwMeshMaterials, &rgMaterials, &rgCropInfo, &cMaterials);
    if (FAILED(hr))
        goto e_Exit;

    hr = GenerateMeshData(pMesh, &MeshData, rgdwMeshMaterials);
    if (FAILED(hr))
        goto e_Exit;

    cVertices = MeshData.m_cVertices;
    cFaces = MeshData.m_cFaces;
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
    for (iVertex = 0; iVertex < MeshData.m_cVertices; iVertex++)
    {
		//	EDITED : 
		//
		if ( psc->m_bLeftHandAxis )
		{
			Point3 Vert;
			Vert.x = pMesh->verts[MeshData.m_rgVertices[iVertex].iPointRep].x;
			Vert.y = pMesh->verts[MeshData.m_rgVertices[iVertex].iPointRep].z;
			Vert.z = pMesh->verts[MeshData.m_rgVertices[iVertex].iPointRep].y;

			WRITE_POINT3 ( pbCur, Vert );
		}
		else
		{
			WRITE_POINT3 ( pbCur, pMesh->verts[MeshData.m_rgVertices[iVertex].iPointRep] );
		}
    }

    // write nFaces
    WRITE_DWORD(pbCur, cFaces);
    
    // write faces
    for( iFace = 0; iFace < cFaces; iFace++ )
    {
        WRITE_DWORD(pbCur, 3); //nFaceVertexIndices

		//	EDITED : 
		//
        // face vertex indices
		if ( psc->m_bLeftHandAxis )
		{
			if( !bSwapTriOrder )
			{
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[0]);
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[2]);
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[1]);
			}
			else
			{
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[0]);
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[1]);
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[2]);
			}
		}
		else
		{
			if( bSwapTriOrder )
			{
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[0]);
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[2]);
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[1]);
			}
			else
			{
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[0]);
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[1]);
				WRITE_DWORD(pbCur, MeshData.m_rgFaces[iFace].index[2]);
			}
		}
	}

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMesh,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
        goto e_Exit;

    // add the normals to the file
    hr = AddNormals(psc, &MeshData, bSwapTriOrder, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    // write texture coordinates
    hr = AddTextureCoordinates(psc, &MeshData, rgCropInfo, pMesh, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddVertexDuplicationIndices(psc, &MeshData, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshMaterials(psc, pNode, pMesh, rgdwMeshMaterials, rgMaterials, cMaterials, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddSkinData(psc, pNode, &MeshData, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    if (bDeleteTriObject)
    {
        delete pTriObject;
    }

    RELEASE(pDataObject);
    delete []rgdwMeshMaterials;
    delete []rgMaterials;
    delete []rgCropInfo;
    return hr;
}

HRESULT
AddPatchMeshMaterials(
    SSaveContext *psc,
    INode *pNode,
    PatchMesh *pPatchMesh,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    Mtl *pMtlMain;
    Mtl *pMtlCur;
    DWORD cSubMaterials;
    DWORD iPatch;
    DWORD cPatches;
    BOOL bNoSubMaterials;
    BOOL bWireframeColor;
    PBYTE pbCur;
    PBYTE pbData = NULL;
    DWORD cbSize;
    DWORD iCurMatID;
    DWORD iCurMaterial;

    pMtlMain = pNode->GetMtl();
    cPatches = pPatchMesh->getNumPatches();
    cSubMaterials = 0;
    bNoSubMaterials = FALSE;
    bWireframeColor = FALSE;

    // The color of a given mesh can be provided by three different sources:
    //   1) applied texture maps, as part of a material
    //   2) explicitly defined & applied materials without texture maps
    //   3) a 'wireframe' color.
    
    // For our purposes, we will output these in this order of preference, ignoring
    // processing past the first one we find.

    cbSize = sizeof(DWORD) // nMaterials
                + sizeof(DWORD) // nFaceIndexes
                + cPatches*sizeof(DWORD); // face indexes

    pbData = pbCur = new BYTE[cbSize];
    if (pbCur == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    
    if (pMtlMain != NULL)
    {
        // There is at least one material. We're in case 1) or 2)

        cSubMaterials = pMtlMain->NumSubMtls();
        if (cSubMaterials < 1)
        {
            // Count the material itself as a submaterial.
            cSubMaterials = 1;
            bNoSubMaterials = TRUE;
        }
    }
    else  // no material, then we'll create a material corresponding to the default wire color.
    {
        bWireframeColor = TRUE;
        cSubMaterials = 1;
    }

    WRITE_DWORD(pbCur, cSubMaterials);
    WRITE_DWORD(pbCur, cPatches);


    // For each face, output the index of the material which applies to it, 
    // starting from  0

    for (iPatch=0; iPatch < cPatches; iPatch++)
    {

        if (bWireframeColor || bNoSubMaterials) 
        {
            // If we're using wireframe color, it's our only material
            WRITE_DWORD(pbCur, 0);
        }
        else
        {
            // Otherwise we have at least one material to process.

            iCurMatID = pPatchMesh->getPatchMtlIndex(iPatch);

            if (cSubMaterials == 1)
            {
                iCurMatID = 0;
            }
            else
            {
                // SDK recommends mod'ing the material ID by the valid # of materials, 
                // as sometimes a material number that's too high is returned.
                iCurMatID %= cSubMaterials;
            }

            // output the appropriate material color

            WRITE_DWORD(pbCur, iCurMatID);

        } 

    } 

    // now finally create the mesh material list
    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMMeshMaterialList,
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

    // 3DSMax uses wireframe color as its default material for a mesh.
    // Output the wireframe color as the material if there are no explicit materials.

    if (bWireframeColor)
    {
        AddWireframeMaterial(psc, pNode, pDataObject);
    } 
    else
    {
        // 3DSMax allows multiple materials to be used on a single mesh via
        // 'submaterials'. When the first submaterial is defined, the main material
        // is copied into submaterial #1, and the new submaterial because submaterial #2.
        // 
        // We have to catch the case where there's a material, but no submaterials. For this
        // case, set NumSubMaterials to 1 which would never happen otherwise. It's imperative
        // that the first material be set to MtlMain, rather than trying to GetSubMtl() to 
        // allow this logic to work.

        // Loop through the materials (if any) and output them.

        for (iCurMaterial = 0; (iCurMaterial < cSubMaterials); iCurMaterial++)
        {
            if (bNoSubMaterials)
            {
                // We're on the parent material, possibly the ONLY material.
                // We won't be able to get it with GetSubMtl() if it's the only one, and
                // the data in the first submaterial is identical to the main material,
                // so just use the main material.

                pMtlCur = pMtlMain;
            }
            else
            {
                // We're into the true submaterials. Safe to get with 'GetSubMtl'

                pMtlCur = pMtlMain->GetSubMtl(iCurMaterial);
            }

            hr = AddMaterial(psc, pMtlCur, pDataObject);
            if (FAILED(hr))
                goto e_Exit;
        } 
    }

e_Exit:
    delete []pbData;
    RELEASE(pDataObject);


    return hr;
} 

HRESULT AddPatchMesh
    (
    SSaveContext *psc,
    INode *pNode, 
    Object* pObj,
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT    hr = S_OK;
    LPDIRECTXFILEDATA pDataObject = NULL;
    PBYTE         pbData = NULL;
    PBYTE		  pbCur = NULL;        
    DWORD          cbSize;
    DWORD iVertex;
    DWORD iPatch;
    Point3 *pvVertices;
    PatchMesh *pPatchMesh;
	PatchObject *pPatchObject = NULL;
	BOOL bDeletePatchObject;
	DWORD iControl;
    DWORD cPatchIndices;
	SPatchMeshData PatchMeshData;

	assert(psc != NULL);
	assert(pNode != NULL);
	assert(pObj != NULL);
	assert(pParent != NULL);

    pPatchObject = GetPatchObjectFromObjRef(pObj, &bDeletePatchObject);

    // If no PatchObject then skip
    if (pPatchObject == NULL) 
        goto e_Exit;

    pPatchMesh = &pPatchObject->patch;

	// massage the patch data into an outputable format (including texture coord handling)
	hr = GeneratePatchMeshData(pPatchMesh, &PatchMeshData);
	if (FAILED(hr))
		goto e_Exit;

	// figure out the total number of control indices
	cPatchIndices = 0;
	for (iPatch = 0; iPatch < PatchMeshData.m_cPatches; iPatch++)
	{
		cPatchIndices += PatchMeshData.m_rgPatches[iPatch].m_cControl;
	}

    cbSize = sizeof(DWORD) // nVertices
             + PatchMeshData.m_cVertices * sizeof(float)*3 // vertices
             + sizeof(DWORD) // nPatches
             + PatchMeshData.m_cPatches * sizeof(DWORD) /*nControlIndices*/ 
             + cPatchIndices * sizeof(DWORD) /*controlIndices*/; // patches

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	// write the number of vertices
	WRITE_DWORD(pbCur, PatchMeshData.m_cVertices);

    // first write all the vertices
    pvVertices = (Point3*)pbCur;
    for (iVertex = 0; iVertex < PatchMeshData.m_cVertices; iVertex++)
    {
        pvVertices[iVertex] = PatchMeshData.m_rgVertices[iVertex].vPosition;
    }

    // skip over the vertices
    pbCur = pbData + sizeof(DWORD) + sizeof(float)*3*PatchMeshData.m_cVertices;

	// write the number of patches
	WRITE_DWORD(pbCur, PatchMeshData.m_cPatches);

	// now write all the patch data
	for (iPatch = 0; iPatch < PatchMeshData.m_cPatches; iPatch++)
	{
		WRITE_DWORD(pbCur, PatchMeshData.m_rgPatches[iPatch].m_cControl);
		for (iControl = 0; iControl < PatchMeshData.m_rgPatches[iPatch].m_cControl; iControl++)
		{
			WRITE_DWORD(pbCur, PatchMeshData.m_rgPatches[iPatch].m_rgdwControl[iControl]);
		}
	}

    hr = psc->m_pxofsave->CreateDataObject(DXFILEOBJ_PatchMesh,
                                    NULL,
                                    NULL,
                                    cbSize,
                                    pbData,
                                    &pDataObject
                                    );
    if (FAILED(hr))
        goto e_Exit;

    hr = AddPatchTextureCoordinates(psc, &PatchMeshData, pPatchMesh, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddPatchVertexDuplicationIndices(psc, &PatchMeshData, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddPatchMeshMaterials(psc, pNode, pPatchMesh, pDataObject);
    if (FAILED(hr))
        goto e_Exit;

    hr = pParent->AddDataObject(pDataObject);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    delete []pbData;
    RELEASE(pDataObject);

    if (bDeletePatchObject)
    {
        delete pPatchObject;
    }

    return hr;
}

HRESULT AddTransform
    (
    SSaveContext *psc,
    INode *pNode, 
    LPDIRECTXFILEDATA pParent
    )
{
    HRESULT hr = S_OK;
    PBYTE pbData = NULL;
    PBYTE pbCur;
    DWORD cbSize;
    Matrix3 matNodeTM;
    Matrix3 matRelativeTM;
    LPDIRECTXFILEDATA pDataObject = NULL;

    cbSize = 16*sizeof(float);

    pbCur = pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // Node transform
    matNodeTM = pNode->GetNodeTM(psc->m_iTime);

    if( pNode->IsRootNode() )
    {
        // scene root
        matRelativeTM = matNodeTM;

		//	EDITED :
		//
		if ( psc->m_bLeftHandAxis )
		{
			RHMatToLHMat ( matRelativeTM, matRelativeTM );
		}
    }
    else
    {
        Matrix3 matParentTM = pNode->GetParentTM(psc->m_iTime);
        if( memcmp(&matNodeTM,&matParentTM,12*sizeof(float)) == 0 )
        {
            matRelativeTM.IdentityMatrix();
        }
        else
        {
			//	EDITED :
			//
			if ( psc->m_bLeftHandAxis )
			{
				Matrix3 matNodeTMlh;
				Matrix3 matParentTMlh;

				RHMatToLHMat ( matNodeTM, matNodeTMlh );
				RHMatToLHMat ( matParentTM, matParentTMlh );

				matRelativeTM = matNodeTMlh * Inverse(matParentTMlh);
			}
			else
			{
	            matRelativeTM = matNodeTM * Inverse(matParentTM);
			}
        }
	}

	WRITE_MATRIX4_FROM_MATRIX3(pbCur, matRelativeTM);


    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMFrameTransformMatrix,
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
    RELEASE(pDataObject);

    return hr;
}

HRESULT AddObjectOffsetTransform
    (
    SSaveContext *psc,
	INode *pNode,
    LPDIRECTXFILEDATA pParent,
    LPDIRECTXFILEDATA *ppMeshParent
    )
{
    HRESULT hr = S_OK;
    PBYTE pbData = NULL;
    PBYTE pbCur;
    DWORD cbSize;
    LPDIRECTXFILEDATA pDataObject = NULL;
	Matrix3 matNodeTM;
	Matrix3 matObjTMAfterWSM;
	Matrix3 matObjectOffset;
    LPDIRECTXFILEDATA pObjectOffset = NULL;

	// check to see if the node has an object offset matrix
	matNodeTM = pNode->GetNodeTM(psc->m_iTime);
	matObjTMAfterWSM = pNode->GetObjTMAfterWSM(psc->m_iTime);
	if (memcmp(&matObjTMAfterWSM, &matNodeTM,12*sizeof(Matrix3)) != 0)
	{
		// the mesh is positioned offset from the node, so add another
		//   frame (unnamed) to offset the mesh without affecting the node's children
		//   and/or animation attached to the node
		hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMFrame,
										NULL,
										NULL,
										0,
										NULL,
										&pObjectOffset
										);

		//	EDITED :
		//
		if ( psc->m_bLeftHandAxis )
		{
			Matrix3 matNodeTMlh;
			Matrix3 matObjTMAfterWSMlh;

			RHMatToLHMat ( matNodeTM, matNodeTMlh );
			RHMatToLHMat ( matObjTMAfterWSM, matObjTMAfterWSMlh );

			matObjectOffset = matObjTMAfterWSMlh * Inverse(matNodeTMlh);
		}
		else
		{
            matObjectOffset = matObjTMAfterWSM * Inverse(matNodeTM);
		}


		cbSize = 16*sizeof(float);

		pbCur = pbData = new BYTE[cbSize];
		if (pbData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		WRITE_MATRIX4_FROM_MATRIX3(pbCur, matObjectOffset);

		hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMFrameTransformMatrix,
										NULL,
										NULL,
										cbSize,
										pbData,
										&pDataObject
										);
		if (FAILED(hr))
			goto e_Exit;

		hr = pObjectOffset->AddDataObject(pDataObject);
		if (FAILED(hr))
			goto e_Exit;

		hr = pParent->AddDataObject(pObjectOffset);
		if (FAILED(hr))
			goto e_Exit;


		*ppMeshParent = pObjectOffset;
	}
	else  // identity object offset, mesh should use node as parent
	{
		*ppMeshParent = pParent;
	}

e_Exit:
    delete []pbData;
    RELEASE(pDataObject);
	RELEASE(pObjectOffset);

    return hr;
}

HRESULT EnumTree(
    SSaveContext *psc,
    INode *pNode, 
    LPDIRECTXFILEDATA *ppDataObjectNew )
{
    HRESULT hr = S_OK;
    DWORD cChildren;
    DWORD iChild;
    LPDIRECTXFILEDATA pChildDataObject = NULL;
    LPDIRECTXFILEDATA pDataObject = NULL;
    LPDIRECTXFILEDATA pMeshParent = NULL;
    Object* pObj = NULL;
    char *szName = NULL;

    szName = psc->m_stStrings.CreateNiceString(pNode->GetName());
    if (szName == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	if( !psc->m_bSaveSelection || IsSelectedChild( pNode ) )
	{
		OutputDebugString( szName );
		OutputDebugString( "\n" );

		hr = psc->m_pxofsave->CreateDataObject(
			TID_D3DRMFrame,
			szName,
			NULL,
			0,
			NULL,
			&pDataObject );

		hr = AddTransform(psc, pNode, pDataObject);
		if (FAILED(hr))
			goto e_Exit;

		// add the node to the array for anim purposes
		assert( psc->m_cNodesCur < psc->m_cNodes );
		psc->m_rgpnNodes[psc->m_cNodesCur] = pNode;
		psc->m_cNodesCur += 1;
	}

	if( !psc->m_bSaveSelection || pNode->Selected() )
	{
		if (psc->m_bSavePatchData && IsExportablePatchMesh(pNode, pObj))
		{
			hr = AddObjectOffsetTransform(psc, pNode, pDataObject, &pMeshParent);
			if (FAILED(hr))
				goto e_Exit;

			hr = AddPatchMesh(psc, pNode, pObj, pMeshParent);
			if (FAILED(hr))
				goto e_Exit;
		}
		else if (IsExportableMesh(pNode, pObj))
		{
			hr = AddObjectOffsetTransform(psc, pNode, pDataObject, &pMeshParent);
			if (FAILED(hr))
				goto e_Exit;
			
			//	EDITED : 프레임 출력시, 바이페드 숨김? 검사.
			//
			if ( psc->m_ExportDataOption&EXPD_FRAME )
			{
				if ( psc->m_bHideBiped ) 
				{
					if ( !_streql ( szName, "Bip", 3 ) && !_streql ( szName, "bip", 3 ) )
					{
						hr = AddMesh(psc, pNode, pObj, pMeshParent);
						if (FAILED(hr))		goto e_Exit;
					}
				}
				else
				{
					hr = AddMesh(psc, pNode, pObj, pMeshParent);
					if (FAILED(hr))			goto e_Exit;
				}
			}
		}
	}

	cChildren = pNode->NumberOfChildren();
    for (iChild = 0; iChild < cChildren; iChild++)
    {
        // enumerate the child
        hr = EnumTree(psc, pNode->GetChildNode(iChild), &pChildDataObject);
        if (FAILED(hr))
            goto e_Exit;

		if( pChildDataObject )
		{
			hr = pDataObject->AddDataObject(pChildDataObject);
			if (FAILED(hr))
				goto e_Exit;

			RELEASE(pChildDataObject);
		}
    }

    *ppDataObjectNew = pDataObject;

e_Exit:
    return hr;
}


HRESULT Preprocess
    (
    SPreprocessContext *ppc,
    INode *pNode
    )
{
    HRESULT hr = S_OK;
    DWORD cChildren;
    DWORD iChild;
    Object* pObj;
    SSkinMap **rgpsmTemp = NULL;
    IPhyVertexExport *pVertexExport;
    IPhyRigidVertex* pRigidVertexExport;
    IPhyBlendedRigidVertex *pBlendedRigidVertexExport;
    INode* pBoneNode;
    SSkinMap *psmSkinMap;
    Modifier* pPhyMod = NULL;
    Modifier* pSkinMod = NULL;
    IPhysiqueExport* pPhyExport = NULL;
    IPhyContextExport* pPhyContextExport = NULL;
    DWORD iVertex;
    DWORD cVertices;
    DWORD iVertexType;
    SBoneInfo *pbi;
    DWORD cTotalBones;
    DWORD iBone;
    DWORD cpnBonesSeen;
    DWORD cpnBonesSeenMax;
    INode **rgpnBonesSeen = NULL;
    INode **rgpnTemp;
    BOOL bBoneSeen;
    DWORD iBoneSeen;
    BOOL bDeleteTriObject = false;
    TriObject *pTriObject = NULL;
    Mesh *pMesh;
	DWORD iFace;
	DWORD iPoint;

	if( !ppc->m_bSaveSelection || IsSelectedChild( pNode ) )
	    ppc->m_cNodes += 1;

    if (IsExportableMesh(pNode,pObj) && (!ppc->m_bSaveSelection || pNode->Selected()))
    {
        // first see if physique is present
        pPhyMod = FindPhysiqueModifier(pNode);
        if (pPhyMod != NULL)
        {
            // Get a Physique Export interface
            pPhyExport = (IPhysiqueExport *)pPhyMod->GetInterface(I_PHYINTERFACE);
            if (pPhyExport == NULL)
            {   // not all interfaces present, so ignore
                goto e_NoPhysique;
            }

            // For a given Object's INode get a
            // ModContext Interface from the Physique Export Interface:
            pPhyContextExport = (IPhyContextExport *)pPhyExport->GetContextInterface(pNode);
            if(pPhyContextExport == NULL)
            {
                // not all interfaces present, so ignore
                goto e_NoPhysique;
            }

            // convert to rigid with blending
            pPhyContextExport->ConvertToRigid(TRUE);
            pPhyContextExport->AllowBlending(TRUE);

            psmSkinMap = new SSkinMap;
            if (psmSkinMap == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            // realloc if necessary
            if (ppc->m_cpsmSkinMaps == ppc->m_cpsmSkinMapsMax)
            {
                rgpsmTemp = ppc->m_rgpsmSkinMaps;

                ppc->m_cpsmSkinMapsMax = max(1, ppc->m_cpsmSkinMapsMax);
                ppc->m_cpsmSkinMapsMax = ppc->m_cpsmSkinMapsMax * 2;
                ppc->m_rgpsmSkinMaps = new SSkinMap*[ppc->m_cpsmSkinMapsMax];
                if (ppc->m_rgpsmSkinMaps == NULL)
                {
                    ppc->m_rgpsmSkinMaps = rgpsmTemp;
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                if (ppc->m_cpsmSkinMaps > 0)
                {
                    memcpy(ppc->m_rgpsmSkinMaps, rgpsmTemp, sizeof(SSkinMap*) * ppc->m_cpsmSkinMaps);
                }

                delete []rgpsmTemp;
            }
            ppc->m_rgpsmSkinMaps[ppc->m_cpsmSkinMaps] = psmSkinMap;
            ppc->m_cpsmSkinMaps += 1;

            // init the map
            psmSkinMap->m_pMeshNode = pNode;
            psmSkinMap->m_cbiBonesMax = 30;
            psmSkinMap->m_rgbiBones = new SBoneInfo[psmSkinMap->m_cbiBonesMax];
            if (psmSkinMap->m_rgbiBones == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            // init duplication removal for redundant weights
            cpnBonesSeenMax = 30;
            cpnBonesSeen = 0;
            rgpnBonesSeen = new INode*[cpnBonesSeenMax];
            if (rgpnBonesSeen == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            cVertices = pPhyContextExport->GetNumberVertices();
            for (iVertex = 0; iVertex < cVertices; iVertex++ )
            {
                pVertexExport = (IPhyVertexExport *)pPhyContextExport->GetVertexInterface(iVertex);    
                if (pVertexExport == NULL)
                {
                    OutputDebugString("Couldn't get export interface!");
                    hr = E_FAIL;
                    goto e_Exit;
                }
            
                // What kind of vertices are these?
                iVertexType = pVertexExport->GetVertexType();

                pPhyContextExport->ReleaseVertexInterface( pVertexExport );    

                if( iVertexType == RIGID_TYPE )
                {
                    pRigidVertexExport = (IPhyRigidVertex *)pPhyContextExport->GetVertexInterface(iVertex);
                    if (pRigidVertexExport == NULL)
                    {
                        OutputDebugString("Couldn't get rigid vertex export interface!");
                        hr = E_FAIL;
                        goto e_Exit;
                    }
                    // Get the vertex info!
                
                    pBoneNode = pRigidVertexExport->GetNode();

                    pbi = psmSkinMap->FindBone(pBoneNode);
                    if (pbi == NULL)
                    {
                        hr = psmSkinMap->AddBone(pBoneNode, &pbi);
                        if (FAILED(hr))
                            goto e_Exit;
                    }

                    pbi->m_cVertices += 1;

                    ppc->m_cMaxWeightsPerVertex = max(1, ppc->m_cMaxWeightsPerVertex);

                    pPhyContextExport->ReleaseVertexInterface( pRigidVertexExport);
                }
                else
                {
                    assert( iVertexType == RIGID_BLENDED_TYPE );

                    pBlendedRigidVertexExport = (IPhyBlendedRigidVertex *)pPhyContextExport->GetVertexInterface(iVertex);
                    if (pBlendedRigidVertexExport == NULL)
                    {
                        OutputDebugString("Couldn't get blended rigid vertex export interface!");
                        hr = E_FAIL;
                        goto e_Exit;
                    }

                    // How many nodes affect his vertex?
                    cTotalBones = pBlendedRigidVertexExport->GetNumberNodes();
                    cpnBonesSeen = 0;
                    for (iBone = 0; iBone < cTotalBones; iBone++ )
                    {
                        pBoneNode = pBlendedRigidVertexExport->GetNode(iBone);

                        // first see if the bone has already been seen
                        bBoneSeen = FALSE;
                        for (iBoneSeen = 0; iBoneSeen < cpnBonesSeen; iBoneSeen++)
                        {
                            if (rgpnBonesSeen[iBoneSeen] == pBoneNode)
                            {
                                bBoneSeen = TRUE;
                                break;
                            }
                        }
                        
                        // if not seen, collect stats and add to already seen
                        if (!bBoneSeen)
                        {
                            pbi = psmSkinMap->FindBone(pBoneNode);
                            if (pbi == NULL)
                            {
                                hr = psmSkinMap->AddBone(pBoneNode, &pbi);
                                if (FAILED(hr))
                                    goto e_Exit;
                            }
                            pbi->m_cVertices += 1;

                            if (cpnBonesSeen >= cpnBonesSeenMax)
                            {
                                rgpnTemp = rgpnBonesSeen;
                                cpnBonesSeenMax *= 2;

                                rgpnBonesSeen = new INode*[cpnBonesSeenMax];
                                if (rgpnBonesSeen == NULL)
                                {
                                    rgpnBonesSeen = rgpnTemp;
                                    hr = E_OUTOFMEMORY;
                                    goto e_Exit;
                                }

                                memcpy(rgpnBonesSeen, rgpnTemp, cpnBonesSeen * sizeof(INode*));
                                delete []rgpnTemp;
                            }

                            rgpnBonesSeen[cpnBonesSeen] = pBoneNode;
                            cpnBonesSeen += 1;
                        }
                    }

                    // actualNum... accounts for duplicated weights to same transform node
                    // that are combined automatically above
                    ppc->m_cMaxWeightsPerVertex = max(cpnBonesSeen, ppc->m_cMaxWeightsPerVertex);
            
                    pPhyContextExport->ReleaseVertexInterface( pBlendedRigidVertexExport);
                }
            }

			// now figure out the max number of weights per face

            pTriObject = GetTriObjectFromObjRef(pObj, &bDeleteTriObject);
            if (pTriObject == NULL) 
            {
                OutputDebugString("Physique info, but no mesh");
                hr = E_FAIL;
                goto e_Exit;
            }

            pMesh = &(pTriObject->mesh);

            for (iFace = 0; iFace < (DWORD)pMesh->numFaces; iFace++)
            {
				cpnBonesSeen = 0;
				for (iPoint = 0; iPoint < 3; iPoint++ )
				{
					iVertex = pMesh->faces[iFace].v[iPoint];

					pVertexExport = (IPhyVertexExport *)pPhyContextExport->GetVertexInterface(iVertex);    
					if (pVertexExport == NULL)
					{
						OutputDebugString("Couldn't get export interface!");
						hr = E_FAIL;
						goto e_Exit;
					}
            
					// What kind of vertices are these?
					iVertexType = pVertexExport->GetVertexType();

					pPhyContextExport->ReleaseVertexInterface( pVertexExport );    

					if( iVertexType == RIGID_TYPE )
					{
						pRigidVertexExport = (IPhyRigidVertex *)pPhyContextExport->GetVertexInterface(iVertex);
						if (pRigidVertexExport == NULL)
						{
							OutputDebugString("Couldn't get rigid vertex export interface!");
							hr = E_FAIL;
							goto e_Exit;
						}
						// Get the vertex info!
                
						pBoneNode = pRigidVertexExport->GetNode();

						pbi = psmSkinMap->FindBone(pBoneNode);
						if (pbi == NULL)
						{
							hr = psmSkinMap->AddBone(pBoneNode, &pbi);
							if (FAILED(hr))
								goto e_Exit;
						}

						// first see if the bone has already been seen
						bBoneSeen = FALSE;
						for (iBoneSeen = 0; iBoneSeen < cpnBonesSeen; iBoneSeen++)
						{
							if (rgpnBonesSeen[iBoneSeen] == pBoneNode)
							{
								bBoneSeen = TRUE;
								break;
							}
						}
                    
						// if not seen, collect stats and add to already seen
						if (!bBoneSeen)
						{
							if (cpnBonesSeen >= cpnBonesSeenMax)
							{
								rgpnTemp = rgpnBonesSeen;
								cpnBonesSeenMax *= 2;

								rgpnBonesSeen = new INode*[cpnBonesSeenMax];
								if (rgpnBonesSeen == NULL)
								{
									rgpnBonesSeen = rgpnTemp;
									hr = E_OUTOFMEMORY;
									goto e_Exit;
								}

								memcpy(rgpnBonesSeen, rgpnTemp, cpnBonesSeen * sizeof(INode*));
								delete []rgpnTemp;
							}

							rgpnBonesSeen[cpnBonesSeen] = pBoneNode;
							cpnBonesSeen += 1;
						}

						pPhyContextExport->ReleaseVertexInterface( pRigidVertexExport);
					}
					else
					{
						assert( iVertexType == RIGID_BLENDED_TYPE );

						pBlendedRigidVertexExport = (IPhyBlendedRigidVertex *)pPhyContextExport->GetVertexInterface(iVertex);
						if (pBlendedRigidVertexExport == NULL)
						{
							OutputDebugString("Couldn't get blended rigid vertex export interface!");
							hr = E_FAIL;
							goto e_Exit;
						}

						// How many nodes affect his vertex?
						cTotalBones = pBlendedRigidVertexExport->GetNumberNodes();
						for (iBone = 0; iBone < cTotalBones; iBone++ )
						{
							pBoneNode = pBlendedRigidVertexExport->GetNode(iBone);

							// first see if the bone has already been seen
							bBoneSeen = FALSE;
							for (iBoneSeen = 0; iBoneSeen < cpnBonesSeen; iBoneSeen++)
							{
								if (rgpnBonesSeen[iBoneSeen] == pBoneNode)
								{
									bBoneSeen = TRUE;
									break;
								}
							}
                        
							// if not seen, collect stats and add to already seen
							if (!bBoneSeen)
							{
								if (cpnBonesSeen >= cpnBonesSeenMax)
								{
									rgpnTemp = rgpnBonesSeen;
									cpnBonesSeenMax *= 2;

									rgpnBonesSeen = new INode*[cpnBonesSeenMax];
									if (rgpnBonesSeen == NULL)
									{
										rgpnBonesSeen = rgpnTemp;
										hr = E_OUTOFMEMORY;
										goto e_Exit;
									}

									memcpy(rgpnBonesSeen, rgpnTemp, cpnBonesSeen * sizeof(INode*));
									delete []rgpnTemp;
								}

								rgpnBonesSeen[cpnBonesSeen] = pBoneNode;
								cpnBonesSeen += 1;
							}
						}

						pPhyContextExport->ReleaseVertexInterface( pBlendedRigidVertexExport);
					}
				}

				ppc->m_cMaxWeightsPerFace = max(cpnBonesSeen, ppc->m_cMaxWeightsPerFace);
            }
        }

e_NoPhysique:;

        pSkinMod = FindSkinModifier( pNode );
	    if( pSkinMod )
	    {
            ISkin* pSkin = (ISkin*)pSkinMod->GetInterface( I_SKIN );
            if( !pSkin )
                goto e_NoSkin;

            if( pSkin->GetNumBones() <= 0 )
                goto e_NoSkin;

            ISkinContextData* pSkinData = (ISkinContextData*)pSkin->GetContextInterface( pNode );
            if( !pSkinData )
                goto e_NoSkin;

            psmSkinMap = new SSkinMap;
            if( !psmSkinMap )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            // realloc if necessary
            if (ppc->m_cpsmSkinMaps == ppc->m_cpsmSkinMapsMax)
            {
                rgpsmTemp = ppc->m_rgpsmSkinMaps;

                ppc->m_cpsmSkinMapsMax = max(1, ppc->m_cpsmSkinMapsMax);
                ppc->m_cpsmSkinMapsMax = ppc->m_cpsmSkinMapsMax * 2;
                ppc->m_rgpsmSkinMaps = new SSkinMap*[ppc->m_cpsmSkinMapsMax];
                if (ppc->m_rgpsmSkinMaps == NULL)
                {
                    ppc->m_rgpsmSkinMaps = rgpsmTemp;
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                if (ppc->m_cpsmSkinMaps > 0)
                {
                    memcpy(ppc->m_rgpsmSkinMaps, rgpsmTemp, sizeof(SSkinMap*) * ppc->m_cpsmSkinMaps);
                }

                delete []rgpsmTemp;
            }
            ppc->m_rgpsmSkinMaps[ppc->m_cpsmSkinMaps] = psmSkinMap;
            ppc->m_cpsmSkinMaps += 1;

            // init the map
            psmSkinMap->m_pMeshNode = pNode;
            psmSkinMap->m_cbiBonesMax = 30;
            psmSkinMap->m_rgbiBones = new SBoneInfo[psmSkinMap->m_cbiBonesMax];
            if (psmSkinMap->m_rgbiBones == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            // init duplication removal for redundant weights
            cpnBonesSeenMax = 30;
            cpnBonesSeen = 0;
            rgpnBonesSeen = new INode*[cpnBonesSeenMax];
            if (rgpnBonesSeen == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            cVertices = pSkinData->GetNumPoints();
            for (iVertex = 0; iVertex < cVertices; iVertex++ )
            {
                // How many nodes affect his vertex?
                cTotalBones = pSkinData->GetNumAssignedBones( iVertex );
                if( cTotalBones > 4 )
                    continue;

                cpnBonesSeen = 0;
                for (iBone = 0; iBone < cTotalBones; iBone++ )
                {
                    int	boneNr = pSkinData->GetAssignedBone( iVertex, iBone );
                    if( boneNr < 0 )
		                continue;

                    pBoneNode = pSkin->GetBone( boneNr );
                    if( !pBoneNode )
                        continue;

                    // first see if the bone has already been seen
                    bBoneSeen = FALSE;
                    for (iBoneSeen = 0; iBoneSeen < cpnBonesSeen; iBoneSeen++)
                    {
                        if (rgpnBonesSeen[iBoneSeen] == pBoneNode)
                        {
                            bBoneSeen = TRUE;
                            break;
                        }
                    }

                    // if not seen, collect stats and add to already seen
                    if (!bBoneSeen)
                    {
                        pbi = psmSkinMap->FindBone(pBoneNode);
                        if (pbi == NULL)
                        {
                            hr = psmSkinMap->AddBone(pBoneNode, &pbi);
                            if (FAILED(hr))
                                goto e_Exit;
                        }
                        pbi->m_cVertices += 1;

                        if (cpnBonesSeen >= cpnBonesSeenMax)
                        {
                            rgpnTemp = rgpnBonesSeen;
                            cpnBonesSeenMax *= 2;

                            rgpnBonesSeen = new INode*[cpnBonesSeenMax];
                            if (rgpnBonesSeen == NULL)
                            {
                                rgpnBonesSeen = rgpnTemp;
                                hr = E_OUTOFMEMORY;
                                goto e_Exit;
                            }

                            memcpy(rgpnBonesSeen, rgpnTemp, cpnBonesSeen * sizeof(INode*));
                            delete []rgpnTemp;
                        }

                        rgpnBonesSeen[cpnBonesSeen] = pBoneNode;
                        cpnBonesSeen += 1;
                    }
                }

                // actualNum... accounts for duplicated weights to same transform node
                // that are combined automatically above
                ppc->m_cMaxWeightsPerVertex = max(cpnBonesSeen, ppc->m_cMaxWeightsPerVertex);
            }

			// now figure out the max number of weights per face

            pTriObject = GetTriObjectFromObjRef(pObj, &bDeleteTriObject);
            if (pTriObject == NULL) 
            {
                OutputDebugString("Skin info, but no mesh");
                hr = E_FAIL;
                goto e_Exit;
            }

            pMesh = &(pTriObject->mesh);

            for (iFace = 0; iFace < (DWORD)pMesh->numFaces; iFace++)
            {
				cpnBonesSeen = 0;
				for (iPoint = 0; iPoint < 3; iPoint++ )
				{
					iVertex = pMesh->faces[iFace].v[iPoint];

					// How many nodes affect his vertex?
                    cTotalBones = pSkinData->GetNumAssignedBones( iVertex );
                    if( cTotalBones > 4 )
                        continue;

					for (iBone = 0; iBone < cTotalBones; iBone++ )
					{
                        int	boneNr = pSkinData->GetAssignedBone( iVertex, iBone );
                        if( boneNr < 0 )
		                    continue;

                        pBoneNode = pSkin->GetBone( boneNr );
                        if( !pBoneNode )
                            continue;

						// first see if the bone has already been seen
						bBoneSeen = FALSE;
						for (iBoneSeen = 0; iBoneSeen < cpnBonesSeen; iBoneSeen++)
						{
							if (rgpnBonesSeen[iBoneSeen] == pBoneNode)
							{
								bBoneSeen = TRUE;
								break;
							}
						}
                    
						// if not seen, collect stats and add to already seen
						if (!bBoneSeen)
						{
							if (cpnBonesSeen >= cpnBonesSeenMax)
							{
								rgpnTemp = rgpnBonesSeen;
								cpnBonesSeenMax *= 2;

								rgpnBonesSeen = new INode*[cpnBonesSeenMax];
								if (rgpnBonesSeen == NULL)
								{
									rgpnBonesSeen = rgpnTemp;
									hr = E_OUTOFMEMORY;
									goto e_Exit;
								}

								memcpy(rgpnBonesSeen, rgpnTemp, cpnBonesSeen * sizeof(INode*));
								delete []rgpnTemp;
							}

							rgpnBonesSeen[cpnBonesSeen] = pBoneNode;
							cpnBonesSeen += 1;
						}
					}
				}

				ppc->m_cMaxWeightsPerFace = max(cpnBonesSeen, ppc->m_cMaxWeightsPerFace);
            }

e_NoSkin:
            pSkinMod->ReleaseInterface( I_SKIN, pSkin );
        }
    }

    cChildren = pNode->NumberOfChildren();
    for (iChild = 0; iChild < cChildren; iChild++)
    {
        // enumerate the child
        hr = Preprocess(ppc, pNode->GetChildNode(iChild));
        if (FAILED(hr))
            goto e_Exit;        
    }

e_Exit:
    if (bDeleteTriObject)
    {
        delete pTriObject;
    }

    delete []rgpnBonesSeen;
    return hr;
}


struct SAnimInfo
{
    DWORD dwTime;
    DWORD dwNumValues;
    float rgfValues[16];
};

BOOL BFloatsEqual
    (
    float f1,
    float f2
    )
{
    // first do a bitwise compare
    if ((*(DWORD*)&f1) == (*(DWORD*)&f2))
        return TRUE;

    // next do an epsilon compare
    float fDiff = (f1 - f2);
    return (fDiff <= 1e-6f) && (fDiff >= -1e-6f);
}

BOOL BEqualMatrices(float *rgfMat1, float *rgfMat2)
{
    DWORD iFloat;

    for (iFloat = 0; iFloat < 16; iFloat++)
    {
        if (!BFloatsEqual(rgfMat1[iFloat], rgfMat2[iFloat]))
            return FALSE;
    }

    return TRUE;
}

HRESULT GenerateAnimationSet
    (
    SSaveContext *psc
    )
{
    HRESULT hr = S_OK;
    DWORD cKeys;
    PBYTE pbData = NULL;
    PBYTE pbCur;
    DWORD cTicksPerFrame;
    DWORD iCurTime;
    DWORD iCurKey;
    Matrix3 matFirstTM;
    Matrix3 matTM;
    Matrix3 matRelativeTM;
    Matrix3 matParentTM;
	Interval interval;
    DWORD iNode;
    INode *pNode = NULL;
    DWORD cbSize;
    DWORD iKey;
    DWORD cCurrentKeys;
    SAnimInfo *rgaiAnimData = NULL;
    SAnimInfo *rgaiAnimDataCur;
    LPDIRECTXFILEDATA pAnimation = NULL;
    LPDIRECTXFILEDATA pAnimationKeys = NULL;
    char *szName;
    BOOL bAddEndKey = FALSE;
    DWORD iInterval;
    DWORD iStartTime;
    DWORD iEndTime;
    DWORD cFrameRate;

    // find the animation info from the interface
    interval = psc->m_pInterface->GetAnimRange();
    cTicksPerFrame = GetTicksPerFrame();
    cFrameRate = GetFrameRate();
	iStartTime = psc->m_iAnimStart * GetTicksPerFrame();
	iEndTime = psc->m_iAnimEnd * GetTicksPerFrame();

    iInterval = (cTicksPerFrame * cFrameRate) / psc->m_iAnimSamplingRate;

    cKeys = (iEndTime - iStartTime) / iInterval;

    // add a key for the last frame at iEndTime, or the one added if iEndTime is not directly touched
    cKeys += 1;

    // add a key to give the final frame a length (difference between Max and X file keyframe specification)
    cKeys += 1;

    // if the sampling frequency doesn't end directly on 
    //   on the end time, then add a partial end key
    if (((iEndTime - iStartTime) % iInterval) != 0)
    {
        bAddEndKey = TRUE;
    }

    rgaiAnimData = new SAnimInfo[psc->m_cNodes*cKeys];
    if (rgaiAnimData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	for (iCurKey = 0, iCurTime = iStartTime; iCurTime <= iEndTime; iCurTime += iInterval, iCurKey++ )
	{
        for (iNode = 0; iNode < psc->m_cNodes; iNode++)
        {
            pNode = psc->m_rgpnNodes[iNode];

		    //iCurTime = iFrame * (cTicksPerFrame / 2);
		    
			matTM = pNode->GetNodeTM(iCurTime);
			
            if (pNode->GetParentNode() == NULL)
            {
                matRelativeTM = matTM;

				//	EDITED :
				//
				if ( psc->m_bLeftHandAxis )
				{
					RHMatToLHMat ( matRelativeTM, matRelativeTM );
				}
            }
            else
            {
                matParentTM = pNode->GetParentNode()->GetNodeTM(iCurTime);
                if( memcmp(&matTM,&matParentTM,12*sizeof(float)) == 0 )
                {
                    matRelativeTM.IdentityMatrix();
                }
                else
                {
					//	EDITED :
					//
					if ( psc->m_bLeftHandAxis )
					{
						Matrix3 matTMlh;
						Matrix3 matParentTMlh;

						RHMatToLHMat ( matTM, matTMlh );
						RHMatToLHMat ( matParentTM, matParentTMlh );

						matRelativeTM = matTMlh * Inverse(matParentTMlh);
					}
					else
					{
						matRelativeTM = matTM * Inverse(matParentTM);
					}
                }
            }


            // set the current pointer to the correct buffer position
            pbCur = (PBYTE)&rgaiAnimData[iNode*cKeys + iCurKey];

            // first write the time and dword count            
            WRITE_DWORD(pbCur, iCurTime - iStartTime);
            WRITE_DWORD(pbCur, 16);


            // next write the matrix
            WRITE_MATRIX4_FROM_MATRIX3(pbCur, matRelativeTM);
        }
    }

    // if the sampling rate doesn't evenly end on the last frame, add a partial key frame
    if ( bAddEndKey )
    {
        assert(((iEndTime - iStartTime) % iInterval) != 0);

        // just add the end time as a key frame
        for (iNode = 0; iNode < psc->m_cNodes; iNode++)
        {
            // set the current pointer to the correct buffer position
            pbCur = (PBYTE)&rgaiAnimData[iNode*cKeys +iCurKey];

		    matTM = pNode->GetNodeTM(iEndTime);
            if (pNode->GetParentNode() == NULL)
            {
                matRelativeTM = matTM;

				//	EDITED :
				//
				if ( psc->m_bLeftHandAxis )
				{
					RHMatToLHMat ( matRelativeTM, matRelativeTM );
				}
            }
            else
            {
                matParentTM = pNode->GetParentNode()->GetNodeTM(iEndTime);
                if( memcmp(&matTM,&matParentTM,12*sizeof(float)) == 0 )
                {
                    matRelativeTM.IdentityMatrix();
                }
                else
                {
					//	EDITED :
					//
					if ( psc->m_bLeftHandAxis )
					{
						Matrix3 matTMlh;
						Matrix3 matParentTMlh;

						RHMatToLHMat ( matTM, matTMlh );
						RHMatToLHMat ( matParentTM, matParentTMlh );

						matRelativeTM = matTMlh * Inverse(matParentTMlh);
					}
					else
					{
						matRelativeTM = matTM * Inverse(matParentTM);
					}
                }
            }

            WRITE_DWORD(pbCur, iEndTime - iStartTime);
            WRITE_DWORD(pbCur, 16);

            // next write the matrix
            WRITE_MATRIX4_FROM_MATRIX3(pbCur, matRelativeTM);
        }

        // update iCurKey for the looping key frames
        iCurKey += 1;
    }

	// add an final animation frame to allow for looping - Max assumes that the last frame lasts for an interval...  X-files do not, so specify it explicitly
    //    if looping is not desired, then this should be the final frames data instead of the first frames data
    for (iNode = 0; iNode < psc->m_cNodes; iNode++)
    {
		// set the current pointer to the correct buffer position
		pbCur = (PBYTE)&rgaiAnimData[iNode*cKeys + iCurKey];
        if (psc->m_bLoopingAnimationData)
        {
            // copy the zero'th animations data, just update the time value
            memcpy(pbCur, (PBYTE)&rgaiAnimData[iNode*cKeys + 0], sizeof(SAnimInfo));
        }
        else
        {
            // copy the previous frames data to hold for a frames worth of time
            memcpy(pbCur, (PBYTE)&rgaiAnimData[iNode*cKeys + (iCurKey-1)], sizeof(SAnimInfo));
        }

		//	EDITED : cTicksPerFrame -> iInterval.
		//
        WRITE_DWORD(pbCur, iEndTime - iStartTime + iInterval);
    }

    assert(iCurKey + 1 == cKeys);    

    // allocate memory to send to D3DXOF lib
    cbSize = sizeof(DWORD) + sizeof(DWORD) +
            (sizeof(DWORD) //time
                + sizeof(DWORD) //nValues
                + sizeof(FLOAT)*16) // x, y, z
               * cKeys; // number of keys

    pbData = new BYTE[cbSize];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMAnimationSet,
                                    NULL,
                                    NULL,
                                    0,
                                    NULL,
                                    &psc->m_pAnimationSet
                                    );
    if (FAILED(hr))
        goto e_Exit;

	for (iNode = 0; iNode < psc->m_cNodes; iNode++)
	{
		//	Note : 에니메이션 키 유효성 검사.
		//
		BOOL bVaildAnimData = FALSE;
		rgaiAnimDataCur = &rgaiAnimData[iNode*cKeys];
		for ( iKey = 0; iKey < cKeys; iKey++ )
		{
			if ( iKey < (cKeys-1) )
			{
				if ( !BEqualMatrices(rgaiAnimDataCur[iKey].rgfValues, rgaiAnimDataCur[iKey+1].rgfValues) )
				{
					bVaildAnimData = TRUE;
					break;
				}
			}
		}

		if ( bVaildAnimData )
		{
			pbCur = pbData;

			szName = psc->m_stStrings.CreateNiceString(psc->m_rgpnNodes[iNode]->GetName());
			if (szName == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			// write the key type
			WRITE_DWORD(pbCur, 4);

			// write the number of keys
			WRITE_DWORD(pbCur, cKeys);

			rgaiAnimDataCur = &rgaiAnimData[iNode*cKeys];
			cCurrentKeys = 0;
			for ( iKey = 0; iKey < cKeys; iKey++ )
			{
				memcpy(pbCur, &rgaiAnimDataCur[iKey], sizeof(SAnimInfo));
				pbCur += sizeof(SAnimInfo);
				cCurrentKeys += 1;

				// if not last key, then check for start of a run of identical keys
				if ( iKey < (cKeys-1) )
				{
					// if equal to next, check for a run of equal matrices
					if ( BEqualMatrices(rgaiAnimDataCur[iKey].rgfValues, rgaiAnimDataCur[iKey+1].rgfValues) )
					{
						// move to the next key, and skip all equal matrices
						iKey += 1;
						while ( (iKey < (cKeys-1)) && BEqualMatrices(rgaiAnimDataCur[iKey].rgfValues, rgaiAnimDataCur[iKey+1].rgfValues) )
						{
							iKey += 1;
						}

						memcpy(pbCur, &rgaiAnimDataCur[iKey], sizeof(SAnimInfo));
						pbCur += sizeof(SAnimInfo);
						cCurrentKeys += 1;
					}
				}
			}

			// update to current count of keys
			((DWORD*)pbData)[1] = cCurrentKeys;

			hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMAnimation,
											NULL,
											NULL,
											0,
											NULL,
											&pAnimation
											);

			// add the data to the file
			hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMAnimationKey,
											NULL,
											NULL,
											cbSize,
											pbData,
											&pAnimationKeys
											);
			if (FAILED(hr))
				goto e_Exit;

			// add to the animation set
			hr = pAnimation->AddDataObject(pAnimationKeys);
			if (FAILED(hr))
				goto e_Exit;

			hr = pAnimation->AddDataReference(szName, NULL);
			if (FAILED(hr))
				goto e_Exit;

			hr = psc->m_pAnimationSet->AddDataObject(pAnimation);
			if (FAILED(hr))
				goto e_Exit;

			RELEASE(pAnimation);
			RELEASE(pAnimationKeys);
		}
	}

e_Exit:
	delete []rgaiAnimData;
	delete []pbData;
	RELEASE(pAnimation);
	RELEASE(pAnimationKeys);

	return hr;
}


template<class KeyType>
void ExportAnimationKeyLinear( IKeyControl* iKeyControl, DWORD** ppTime, DWORD& dwSize, DWORD& dwSizeMAX )
{
	KeyType ioKey;
	for (int i = 0; i < iKeyControl->GetNumKeys(); i++)
	{
		iKeyControl->GetKey(i, &ioKey);

        (*ppTime)[dwSize++] = ioKey.time;

        if ( dwSize >= dwSizeMAX )
        {
            // 데이터 백업
            DWORD* pTemp = new DWORD[dwSize];
            memcpy( pTemp, (*ppTime), sizeof(DWORD)*dwSize );

            // 데이터 삭제 및 복원
            delete [] (*ppTime);
            dwSizeMAX *= 2;
            (*ppTime) = new DWORD[dwSizeMAX];
            memcpy( (*ppTime), pTemp, sizeof(DWORD)*dwSize );

            // 백업데이터 삭제
            delete [] pTemp;
        }
	}
}

template<class KeyType>
void ExportAnimationKeyBezier( IKeyControl* iKeyControl, DWORD** ppTime, DWORD& dwSize, DWORD& dwSizeMAX )
{
	KeyType ioKey;
	for (int i = 0; i < iKeyControl->GetNumKeys(); i++)
	{
		iKeyControl->GetKey(i, &ioKey);
		
        (*ppTime)[dwSize++] = ioKey.time;

        if ( dwSize >= dwSizeMAX )
        {
            // 데이터 백업
            DWORD* pTemp = new DWORD[dwSize];
            memcpy( pTemp, (*ppTime), sizeof(DWORD)*dwSize );

            // 데이터 삭제 및 복원
            delete [] (*ppTime);
            dwSizeMAX *= 2;
            (*ppTime) = new DWORD[dwSizeMAX];
            memcpy( (*ppTime), pTemp, sizeof(DWORD)*dwSize );

            // 백업데이터 삭제
            delete [] pTemp;
        }
	}
}

template<class KeyType>
void ExportAnimationKeyTCB( IKeyControl* iKeyControl, DWORD** ppTime, DWORD& dwSize, DWORD& dwSizeMAX )
{
	KeyType ioKey;
	for (int i = 0; i < iKeyControl->GetNumKeys(); i++)
	{
		iKeyControl->GetKey(i, &ioKey);

		(*ppTime)[dwSize++] = ioKey.time;

        if ( dwSize >= dwSizeMAX )
        {
            // 데이터 백업
            DWORD* pTemp = new DWORD[dwSize];
            memcpy( pTemp, (*ppTime), sizeof(DWORD)*dwSize );

            // 데이터 삭제 및 복원
            delete [] (*ppTime);
            dwSizeMAX *= 2;
            (*ppTime) = new DWORD[dwSizeMAX];
            memcpy( (*ppTime), pTemp, sizeof(DWORD)*dwSize );

            // 백업데이터 삭제
            delete [] pTemp;
        }
	}
}

void ExportAnimationKeys( Animatable* pAnim, DWORD** ppTime, DWORD& dwSize, DWORD& dwSizeMAX )
{
	// Can we export this controller directly?
	IKeyControl* iKeyControl = GetKeyControlInterface(pAnim);
	if (iKeyControl == NULL)
	{
		// No keys on this controller, but maybe it uses sub-controllers
		// to derive its value
		for (int i = 0; i < pAnim->NumSubs(); i++)
		{
			Animatable* pSubCtrl = pAnim->SubAnim(i);
			MSTR sSubName = pAnim->SubAnimName(i);
			ExportAnimationKeys( pSubCtrl, ppTime, dwSize, dwSizeMAX );
		}
	}
	else
	{
		// This is a keyframe controller, we just have no data.
		if (iKeyControl->GetNumKeys() == 0) 
            return;

		// Here we test the class ID to see what type of
		// key frames this controller will hold
		Class_ID clid = pAnim->ClassID();
		switch(clid.PartA())
		{
		case LININTERP_FLOAT_CLASS_ID:
			ExportAnimationKeyLinear<ILinFloatKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case LININTERP_POSITION_CLASS_ID:
			ExportAnimationKeyLinear<ILinPoint3Key>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case LININTERP_ROTATION_CLASS_ID:
			ExportAnimationKeyLinear<ILinRotKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case LININTERP_SCALE_CLASS_ID:
			ExportAnimationKeyLinear<ILinScaleKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;

		case HYBRIDINTERP_FLOAT_CLASS_ID:
			ExportAnimationKeyBezier<IBezFloatKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case HYBRIDINTERP_POSITION_CLASS_ID:
			ExportAnimationKeyBezier<IBezPoint3Key>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case HYBRIDINTERP_ROTATION_CLASS_ID:
			// The HYBRIDINTERP rotation class is unique in that it doesn't use
			// bezier interpolation: We export this as a simple linear key
			ExportAnimationKeyLinear<IBezQuatKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case HYBRIDINTERP_SCALE_CLASS_ID:
			ExportAnimationKeyBezier<IBezScaleKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;

		case TCBINTERP_FLOAT_CLASS_ID:
			ExportAnimationKeyTCB<ITCBFloatKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case TCBINTERP_POSITION_CLASS_ID:
			ExportAnimationKeyTCB<ITCBPoint3Key>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case TCBINTERP_ROTATION_CLASS_ID:
			ExportAnimationKeyTCB<ITCBRotKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;
		case TCBINTERP_SCALE_CLASS_ID:
			ExportAnimationKeyTCB<ITCBScaleKey>( iKeyControl, ppTime, dwSize, dwSizeMAX );
			break;

		default:
			// If we hit this, we need to update our exporter to support this controller.
			//Write("ERROR: Unknown class exported under KeyFrames: (0x%X, 0x%X)", clid.PartA(), clid.PartB());
			break;
		}
	}
}

void DxQuickSort( int pArray[], int ilo, int ihi ) 
{
	float pivot;	// pivot value for partitioning array
	int ulo, uhi;	// indices at ends of unpartitioned region
	int ieq;		// least index of array entry with value equal to pivot
    int tempEntry;	// temporary entry used for swapping

	if (ilo >= ihi) return;

	// Select a pivot value.
	pivot = pArray[(ilo + ihi)/2];
	// Initialize ends of unpartitioned region and least index of entry
	// with value equal to pivot.
	ieq = ulo = ilo;
	uhi = ihi;
	// While the unpartitioned region is not empty, try to reduce its size.
	while( ulo <= uhi ) 
	{
		if( pArray[uhi] > pivot ) 
		{
			// Here, we can reduce the size of the unpartitioned region and
			// try again.
			uhi--;
		} 
		else 
		{
			// Here, A[uhi] <= pivot, so swap entries at indices ulo and
			// uhi.
			tempEntry = pArray[ulo];
			pArray[ulo] = pArray[uhi];
			pArray[uhi] = tempEntry;

			// After the swap, A[ulo] <= pivot.
			if (pArray[ulo] < pivot) 
			{
				// Swap entries at indices ieq and ulo.
				tempEntry = pArray[ieq];
				pArray[ieq] = pArray[ulo];
				pArray[ulo] = tempEntry;

				// After the swap, A[ieq] < pivot, so we need to change
				// ieq.
				ieq++;
				// We also need to change ulo, but we also need to do
				// that when A[ulo] = pivot, so we do it after this if
				// statement.
			}
			// Once again, we can reduce the size of the unpartitioned
			// region and try again.
			ulo++;
		}
	}

	// Now, all entries from index ilo to ieq - 1 are less than the pivot
	// and all entries from index uhi to ihi + 1 are greater than the
	// pivot.  So we have two regions of the array that can be sorted
	// recursively to put all of the entries in order.
	DxQuickSort( pArray, ilo, ieq - 1 );
	DxQuickSort( pArray, uhi + 1, ihi );
}

BOOL ExportAnimation( INode* pNode, int** ppKeyData, DWORD& dwKeySize )
{
	// How do we know this node is animated?
	// One way to tell is using validity intervals.
	// If the transform is valid forever, it never changes.
	Interval ivNodeValid = FOREVER;
	pNode->GetNodeTM(0, &ivNodeValid);
	if (ivNodeValid == FOREVER) 
        return FALSE;

	// Get the controller off the node.  If it is a TRS
	// see if we can export its keyframes
	Control* pNodeCtrl = pNode->GetTMController();
	Control* pSubCtrls[3];
	pSubCtrls[0] = pNodeCtrl->GetPositionController();
	pSubCtrls[1] = pNodeCtrl->GetRotationController();
	pSubCtrls[2] = pNodeCtrl->GetScaleController();

	// We should always prefer to export keyframes,
	// as it takes less space
	bool bSample = false;
	for (int i = 0; i < 3; i++)
	{
		Control* pCtrl = pSubCtrls[i];

		// This is possible if we are not TRS
		// (eg, an IK Matrix3 controller)
		if (pCtrl != NULL)
		{
			// We always export keyframes if possible
			if (pCtrl->IsKeyable()) continue;

			if (pCtrl->GetXController() != NULL &&
				pCtrl->GetYController() != NULL &&
				pCtrl->GetZController() != NULL)
			{
				// This control has XYZ dimensions -
				// we understand this, we can export the
				// subanim keyframes instead of the keyframes
				// directly on the controller.  If we didnt
				// have these dimensions, we couldnt assume
				// how the sub-anims are used.
				continue;
			}
		}

		// If we are here, we dont understand how one of the
		// TRS controllers derive its value.  In this case, we
		// have to sample the TRS to be sure we are correct
		bSample = true;
		break;
	}

	if (bSample)
	{
		//ExportAnimationSample(pNode, iTreeDepth);

        return FALSE;
	}
	else
	{
        DWORD dwSize = 0;
        DWORD dwSizeMAX = 10;
        DWORD* pTime = new DWORD[dwSizeMAX];

		ExportAnimationKeys( pSubCtrls[0], &pTime, dwSize, dwSizeMAX );
		ExportAnimationKeys( pSubCtrls[1], &pTime, dwSize, dwSizeMAX );
		ExportAnimationKeys( pSubCtrls[2], &pTime, dwSize, dwSizeMAX );


        // 중복된것 없앤다.
        dwKeySize = 0;
        (*ppKeyData) = new int[dwSize];
        for ( DWORD i=0; i<dwSize; ++i )
        {
            BOOL bInsert(TRUE);
            for ( DWORD j=0; j<dwKeySize; ++j )
            {
                if ( pTime[i] == (*ppKeyData)[j] )
                {
                    bInsert = FALSE;
                    break;
                }
            }

            if ( bInsert )
            {
                (*ppKeyData)[dwKeySize++] = pTime[i];
            }
        }

        // Quick Sort를 한다.
        DxQuickSort( (*ppKeyData), 0, dwKeySize-1 );

        delete [] pTime;
	}

    // 키가 없다면 만들지 않는다.
    if ( dwKeySize == 0 )
    {
        delete [] (*ppKeyData);

        return FALSE;
    }

    return TRUE;
}

HRESULT GenerateAnimationSetKey
    (
    SSaveContext *psc
    )
{
    HRESULT hr = S_OK;
    PBYTE pbData = NULL;
    PBYTE pbCur;
    DWORD iCurTime;
    Matrix3 matTM;
    Matrix3 matRelativeTM;
    Matrix3 matParentTM;
    INode *pNode;
    DWORD cbSize;
    LPDIRECTXFILEDATA pAnimation = NULL;
    LPDIRECTXFILEDATA pAnimationKeys = NULL;
    char *szName;

    hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMAnimationSet,
                                    NULL,
                                    NULL,
                                    0,
                                    NULL,
                                    &psc->m_pAnimationSet
                                    );
    if (FAILED(hr))
        goto e_Exit;

	for ( DWORD iNode = 0; iNode < psc->m_cNodes; iNode++)
	{
        int* pKeyData(NULL);
        DWORD dwKeySize(0);

		if ( ExportAnimation( psc->m_rgpnNodes[iNode], &pKeyData, dwKeySize ) )
		{
            // allocate memory to send to D3DXOF lib
            cbSize = sizeof(DWORD) + sizeof(DWORD) +
                    (sizeof(DWORD) //time
                        + sizeof(DWORD) //nValues
                        + sizeof(FLOAT)*16) // x, y, z
                       * dwKeySize; // number of keys

            pbData = new BYTE[cbSize];
            if (pbData == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }


            // 데이터 정리
            delete [] pKeyData;



			pbCur = pbData;

            pNode = psc->m_rgpnNodes[iNode];
			szName = psc->m_stStrings.CreateNiceString( pNode->GetName() );
			if (szName == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			// write the key type
			WRITE_DWORD(pbCur, 4);

			// write the number of keys
			WRITE_DWORD(pbCur, dwKeySize);

            for ( DWORD i=0 ;i<dwKeySize; ++i )
            {
                iCurTime = pKeyData[i];
                matTM = pNode->GetNodeTM( iCurTime );
    			
                if (pNode->GetParentNode() == NULL)
                {
                    matRelativeTM = matTM;

				    //	EDITED :
				    //
				    if ( psc->m_bLeftHandAxis )
				    {
					    RHMatToLHMat ( matRelativeTM, matRelativeTM );
				    }
                }
                else
                {
                    matParentTM = pNode->GetParentNode()->GetNodeTM(iCurTime);
                    if( memcmp(&matTM,&matParentTM,12*sizeof(float)) == 0 )
                    {
                        matRelativeTM.IdentityMatrix();
                    }
                    else
                    {
					    //	EDITED :
					    //
					    if ( psc->m_bLeftHandAxis )
					    {
						    Matrix3 matTMlh;
						    Matrix3 matParentTMlh;

						    RHMatToLHMat ( matTM, matTMlh );
						    RHMatToLHMat ( matParentTM, matParentTMlh );

						    matRelativeTM = matTMlh * Inverse(matParentTMlh);
					    }
					    else
					    {
						    matRelativeTM = matTM * Inverse(matParentTM);
					    }
                    }
                }

                // first write the time and dword count            
                WRITE_DWORD(pbCur, iCurTime);
                WRITE_DWORD(pbCur, 16);

                // next write the matrix
                WRITE_MATRIX4_FROM_MATRIX3(pbCur, matRelativeTM);
            }

			hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMAnimation,
											NULL,
											NULL,
											0,
											NULL,
											&pAnimation
											);

			// add the data to the file
			hr = psc->m_pxofsave->CreateDataObject(TID_D3DRMAnimationKey,
											NULL,
											NULL,
											cbSize,
											pbData,
											&pAnimationKeys
											);
			if (FAILED(hr))
				goto e_Exit;

			// add to the animation set
			hr = pAnimation->AddDataObject(pAnimationKeys);
			if (FAILED(hr))
				goto e_Exit;

			hr = pAnimation->AddDataReference(szName, NULL);
			if (FAILED(hr))
				goto e_Exit;

			hr = psc->m_pAnimationSet->AddDataObject(pAnimation);
			if (FAILED(hr))
				goto e_Exit;

			RELEASE(pAnimation);
			RELEASE(pAnimationKeys);

            delete [] pbData;
		}
	}

e_Exit:
    if ( pbData )
    {
        delete [] pbData;
    }
	RELEASE(pAnimation);
	RELEASE(pAnimationKeys);

	return hr;
}


// ================================================== CDataSave::CDataSave()
HRESULT ExportXFile(
    const TCHAR *szFilename,
    ExpInterface *pExportInterface,
    Interface *pInterface, 
    BOOL bSuppressPrompts,
    BOOL bSaveSelection,
    HWND hwndParent ) 
{
    LPDIRECTXFILE pxofapi = NULL;
    LPDIRECTXFILEDATA pRootData = NULL;
    LPDIRECTXFILESAVEOBJECT pxofsave = NULL; 
    HRESULT hr = S_OK;
    INode *pRootNode = NULL;
    SSaveContext sc;
    SPreprocessContext pc;
    SDialogOptions DlgOptions;
    bool bInterfaceStart = false;

    assert(szFilename && pExportInterface && pInterface);

    // Extract scene information
    bInterfaceStart = true;
    pInterface->ProgressStart(_T("Extracting skinning data"),TRUE,dummyFn,NULL);
    pInterface->ProgressUpdate(0);
    //pInterface->ProgressUpdate(100);

    // first find the root node
    hr = FindRootNode(pExportInterface->theScene, &pRootNode);
    if (FAILED(hr))
        goto e_Exit;

    // setup options for Preprocess stage
    pc.m_bSaveSelection = bSaveSelection;

    // figure out bone counts, etc.
    hr = Preprocess(&pc, pRootNode);
    if (FAILED(hr))
        goto e_Exit;

    // move the skin map data from the preprocess context to the save context
    sc.m_cpsmSkinMaps = pc.m_cpsmSkinMaps;
    sc.m_rgpsmSkinMaps = pc.m_rgpsmSkinMaps;
    pc.m_rgpsmSkinMaps = NULL;

    pInterface->ProgressUpdate(25);

    // setup dialog options
    DlgOptions.m_xFormat = DXFILEFORMAT_BINARY;
    DlgOptions.m_bSavePatchData = FALSE;
    DlgOptions.m_ExportDataOption = EXPD_FRAME|EXPD_ANIMATION;	//	EDITED : 파라메타 변경.
    DlgOptions.m_bLoopingAnimationData = TRUE;
    DlgOptions.m_iAnimSamplingRate = 30;
	DlgOptions.m_iAnimStart = pInterface->GetAnimRange().Start() / GetTicksPerFrame();
	DlgOptions.m_iAnimEnd = pInterface->GetAnimRange().End() / GetTicksPerFrame();
    DlgOptions.m_cMaxBonesPerVertex = pc.m_cMaxWeightsPerVertex;
    DlgOptions.m_cMaxBonesPerFace = pc.m_cMaxWeightsPerFace;

	//	EDITED : 추가 설정값.
	//
	DlgOptions.m_bLeftHandAxis = TRUE;
	DlgOptions.m_bHideBiped = TRUE;
	DlgOptions.m_bTexEncrypt = FALSE;

    if( bInterfaceStart )
    {
        bInterfaceStart = false;
        pInterface->ProgressEnd();
    }

    // if prompts not suppressed, then check with the user on options
    if (!bSuppressPrompts)
    {
        DialogBoxParam(
            g_hInstance,
            MAKEINTRESOURCE( IDD_PANEL ),
            hwndParent,
            XSkinExpOptionsDlgProc,
            (LPARAM)&DlgOptions );

        if (!DlgOptions.m_bProceedWithExport)
            goto e_Exit;
    }

    bInterfaceStart = true;
    pInterface->ProgressStart(_T("Exporting data"),TRUE,dummyFn,NULL);
    pInterface->ProgressUpdate(25);

    // Create xofapi object.
    hr = DirectXFileCreate(&pxofapi);
    if (FAILED(hr))
        goto e_Exit;

    // Register templates for d3drm.
    hr = pxofapi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES,
                                    D3DRM_XTEMPLATE_BYTES);
    if (FAILED(hr))
        goto e_Exit;

    hr = pxofapi->RegisterTemplates((LPVOID)XSKINEXP_TEMPLATES_MAX,
                                    strlen(XSKINEXP_TEMPLATES_MAX));
    if (FAILED(hr))
        goto e_Exit;

    // Create save object.
    hr = pxofapi->CreateSaveObject(szFilename,    // filename
                                   DlgOptions.m_xFormat,  // binary or text
                                   &pxofsave);
    if (FAILED(hr))
        goto e_Exit;

    hr = pxofsave->SaveTemplates(3, aIds);
    if (FAILED(hr))
        goto e_Exit;

    sc.m_pxofsave = pxofsave;
    sc.m_xFormat = DlgOptions.m_xFormat;
    sc.m_ExportDataOption = DlgOptions.m_ExportDataOption;			//	EDITED : 파라메타 변경.
    sc.m_bLoopingAnimationData = DlgOptions.m_bLoopingAnimationData;
    sc.m_iAnimSamplingRate = DlgOptions.m_iAnimSamplingRate;
	sc.m_iAnimStart = DlgOptions.m_iAnimStart;
	sc.m_iAnimEnd = DlgOptions.m_iAnimEnd;
	sc.m_bSavePatchData = DlgOptions.m_bSavePatchData;
    sc.m_iTime = pInterface->GetTime();
    sc.m_pInterface = pInterface;
    sc.m_bSaveSelection = bSaveSelection;
    sc.m_cMaxWeightsPerVertex = pc.m_cMaxWeightsPerVertex;
    sc.m_cMaxWeightsPerFace = pc.m_cMaxWeightsPerFace;

    sc.m_cNodes = pc.m_cNodes;
    sc.m_cNodesCur = 0;
    sc.m_rgpnNodes = new INode*[sc.m_cNodes];
    if (sc.m_rgpnNodes == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	//	EDITED : 추가 설정값.
	//
	sc.m_bLeftHandAxis = DlgOptions.m_bLeftHandAxis;
	sc.m_bHideBiped = DlgOptions.m_bHideBiped;
	sc.m_bTexEncrypt = DlgOptions.m_bTexEncrypt;

    // then write the whole tree out into file data's
    hr = EnumTree(&sc, pRootNode, &pRootData);
    if (FAILED(hr))
        goto e_Exit;

    if( bInterfaceStart )
    {
        bInterfaceStart = false;
        pInterface->ProgressEnd();
    }

    bInterfaceStart = true;
    pInterface->ProgressStart(_T("Saving data to file"),TRUE,dummyFn,NULL);
    pInterface->ProgressUpdate(50);

    // now save that file data to the file
    hr = pxofsave->SaveData(pRootData);
    if (FAILED(hr))
        goto e_Exit;

    pInterface->ProgressUpdate(75);

    if ( DlgOptions.m_ExportDataOption&EXPD_ANIMATION )
    {
        if( bInterfaceStart )
        {
            bInterfaceStart = false;
            pInterface->ProgressEnd();
        }

        bInterfaceStart = true;
        pInterface->ProgressStart(_T("Saving animation data to file"),TRUE,dummyFn,NULL);
        pInterface->ProgressUpdate(75);

        hr = GenerateAnimationSet(&sc);
        if (FAILED(hr))
            goto e_Exit;

        hr = pxofsave->SaveData(sc.m_pAnimationSet);
        if (FAILED(hr))
        {
            OutputDebugString("Failed to add animation set to X File\n");
            goto e_Exit;
        }            
    }
    else if ( DlgOptions.m_ExportDataOption&EXPD_KEY_ANI )
    {
        if( bInterfaceStart )
        {
            bInterfaceStart = false;
            pInterface->ProgressEnd();
        }

        bInterfaceStart = true;
        pInterface->ProgressStart(_T("Saving animation data to file"),TRUE,dummyFn,NULL);
        pInterface->ProgressUpdate(75);

        hr = GenerateAnimationSetKey(&sc);
        if (FAILED(hr))
            goto e_Exit;

        hr = pxofsave->SaveData(sc.m_pAnimationSet);
        if (FAILED(hr))
        {
            OutputDebugString("Failed to add animation set to X File\n");
            goto e_Exit;
        }            
    }

e_Exit:
    if (FAILED(hr))
    {
        OutputDebugString("File was not successfully exported.");
        {
            TCHAR errstr[500 + _MAX_PATH];
            _stprintf(errstr,"Could not write to file: %s.\n"\
                "Try checking the file's permissions, or if it is currently open.",szFilename);
            MessageBox(hwndParent,errstr,_T("Error!"),MB_OK);
        }
    }

    // falling through
    // Free up outstanding interfaces
    RELEASE(pxofsave);
    RELEASE(pRootData);
    RELEASE(pxofapi);

    if( bInterfaceStart )
    {
        pInterface->ProgressUpdate( 100 );
        pInterface->ProgressEnd();
    }

    return hr;
}


/*
BOOL EncryptTexture( const char* szInput, const char* szOutput )
{
	// 파일오픈
	FILE* pFileInput = NULL;
	pFileInput = _fsopen( szInput, "rb", _SH_DENYNO );
	if ( !pFileInput )
	{
		return FALSE;
	}

	fseek ( pFileInput, 0, SEEK_END );
	int nSize = ftell ( pFileInput );

	fseek ( pFileInput, 0, SEEK_SET );	
	
	// 헤더추가	
	BYTE* pBuffer = new BYTE[TEX_HEADER_SIZE+nSize];

	int nVersion = TEX_VERSION;
	int nFileType = TEXTURE_DDS;

	char szFileExt[MAX_PATH] = {0};
	
	char *szTextureFileName = strrchr ( szInput, '.' );
	if ( szTextureFileName )	strcpy( szFileExt, szTextureFileName );
	
	for ( int i = 0; i < strlen(szFileExt); i++ )	tolower( szFileExt[i] );

	if ( !strcmp ( szFileExt, g_szFileExt[TEXTURE_DDS] ) )	nFileType = TEXTURE_DDS;
	else if ( !strcmp ( szFileExt, g_szFileExt[TEXTURE_TGA] ) ) nFileType = TEXTURE_TGA;
	
	memcpy( pBuffer, &nVersion, sizeof( int ) );
	memcpy( pBuffer+4 , &nSize, sizeof(int) );
	memcpy( pBuffer+8, &nFileType, sizeof(int) );	
	
	// 붙이기
	fread( pBuffer+TEX_HEADER_SIZE, sizeof(BYTE), nSize, pFileInput );
	fclose( pFileInput );

	// XOR변환
	for ( int i = 0; i < nSize; ++i )
	{
		pBuffer[TEX_HEADER_SIZE+i] = pBuffer[TEX_HEADER_SIZE+i] ^ TEX_XOR_DATA;
		pBuffer[TEX_HEADER_SIZE+i] -= TEX_DIFF_DATA;
	}

	// 파일저장
	FILE* pFileOutput = NULL;
	pFileOutput = _fsopen( szOutput, "wb", _SH_DENYNO );
	if ( !pFileOutput )
	{
		delete[] pBuffer;
		return FALSE;
	}
	
	fwrite( pBuffer, sizeof(BYTE), nSize+TEX_HEADER_SIZE, pFileOutput );
	fclose( pFileOutput );

	delete[] pBuffer;
	return TRUE;
}
*/