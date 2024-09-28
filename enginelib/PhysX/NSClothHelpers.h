#pragma once

#include <vector>
#include "./SampleArray.h"

namespace NSClothHelpers
{
	// create cloth mesh from obj file (user must provide vertex, primitive, and optionally texture coord buffer)
	bool createMeshFromObj(LPD3DXMESH pMesh, const D3DXMATRIX* arrayBoneMatrices, const D3DXMATRIX& matWorld,
							DWORD dwNumInfl, DWORD dwNumPaletteEntries, CONST D3DVERTEXELEMENT9 *pDeclaration, PxReal scale, const PxQuat* rot, const PxVec3* offset, 
							std::vector<PxVec3>& vertexBuffer, std::vector<WORD>& primitiveBuffer,
							PxClothMeshDesc &meshDesc);

	// create cloth fabric from mesh descriptor
	PxClothFabric* createFabric(PxPhysics &physics, PxCooking &cooking, const PxClothMeshDesc &desc, const PxVec3& gravityDir);

	// copy mesh points to initial position of the particle and assign default mass of 1
	bool createDefaultParticles(const PxClothMeshDesc& meshDesc, PxClothParticle* clothParticles, PxReal massPerParticle=1.0);

	// create virtual particle samples
	bool createVirtualParticles(PxCloth& cloth, PxClothMeshDesc& meshDesc, int numSamples);

	// get particle location from the cloth
	bool getParticlePositions( PxCloth& cloth, SampleArray<PxVec3> &positions);

	// set particle location from the cloth
	bool setParticlePositions( PxCloth& cloth, const SampleArray<PxVec3> &positions, const float* arrayInvWeight, float fInvWeight_AddValue, bool useConstrainedOnly = true, bool useCurrentOnly = true);
}