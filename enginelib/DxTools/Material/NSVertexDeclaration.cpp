#include "pch.h"

#include "NSVertexDeclaration.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSVertexDeclaration
{
	BYTE GetUsageIndexFromSemantic(const char* pSemantic, std::string& kSemantic)
	{
		BYTE usageIndex = 0;

		kSemantic = pSemantic;
		//kSemantic = UtilityLib::ConvertA2T(pSemantic);
		std::transform(kSemantic.begin(), kSemantic.begin(), kSemantic.end(), toupper);

		TSTRING kIndex = kSemantic.substr(kSemantic.size() - 1);

		if (_istdigit(kIndex[0]))
		{
			//usageIndex = _tstoi(kIndex.c_str());
			usageIndex = atoi(kIndex.c_str());
			kSemantic.erase(kSemantic.size() - 1, 1);
		}

		return usageIndex;
	}

	BYTE GetUsageFromSemantic(const std::string& kSemantic)
	{
		const char* SEMANTIC_POSITION		= "POSITION";
		const char* SEMANTIC_POSITIONT 		= "POSITIONT";
		const char* SEMANTIC_NORMAL			= "NORMAL";
		const char* SEMANTIC_TEXCOORD		= "TEXCOORD";
		const char* SEMANTIC_COLOR			= "COLOR";
		const char* SEMANTIC_TANGENT		= "TANGENT";
		const char* SEMANTIC_BINORMAL		= "BINORMAL";
		const char* SEMANTIC_BLENDWEIGHT	= "BLENDWEIGHT";
		const char* SEMANTIC_BLENDINDICES	= "BLENDINDICES";

		// Default usage of texcoord, since this is very common
		BYTE usage = D3DDECLUSAGE_TEXCOORD;

		if (kSemantic == SEMANTIC_POSITION)
		{
			usage = D3DDECLUSAGE_POSITION;
		}
		else if (kSemantic == SEMANTIC_POSITIONT)
		{
			usage = D3DDECLUSAGE_POSITIONT;
		}
		else if (kSemantic == SEMANTIC_NORMAL)
		{
			usage = D3DDECLUSAGE_NORMAL;
		}
		else if (kSemantic == SEMANTIC_TEXCOORD)
		{
			usage = D3DDECLUSAGE_TEXCOORD;
		}
		else if (kSemantic == SEMANTIC_COLOR)
		{
			usage = D3DDECLUSAGE_COLOR;
		}
		else if (kSemantic == SEMANTIC_TANGENT)
		{
			usage = D3DDECLUSAGE_TANGENT;
		}
		else if (kSemantic == SEMANTIC_BINORMAL)
		{
			usage = D3DDECLUSAGE_BINORMAL;
		}
		else if (kSemantic == SEMANTIC_BLENDWEIGHT)
		{
			usage = D3DDECLUSAGE_BLENDWEIGHT;
		}
		else if (kSemantic == SEMANTIC_BLENDINDICES)
		{
			usage = D3DDECLUSAGE_BLENDINDICES;
		}

		return usage;
	}

	void GetFVF( DWORD& dwFVF, const char* pSemantic )
	{
		std::string strName = pSemantic;
		std::transform(strName.begin(), strName.begin(), strName.end(), toupper);

		if ( strName == "POSITION" )
		{
			dwFVF |= D3DFVF_XYZ;
		}
		else if ( strName == "NORMAL" )
		{
			dwFVF |= D3DFVF_NORMAL;
		}
		else if ( strName == "COLOR0" )
		{
			dwFVF |= D3DFVF_DIFFUSE;
		}
		else if ( strName == "COLOR1" )
		{
			dwFVF |= D3DFVF_SPECULAR;
		}
		else if ( strName == "TEXCOORD0" )
		{
			dwFVF += D3DFVF_TEX1;
		}
		else if ( strName == "TEXCOORD1" )
		{
			dwFVF += D3DFVF_TEX1;
		}
		else if ( strName == "TEXCOORD2" )
		{
			dwFVF += D3DFVF_TEX1;
		}
		else if ( strName == "TEXCOORD3" )
		{
			dwFVF += D3DFVF_TEX1;
		}
		else if ( strName == "BINORMAL" )
		{
			dwFVF += D3DFVF_TEX1;
		}
		else if ( strName == "TANGENT" )
		{
			dwFVF += D3DFVF_TEX1;
		}
	}

	BYTE GetSizeFromDesc(D3DXPARAMETER_DESC& kDesc, BYTE byUsage, BYTE& byType)
	{
		// Default to single float
		BYTE type = D3DDECLTYPE_FLOAT1;
		BYTE size = 4;

		// Find type
		switch (kDesc.Type)
		{
			// Determine general class
		case D3DXPT_FLOAT:
			{
				// Determine float size
				switch (kDesc.Columns)
				{
				case 0:
				case 1:
					{
						type = D3DDECLTYPE_FLOAT1;
						size = 4;
						break;
					}
				case 2:
					{
						type = D3DDECLTYPE_FLOAT2;
						size = 8;
						break;
					}
				case 3:
					{
						type = D3DDECLTYPE_FLOAT3;
						size = 12;
						break;
					}
				case 4:
					{
						// Determine if this is a color semantic
						if (byUsage == D3DDECLUSAGE_COLOR)
						{
							// If they want to use 128-bit colors, the name of the element will contain '128'
							// Otherwise, create a more compact 64-bit color.
							//if (UtilityLib::ConvertA2T(kDesc.Name).find(_T("128")) == TSTRING::npos)
							if (std::string(kDesc.Name).find(_T("128")) == std::string::npos)
							{
								type = D3DDECLTYPE_D3DCOLOR;
								size = 4;
								break;
							}	
						}
						else if (byUsage == D3DDECLUSAGE_POSITION)
						{
							type = D3DDECLTYPE_FLOAT3;
							size = 12;
							break;
						}
						else if (byUsage == D3DDECLUSAGE_BLENDINDICES)
						{
							//type = D3DDECLTYPE_UBYTE4;
							type = D3DDECLTYPE_D3DCOLOR;
							size = 4;
							break;
						}

						type = D3DDECLTYPE_FLOAT4;
						size = 16;
						break;
					}
				}
			}
		}

		byType = type;

		return size;
	}

	bool BuildVertexDeclaration( LPDIRECT3DDEVICEQ pd3dDevice, 
									ID3DXEffect* pFX, 
									const char* pSemantic, 
									DWORD& dwFVF, 
									LPDIRECT3DVERTEXDECLARATION9& pDCRT, 
									D3DVERTEXELEMENT9** ppVertElements,
									BOOL& bLightMap,
									BOOL& bLightMap_float4,
									int& nLightMapIndex,
									BOOL& bTangent, 
									BOOL& bBinormal )
	{
		bLightMap = FALSE;
		bLightMap_float4 = FALSE;
		nLightMapIndex = -1;

		// caution ascii
		D3DXHANDLE hVSInput = pFX->GetParameterBySemantic(NULL, pSemantic);

		if (hVSInput == 0)
		{
			assert(0);
			return false;
		}

		D3DXPARAMETER_DESC kDesc;

		if (pFX->GetParameterDesc(hVSInput, &kDesc) != S_OK)
		{
			assert(0);
			return false;
		}

		SAFE_DELETE_ARRAY ( (*ppVertElements) );
		(*ppVertElements) = new D3DVERTEXELEMENT9[kDesc.StructMembers + 1];

		if ( (*ppVertElements) == NULL )
		{
			assert(0);
			return false;
		}

		int nCurOffset = 0;

		dwFVF = 0L;
		bTangent = FALSE;
		bBinormal = FALSE;
		for (UINT i = 0; i < kDesc.StructMembers; ++i)
		{
			D3DXHANDLE hMember = pFX->GetParameter(hVSInput, i);

			if (hMember)
			{
				D3DXPARAMETER_DESC kMemberDesc;

				if (pFX->GetParameterDesc(hMember, &kMemberDesc) != S_OK)
				{
					assert(0);
					return false;
				}

				std::string kSemantic;

				BYTE byUsage		= 0;
				BYTE byUsageIndex	= 0;
				BYTE byType			= 0;
				BYTE bySize			= 0;

				if (kMemberDesc.Semantic != NULL)
				{
					GetFVF( dwFVF, kMemberDesc.Semantic );
					byUsageIndex = GetUsageIndexFromSemantic(kMemberDesc.Semantic, kSemantic);
				}

				byUsage	= GetUsageFromSemantic(kSemantic);
				bySize	= GetSizeFromDesc(kMemberDesc, byUsage, byType);

				(*ppVertElements)[i].Stream 	= 0;
				(*ppVertElements)[i].Method 	= D3DDECLMETHOD_DEFAULT;
				(*ppVertElements)[i].Offset 	= nCurOffset;
				(*ppVertElements)[i].Type		= byType;
				(*ppVertElements)[i].Usage		= byUsage;
				(*ppVertElements)[i].UsageIndex	= byUsageIndex;

				if ( std::string("m_vLightmapUV") == kMemberDesc.Name )
				{
					bLightMap = TRUE;
					bLightMap_float4 = FALSE;
					nLightMapIndex = byUsageIndex;
				}

				if ( std::string("m_vAlbedoXY_LMZW") == kMemberDesc.Name )
				{
					bLightMap = TRUE;
					bLightMap_float4 = TRUE;
					nLightMapIndex = byUsageIndex;
				}

				if ( std::string("TANGENT") == kSemantic )
				{
					bTangent = TRUE;
				}

				if ( std::string("BINORMAL") == kSemantic )
				{
					bBinormal = TRUE;
				}

				//pVertElements[i].Semantic	= kSemantic;
				//pVertElements[i].Size		= bySize;

				nCurOffset += bySize;
			}
		}

		(*ppVertElements)[kDesc.StructMembers].Stream		= 0xff;
		(*ppVertElements)[kDesc.StructMembers].Offset		= 0;
		(*ppVertElements)[kDesc.StructMembers].Type			= D3DDECLTYPE_UNUSED;
		(*ppVertElements)[kDesc.StructMembers].Method		= 0;
		(*ppVertElements)[kDesc.StructMembers].Usage		= 0;
		(*ppVertElements)[kDesc.StructMembers].UsageIndex	= 0;

		SAFE_RELEASE ( pDCRT );
		HRESULT hr = pd3dDevice->CreateVertexDeclaration( &(*ppVertElements)[0], &pDCRT );
		if (hr != S_OK)
		{
			assert(0);
			SAFE_DELETE_ARRAY ( (*ppVertElements) );
			return false;
		}

		return true;
	}
};