#include "pch.h"

#include "../../../SigmaCore/String/StringUtil.h"

#include "NSXFormatLoader.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSXFormatLoader
{
	TSTRING GetObjectName(LPD3DXFILEDATA pDataObj)
	{
		TSTRING strName(_T(""));
		char  *Name = NULL;
		DWORD  Size = 0;

		// Error checking
		if(pDataObj == NULL)
			return NULL;

		// Get the template name (if any)
		if(FAILED(pDataObj->GetName(NULL, &Size)))
			return NULL;

		// Allocate a name buffer and retrieve name
		if(Size) 
		{
			if( (Name = new char[Size]) != NULL )
			{
				pDataObj->GetName(Name, &Size);
				strName = Name;
				SAFE_DELETE_ARRAY ( Name );
			}
		}

		return strName;
	}

	HRESULT ParseObject_MaterialName( LPD3DXFILEDATA pDataObj, VEC_TSTRING& vecMaterialName )
	{
		//	Note : 오브젝트의 타입.
		GUID type;
		HRESULT hr = pDataObj->GetType ( &type );
		if (FAILED(hr))
			return hr;

		if( type == TID_D3DRMMaterial )
		{
			TSTRING strName = GetObjectName( pDataObj );
			sc::string::lowerString( strName );

			vecMaterialName.push_back ( strName );
		}

		ParseChildObjects_MaterialName( pDataObj, vecMaterialName );
		return S_OK;
	}

	BOOL ParseChildObjects_MaterialName( LPD3DXFILEDATA pDataObj, VEC_TSTRING& vecMaterialName )
	{
		HRESULT hr(S_OK);
		LPD3DXFILEDATA				pSubData = NULL;

		//	Note : 최상위 오브젝트를 읽어옴.
		//
		SIZE_T cChildren = (0);
		pDataObj->GetChildren ( &cChildren );
		for ( UINT iChild=0; iChild<cChildren; iChild++ )
		{
			hr = pDataObj->GetChild ( iChild, &pSubData );
			if ( FAILED(hr) )
				continue;

			hr = ParseObject_MaterialName( pSubData, vecMaterialName );
			SAFE_RELEASE( pSubData );
			if (FAILED(hr))
				continue;
		}


		//IDirectXFileObject        *pSubObj  = NULL;
		//IDirectXFileData		  *pSubData = NULL;
		//IDirectXFileDataReference *pDataRef = NULL;
		//BOOL                       ParseResult = TRUE;

		//// Scan for embedded templates
		//while(SUCCEEDED(pDataObj->GetNextObject(&pSubObj)))
		//{
		//	// Process embedded references
		//	if(SUCCEEDED(pSubObj->QueryInterface(
		//		IID_IDirectXFileDataReference,
		//		(void**)&pDataRef))) 
		//	{
		//		// Resolve the data object
		//		if(SUCCEEDED(pDataRef->Resolve(&pSubData))) 
		//		{
		//			// Parse the object, remembering the return code
		//			ParseObject_MaterialName( pSubData, vecMaterialName );
		//			SAFE_RELEASE( pSubData );
		//		}
		//		SAFE_RELEASE( pDataRef );

		//	} 
		//	else
		//	{
		//		// Process non-referenced embedded templates
		//		if(SUCCEEDED(pSubObj->QueryInterface(                     
		//			IID_IDirectXFileData,           
		//			(void**)&pSubData))) 
		//		{
		//			// Parse the object, remembering the return code
		//			ParseObject_MaterialName( pSubData, vecMaterialName );
		//			SAFE_RELEASE( pSubData );
		//		}
		//	}

		//	// Release the data object
		//	SAFE_RELEASE( pSubObj );
		//}

		return TRUE;
	}
}