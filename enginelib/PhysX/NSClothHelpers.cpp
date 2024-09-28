#include "pch.h"

#include "../../../RanThirdParty/PhysX_3_2/include/PxPhysicsAPI.h"

#include "./PxTkStream.h"

#include "NSClothHelpers.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSClothHelpers
{
	////////////////////////////////////////////////////////////////////////////////
	// fill cloth mesh descriptor from vertices and primitives
	void fillClothMeshDesc( std::vector<PxVec3> &vertexBuffer, std::vector<WORD>& primitiveBuffer, PxClothMeshDesc &meshDesc )
	{
		int numVertices = vertexBuffer.size();
		int numTriangles = primitiveBuffer.size() / 3;

		// convert vertex array to PxBoundedData (desc.points)
		meshDesc.points.data = &vertexBuffer[0];
		meshDesc.points.count = static_cast<PxU32>(numVertices);
		meshDesc.points.stride = sizeof(PxVec3);

		// convert face index array to PxBoundedData (desc.triangles)
		meshDesc.triangles.data = &primitiveBuffer[0];
		meshDesc.triangles.count = static_cast<PxU32>(numTriangles); 
		meshDesc.triangles.stride = sizeof(WORD) * 3; // <- stride per triangle

		meshDesc.flags |= PxMeshFlag::e16_BIT_INDICES;
	}


	//////////////////////////////////////////////////////////////////////////
	// AttachParentBone 의 Scale 변화에 따른 Cloth 에서 작업해줘야 할 것 이해.
	//////////////////////////////////////////////////////////////////////////
	// 
	// 기본. cloth 는 Bone의 기준점(Bone_InverseMatrix)이 되도록 해주고,
	//		LockParticle 을 실제 좌표로 이동시켜주면, PhysX 상에서 계산해주고, 그걸 받아서 렌더링하게 된다.
	//    실제 공간자체에서 계산 및 렌더링 된다는 이야기임.
	//
	// 1. 원본이 작아지면 Bone의 기준점으로 옮겨졌을 때의 cloth 도 작아져야 한다.
	//  ( 원본이 작아지면서 arrayBoneMatrices 값들은 모두 사이즈가 작게 되어있다. )
	// 2. 하지만 Bone_InverseMatrix 를 하면 실제 크기로 돌아오기 때문에, AttachParentBone 의 Scale 만큼 작게 만들어야 한다.
	// 3. LockParticle 은 Bone_Matrix 와 연산이 되기 때문에, 작게 만들면 안되고, 사이즈가 원본 사이즈로 되어 있어야 한다.
	//
	//////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////
	// create cloth mesh from obj file (user must provide vertex, primitive, and optionally texture coord buffer)
	bool createMeshFromObj(LPD3DXMESH pMesh, const D3DXMATRIX* arrayBoneMatrices, const D3DXMATRIX& matWorld,
							DWORD dwNumInfl, DWORD dwNumPaletteEntries, CONST D3DVERTEXELEMENT9 *pDeclaration, PxReal scale, const PxQuat* rot, const PxVec3* offset, 
							std::vector<PxVec3>& vertexBuffer, std::vector<WORD>& primitiveBuffer,
							PxClothMeshDesc &meshDesc)
	{
		PxVec3 myOffset(0.0f);
		PxQuat myRot = PxQuat::createIdentity();

		if (offset)
			myOffset = *offset;

		if (rot)
			myRot = *rot;

		UINT nVertSize = D3DXGetDeclVertexSize( pDeclaration, 0 );

		int numVertices	= pMesh->GetNumVertices();
		int numPrimitives = pMesh->GetNumFaces();

		vertexBuffer.resize(numVertices);
		primitiveBuffer.resize(numPrimitives*3);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
		LPD3DVERTEXELEMENT9 pDeclCur;
		HRESULT hr = pMesh->GetDeclaration(pDecl);
		if (FAILED(hr))
			return false;

		UINT unOffset_BlendWeight(0);
		UINT unOffset_BlendIndices(0);
		pDeclCur = pDecl;
		while (pDeclCur->Stream != 0xff)
		{
			if ( pDeclCur->Usage == D3DDECLUSAGE_BLENDWEIGHT )
			{
				unOffset_BlendWeight = pDeclCur->Offset;
			}
			else if ( pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES )
			{
				unOffset_BlendIndices = pDeclCur->Offset;
			}
			pDeclCur++;
		}

		//////////////////////////////////////////////////////////////////////////
		D3DXMATRIX* arrayBoneIdentity(NULL);
		if ( dwNumPaletteEntries )
		{
			D3DXMATRIX matInverse;
			arrayBoneIdentity = new D3DXMATRIX[dwNumPaletteEntries];
			D3DXMatrixInverse( &matInverse, NULL, &matWorld );
			for ( DWORD i=0; i<dwNumPaletteEntries; ++i )
			{
				D3DXMatrixMultiply ( &arrayBoneIdentity[i], &arrayBoneMatrices[i], &matInverse );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//모든 파티클 정점 정보를 Vertex Buffer로 복사한다.
		float fLastWeight(0.f);
		BYTE* pBufferVB(NULL);
		PxVec3 *vDest = (PxVec3*)&vertexBuffer[0];
		const float* arrayBlendWeight(NULL);
		const DWORD* pBlendIndices(NULL);
		DWORD arrayBlendIndices[4];
		const D3DXVECTOR3* pPosSrc(NULL);
		D3DXVECTOR3 vPos(0.f,0.f,0.f);
		D3DXVECTOR3 vNewPos(0.f,0.f,0.f);
		pMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pBufferVB );
		for( int i = 0 ; i < numVertices ; i++, vDest++ )
		{
			pPosSrc = reinterpret_cast<const D3DXVECTOR3*>( pBufferVB+(i*nVertSize) );
			arrayBlendWeight = reinterpret_cast<const float*>( pBufferVB+(i*nVertSize)+unOffset_BlendWeight );
			pBlendIndices = reinterpret_cast<const DWORD*>( pBufferVB+(i*nVertSize)+unOffset_BlendIndices );

			arrayBlendIndices[0] = (*pBlendIndices & 0xff);
			arrayBlendIndices[1] = (*pBlendIndices & 0xff00) >> 8;
			arrayBlendIndices[2] = (*pBlendIndices & 0xff0000) >> 16;
			arrayBlendIndices[3] = (*pBlendIndices & 0xff000000) >> 24;

			vPos = D3DXVECTOR3(0.f,0.f,0.f);
			fLastWeight = 0.f;
			for ( DWORD dwBone = 0; dwBone < dwNumInfl-1; ++dwBone )
			{
				fLastWeight = fLastWeight + arrayBlendWeight[dwBone];
				D3DXVec3TransformCoord( &vNewPos, pPosSrc, &arrayBoneIdentity[arrayBlendIndices[dwBone]] );
				vPos += vNewPos * arrayBlendWeight[dwBone];
			}

			// Now that we have the calculated weight, add in the final influence
			fLastWeight = 1.0f - fLastWeight;
			D3DXVec3TransformCoord( &vNewPos, pPosSrc, &arrayBoneIdentity[arrayBlendIndices[dwNumInfl-1]] );
			vPos += vNewPos * fLastWeight;

			//////////////////////////////////////////////////////////////////////////
			// AttachParentBone 의 Scale 변화 의 1.2 설명 ( * scale )
			//////////////////////////////////////////////////////////////////////////
			// 1. 원본이 작아지면 Bone의 기준점으로 옮겨졌을 때의 cloth 도 작아져야 한다.
			//  ( 원본이 작아지면서 arrayBoneMatrices 값들은 모두 사이즈가 작게 되어있다. )
			// 2. 하지만 Bone_InverseMatrix 를 하면 실제 크기로 돌아오기 때문에, AttachParentBone 의 Scale 만큼 작게 만들어야 한다.
			//////////////////////////////////////////////////////////////////////////
			//
			// 최종값을 넣어준다.
			vDest->x = vPos.x * scale;
			vDest->y = vPos.y * scale;
			vDest->z = vPos.z * scale;
		}
		pMesh->UnlockVertexBuffer();

		SAFE_DELETE_ARRAY ( arrayBoneIdentity );

		//////////////////////////////////////////////////////////////////////////
		//physx::shdfnd::fastMemcpy((PxU32*)primitiveBuffer.begin(), wo.mIndices, sizeof(PxU32)*numPrimitives*3);
		BYTE* pBufferIB(NULL);
		pMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pBufferIB );
		memcpy( &primitiveBuffer[0], pBufferIB, sizeof(WORD)*numPrimitives*3 );
		pMesh->UnlockIndexBuffer();

		//////////////////////////////////////////////////////////////////////////
		// fill the physx cloth mesh descriptor 
		fillClothMeshDesc( vertexBuffer, primitiveBuffer, meshDesc );

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	PxClothFabric* createFabric(PxPhysics &physics, PxCooking &cooking, const PxClothMeshDesc &desc, const PxVec3& gravityDir)
	{
		// In this example, we cook the fabric on the fly through a memory stream
		// Note that we can also use a file stream and pre-cook the mesh to save the cooking time
		PxToolkit::MemoryOutputStream wb;
		PX_ASSERT(desc.isValid());

		// cook the fabric data into memory buffer (cooking time operation)
		if (!cooking.cookClothFabric(desc, gravityDir, wb))
			return 0;

		// read fabric from memory stream (runtime operation)
		PxToolkit::MemoryInputData rb(wb.getData(), wb.getSize());
		return physics.createClothFabric(rb);
	}

	////////////////////////////////////////////////////////////////////////////////
	bool createDefaultParticles(const PxClothMeshDesc& meshDesc, PxClothParticle* clothParticles, PxReal massPerParticle)
	{
		const PxVec3* srcPoints = reinterpret_cast<const PxVec3*>(meshDesc.points.data);
		for (PxU32 i = 0; i < meshDesc.points.count; i++)
		{
			clothParticles[i].pos = srcPoints[i]; 
			clothParticles[i].invWeight = 1.0f / massPerParticle;
		}		
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	namespace
	{
		static PxVec3 gVirtualParticleWeights[] = 
		{ 
			// center point
			PxVec3(1.0f / 3, 1.0f / 3, 1.0f / 3),

			// center of sub triangles
			PxVec3(2.0f / 3, 1.0f / 6, 1.0f / 6),
			PxVec3(1.0f / 6, 2.0f / 3, 1.0f / 6),
			PxVec3(1.0f / 6, 1.0f / 6, 2.0f / 3),

			// edge mid points
			PxVec3(1.0f / 2, 1.0f / 2, 0.0f),
			PxVec3(0.0f, 1.0f / 2, 1.0f / 2),
			PxVec3(1.0f / 2, 0.0f, 1.0f / 2),

			// further subdivision of mid points
			PxVec3(1.0f / 4, 3.0f / 4, 0.0f),
			PxVec3(3.0f / 4, 1.0f / 4, 0.0f),

			PxVec3(0.0f, 1.0f / 4, 3.0f / 4),
			PxVec3(0.0f, 3.0f / 4, 1.0f / 4),

			PxVec3(1.0f / 4, 0.0f, 3.0f / 4),
			PxVec3(3.0f / 4, 0.0f, 1.0f / 4)
		};
	}

	////////////////////////////////////////////////////////////////////////////////
	bool createVirtualParticles(PxCloth& cloth, PxClothMeshDesc& meshDesc, int numSamples)
	{
		if(!numSamples)
			return false;

		PxU32 numFaces = meshDesc.triangles.count;
		PxU8* triangles = (PxU8*)meshDesc.triangles.data;

		PxU32 numParticles = numFaces * numSamples;
		SampleArray<PxU32> virtualParticleIndices;
		virtualParticleIndices.reserve(4 * numParticles);

		for (PxU32 i = 0; i < numFaces; i++)
		{
			for (int s = 0; s < numSamples; ++s)
			{
				PxU32 v0, v1, v2;

				if (meshDesc.flags & PxMeshFlag::e16_BIT_INDICES)
				{
					PxU16* triangle = (PxU16*)triangles;
					v0 = triangle[0];
					v1 = triangle[1];
					v2 = triangle[2];
				}
				else
				{
					PxU32* triangle = (PxU32*)triangles;
					v0 = triangle[0];
					v1 = triangle[1];
					v2 = triangle[2];
				}

				virtualParticleIndices.pushBack(v0);
				virtualParticleIndices.pushBack(v1);
				virtualParticleIndices.pushBack(v2);
				virtualParticleIndices.pushBack(s);
			}
			triangles += meshDesc.triangles.stride;
		}

		cloth.setVirtualParticles(numParticles, virtualParticleIndices.begin(), numSamples, gVirtualParticleWeights);

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	bool getParticlePositions(PxCloth &cloth, SampleArray<PxVec3> &positions)
	{
		PxClothReadData* readData = cloth.lockClothReadData();
		if (!readData)
		{
			return false;
		}

		const PxClothParticle* particles = readData->particles;
		if (!particles)
			return false;

		PxU32 nbParticles = cloth.getNbParticles();
		positions.resize(nbParticles);
		for (PxU32 i = 0; i < nbParticles; i++) 
			positions[i] = particles[i].pos;

		readData->unlock();

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	bool setParticlePositions(PxCloth &cloth, const SampleArray<PxVec3> &positions, const float* arrayInvWeight, float fInvWeight_AddValue, bool useConstrainedOnly, bool useCurrentOnly)
	{
		PxU32 nbParticles = cloth.getNbParticles();
		if (nbParticles != positions.size())
			return false;

		PxClothReadData* readData = cloth.lockClothReadData();
		if (!readData)
			return false;

		const PxClothParticle* particles = readData->particles;
		if (!particles)
			return false;

		SampleArray<PxClothParticle> newParticles(nbParticles);
		for (PxU32 i = 0; i < nbParticles; i++) 
		{		
			bool constrained = ( particles[i].invWeight == 0.0f);

			if (!useConstrainedOnly || constrained)
			{
				newParticles[i].pos = positions[i];
				newParticles[i].invWeight = arrayInvWeight[i];
			}
			else
			{
				newParticles[i].pos = particles[i].pos;
				newParticles[i].invWeight = arrayInvWeight[i] + fInvWeight_AddValue;
			}
		}

		readData->unlock();

		if (useCurrentOnly)
			cloth.setParticles(newParticles.begin(), NULL);
		else
			cloth.setParticles(newParticles.begin(), newParticles.begin());


		return true;
	}
}