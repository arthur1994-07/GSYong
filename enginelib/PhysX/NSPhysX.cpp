#include "pch.h"

#include <set>

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <shlobj.h>

//#include "../../../RanThirdParty/PhysX_3_2/include/NxPhysics.h"
//#include "../../../RanThirdParty/PhysX_3_2/include/nxscene.h"
//#include "../../../RanThirdParty/PhysX_3_2/include/PhysXLoader.h"

#include "../../../RanThirdParty/PhysX_3_2/include/PxPhysicsAPI.h"
#include "../../../RanThirdParty/PhysX_3_2/include/extensions/PxSmoothNormals.h"

//#include "../DxLand/DxPiece/DxPiece.h"

#include "../../SigmaCore/File/FileUtil.h"

#include "../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../DxMeshs/SkinMesh/DxClothColl.h"
#include "../Common/StlFunctions.h"
#include "../Common/SUBPATH.h"
#include "../DxTools/EditMeshs.h"
#include "../DxTools/DxMethods.h"

#include "./NSClothHelpers.h"
#include "./NSPhysX_Def.h"
#include "./Joints.h"

#include "NSPhysX.h"

#include "../../SigmaCore/Math/Random.h"	// ����

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSPhysX
{
    using namespace physx;
	//NxPhysicsSDK*	g_pPhysicsSDK = NULL;
	//NxScene*		g_pScene = NULL;
	PxPhysics*		g_pPhysicsSDK = NULL;
	PxScene*		g_pScene = NULL;
	PxVec3          g_DefaultGravity(0.f,-9.81f*10.f,0.f);

	typedef std::set<physx::PxRigidActor*>	SET_NXACTOR;
	typedef SET_NXACTOR::iterator			SET_NXACTOR_ITER;
	typedef std::set<physx::PxJoint*>	SET_NXJOINT;
	typedef SET_NXJOINT::iterator		SET_NXJOINT_ITER;
	typedef std::set<physx::PxCloth*>	SET_NXCLOTH;
	typedef SET_NXCLOTH::iterator		SET_NXCLOTH_ITER;

	SET_NXACTOR	g_setNxActor;	// ���� Manager
	SET_NXJOINT	g_setNxJoint;	// ���� Manager
	SET_NXCLOTH	g_setNxCloth;	// ���� Manager

	// Focus actor
	physx::PxRigidActor* gSelectedActor = NULL;

	LPDIRECT3DDEVICEQ g_pd3dDevice = NULL;

	float fTime(0.f);

	void StartPhysics( float fElapsedTime )
	{
		// Start collision and dynamics for delta time since the last frame
		//g_pScene->simulate( fElapsedTime*4.f );
		//g_pScene->simulate( fElapsedTime*3.f );
		g_pScene->simulate( fElapsedTime );
		//g_pScene->flush();
		//g_pScene->flushStream();
	}

	BOOL IsPhysX()
	{
		return ( g_pPhysicsSDK ? ( g_pScene ? TRUE : FALSE ) : FALSE );
	}

	PxFoundation*			g_pFoundation(NULL);
	PxProfileZoneManager*	g_pProfileZoneManager(NULL);
	PxCooking*				g_pCooking(NULL);
	PxMaterial*				g_pMaterial(NULL);
	PxDefaultCpuDispatcher*	g_pCpuDispatcher(NULL);

	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;

	void SaveLog_InitNx( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pFullPath )
	{
		DWORD dwVer(1);

		sc::SerialFile SFile;
		BOOL bOPEN = SFile.OpenFile ( FOT_WRITE, pFullPath );
		if ( bOPEN )
		{
			SFile << dwVer;

			SFile.BeginBlock();
			{
				int nPhysX_OK(0);
				SFile << nPhysX_OK;		// ������ �����̶� �� �� ����.
			}
			SFile.EndBlock();

			SFile.CloseFile();
		}

		InitNx( pd3dDevice );

		bOPEN = SFile.OpenFile ( FOT_WRITE, pFullPath );
		if ( bOPEN )
		{
			SFile << dwVer;

			SFile.BeginBlock();
			{
				int nPhysX_OK(1);
				SFile << nPhysX_OK;		// ���� �����̴ٶ�� �� �� �ִ�.
			}
			SFile.EndBlock();

			SFile.CloseFile();
		}
	}

	void InitNx_AvoidUnknownError( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pAppName )
	{
		// ���� �ִ��� üũ�Ѵ�.
		// ������ InitNx ���� �ʴ´�. ( ���� PC )
		// ������ �����ϰ� InitNx �� �ϵ��� �ϰ� ������ �����Ѵ�.. ( ���� ���� PC )

		//////////////////////////////////////////////////////////////////////////
		// ��θ� �����.
		// CreteDirectory...
		TCHAR szSpecialFolder[MAX_PATH]={0};
		SHGetSpecialFolderPath( NULL, szSpecialFolder, CSIDL_PERSONAL, FALSE );

		std::string strFolderPath(szSpecialFolder);
		strFolderPath += pAppName;
		strFolderPath += _T("physx");

		std::string strPathname(strFolderPath);
		strPathname += _T("\\physx.log");

		//////////////////////////////////////////////////////////////////////////		
		sc::SerialFile SFile;
		BOOL bOPEN = SFile.OpenFile ( FOT_READ, strPathname.c_str() );
		if ( bOPEN )
		{
			//////////////////////////////////////////////////////////////////////////
			// ������ �ִ�.

			DWORD dwVer, dwBufferSize;
			SFile >> dwVer;
			SFile >> dwBufferSize;

			// ���� �ٸ��ٸ� Save �� ���� ���༭ ���������� ���鵵�� �Ѵ�.
			// �̷��� ���ϸ� ������ ���� �� ������ ������ �ʴ� �̻� �ʿ� ������ �Ұ����� ���� �ִ�.
			if ( !SFile.IsSameRemindLoadSize( dwBufferSize ) || (dwBufferSize==0) ) 
			{
				SFile.CloseFile();

				// ���� Save���� ������ �߻����� ���� �����Ƿ�(���� ����Ȯ��,������ BugTrap�� ����) �ϴ� �� Save �ϴ� �������� �Ѵ�.
				SaveLog_InitNx( pd3dDevice, strPathname.c_str() );

				return;
			}

			int nPhysX_OK;
			SFile >> nPhysX_OK;

			SFile.CloseFile();

			// ������ PhysX �� ������ �Ͽ��°�~?
			if ( nPhysX_OK )
			{
				//////////////////////////////////////////////////////////////////////////
				// ������ �Ǿ�����.
				InitNx( pd3dDevice );
				return;
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				// ������ ������ �Ǿ�����.
				return;
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			// ó�� �۾��ȴ�.

			// ������ ���ٸ� ������ ����� �ε��� �ϰ� ������ �ٽ� �Ѵ�.
			int fh;

			// ó���̸� ������ ���� �� �ִ�.
			CreateDirectory ( strFolderPath.c_str(), NULL );

			// ������ ���� ���鵵�� �Ѵ�.
			fh = _creat( strPathname.c_str(), _S_IREAD | _S_IWRITE );
			if( fh == -1 )
				perror( "Couldn't create data file" );
			else
			{
				printf( "Created data file.\n" );
				_close( fh );
			}

			SaveLog_InitNx( pd3dDevice, strPathname.c_str() );
		}
	}

	void InitNx( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		//Recording memory allocations is necessary if you want to 
		//use the memory facilities in PVD effectively.  Since PVD isn't necessarily connected
		//right away, we add a mechanism that records all outstanding memory allocations and
		//forwards them to PVD when it does connect.

		//This certainly has a performance and memory profile effect and thus should be used
		//only in non-production builds.
		bool recordMemoryAllocations = true;
#ifdef RENDERER_ANDROID
		const bool useCustomTrackingAllocator = false;
#else
		const bool useCustomTrackingAllocator = true;
#endif

		g_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback );
		if(!g_pFoundation)
		{
			//fatalError("PxCreateFoundation failed!");
			ReleaseNx();
			return;
		}

		g_pProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(g_pFoundation);
		if(!g_pProfileZoneManager)
		{
			//fatalError("PxProfileZoneManager::createProfileZoneManager failed!");
			ReleaseNx();
			return;
		}

		g_pPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *g_pFoundation, PxTolerancesScale(), recordMemoryAllocations, g_pProfileZoneManager);
		if(!g_pPhysicsSDK)
		{
			//fatalError("PxCreatePhysics failed!");
			ReleaseNx();
			return;
		}

		if(!PxInitExtensions(*g_pPhysicsSDK))
		{
			//fatalError("PxInitExtensions failed!");
			ReleaseNx();
			return;
		}

		g_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *g_pFoundation, PxCookingParams());
		if(!g_pCooking)
		{
			//fatalError("PxCreateCooking failed!");
			ReleaseNx();
			return;
		}

		// setup default material...
		g_pMaterial = g_pPhysicsSDK->createMaterial(0.5f, 0.5f, 0.5f);
		if(!g_pMaterial)
		{
			//fatalError("createMaterial failed!");
			ReleaseNx();
			return;
		}

		PX_ASSERT(NULL == g_pScene);

		PxSceneDesc sceneDesc(g_pPhysicsSDK->getTolerancesScale());
		sceneDesc.gravity = g_DefaultGravity;	//PxVec3(0.0f, -9.81f, 0.0f);


		if(!sceneDesc.cpuDispatcher)
		{
			DWORD mNbThreads = 1;
			g_pCpuDispatcher = PxDefaultCpuDispatcherCreate(mNbThreads);
			if(!g_pCpuDispatcher)
			{
				//fatalError("PxDefaultCpuDispatcherCreate failed!");
				ReleaseNx();
				return;
			}
			sceneDesc.cpuDispatcher	= g_pCpuDispatcher;
		}

		if(!sceneDesc.filterShader)
			sceneDesc.filterShader	= PxDefaultSimulationFilterShader;

#ifdef PX_PS3
		if(!sceneDesc.spuDispatcher)
		{
			mSpuDispatcher = PxDefaultSpuDispatcherCreate(PxGetPS3Extension().getSimulationTaskSet(),
				PxGetPS3Extension().getQueryTaskSet());
			if(!mSpuDispatcher)
			{
				fatalError("PxDefaultSpuDispatcherCreate failed!");
				ReleaseNx();
				return;
			}
			sceneDesc.spuDispatcher = mSpuDispatcher;
		}
#endif

		//sceneDesc.flags |= PxSceneFlag::eENABLE_TWO_DIRECTIONAL_FRICTION;
		//sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;//need to register the pcm code in the PxCreatePhysics
		//sceneDesc.flags |= PxSceneFlag::eENABLE_ONE_DIRECTIONAL_FRICTION;
		//sceneDesc.flags |= PxSceneFlag::eADAPTIVE_FORCE;
		//sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
		g_pScene = g_pPhysicsSDK->createScene(sceneDesc);
		if(!g_pScene)
		{
			//fatalError("createScene failed!");
			ReleaseNx();
			return;
		}

		PxSceneFlags flag = g_pScene->getFlags();


		g_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE,				1.0f);
		g_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES,	1.0f);
	}

	void GetPhysicsResults()
	{
		// Get results from g_pScene->simulate(gDeltaTime)
		//while (!g_pScene->fetchResults(true));
		//g_pScene->fetchResults(true);

		g_pScene->fetchResults(true);
	}

	void ReleaseNx()
	{
		if (g_pScene)
		{
			// g_setNxActor ����
			{
				SET_NXACTOR_ITER iter = g_setNxActor.begin();
				for ( ; iter!=g_setNxActor.end(); )
				{
					SET_NXACTOR_ITER iterDel = iter++;
					DeleteActor( (*iterDel) );
				}
			}

			// g_setNxJoint ����
			{
				SET_NXJOINT_ITER iter = g_setNxJoint.begin();
				for ( ; iter!=g_setNxJoint.end(); )
				{
					SET_NXJOINT_ITER iterDel = iter++;
					DeleteJoint( (*iterDel) );
				}
			}

			// g_setNxCloth ����
			{
				SET_NXCLOTH_ITER iter = g_setNxCloth.begin();
				for ( ; iter!=g_setNxCloth.end(); )
				{
					SET_NXCLOTH_ITER iterDel = iter++;
					PxCloth* pCloth = (*iterDel);
					DeleteCloth( &pCloth );
				}
			}			

			GetPhysicsResults();  // Make sure to fetchResults() before shutting down
			//g_pPhysicsSDK->releaseScene(*g_pScene);
			//g_pScene = NULL;
		}
		//if (g_pPhysicsSDK)  g_pPhysicsSDK->release();

		if ( g_pScene )
		{
			g_pScene->release();
			g_pScene = NULL;
		}

		if ( g_pCpuDispatcher )
		{
			g_pCpuDispatcher->release();
			g_pCpuDispatcher = NULL;
		}

		if ( g_pMaterial )
		{
			g_pMaterial->release();
			g_pMaterial = NULL;
		}

		if ( g_pCooking )
		{
			g_pCooking->release();
			g_pCooking = NULL;
		}

		if ( g_pPhysicsSDK )
		{
			PxCloseExtensions();

			g_pPhysicsSDK->release();
			g_pPhysicsSDK = NULL;
		}

		if ( g_pProfileZoneManager )
		{
			g_pProfileZoneManager->release();
			g_pProfileZoneManager = NULL;
		}

		if ( g_pFoundation )
		{
			g_pFoundation->release();
			g_pFoundation = NULL;
		}
	}

	void DeleteActor( physx::PxRigidActor* pNxActor )
	{
		//if( pNxActor && g_pScene )
		if( pNxActor && g_pPhysicsSDK )
		{
			SET_NXACTOR_ITER iter = g_setNxActor.find( pNxActor );
			if ( iter != g_setNxActor.end() )
				g_setNxActor.erase( pNxActor );

			pNxActor->release();
			pNxActor = NULL;
			//g_pScene->releaseActor( *pNxActor );
		}
	}

	PX_FORCE_INLINE void SetupDefaultRigidDynamic(PxRigidDynamic& body, bool kinematic=false)
	{
		body.setActorFlag(PxActorFlag::eVISUALIZATION, true);
		body.setAngularDamping(0.5f);
		body.setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, kinematic);
	}

	physx::PxRigidActor* createRigidActor(	const PxTransform& pose, const PxGeometry& geometry, PxMaterial& material,
											const PxFilterData* fd, const PxReal* density, const PxReal* mass, BOOL bCollsion )
	{
		const bool isDynamic = density && *density || mass && *mass;

		PxRigidActor* actor = isDynamic ? static_cast<PxRigidActor*>(g_pPhysicsSDK->createRigidDynamic(pose))
			: static_cast<PxRigidActor*>(g_pPhysicsSDK->createRigidStatic(pose));
		if(!actor)
			return NULL;

		PxShape* shape = actor->createShape(geometry, material);
		if(!shape)
		{
			actor->release();
			return NULL;
		}

		// �浹ó���� �Ѱų� ���ϴ�.
		shape->setFlag( PxShapeFlag::eSIMULATION_SHAPE,bCollsion ? true : false );

		if(fd)
			shape->setSimulationFilterData(*fd);

		if(isDynamic)
		{
			PxRigidDynamic* body = static_cast<PxRigidDynamic*>(actor);
			{
				if(density)
					PxRigidBodyExt::updateMassAndInertia(*body, *density);
				else
					PxRigidBodyExt::setMassAndUpdateInertia(*body, *mass);
			}
		}

		g_pScene->addActor(*actor);

		return actor;
	}

	physx::PxRigidActor* CreateBox(	const PxTransform& pose, const PxVec3& dimensions, const PxReal density, PxMaterial& m, const PxFilterData* fd, BOOL bCollsion )
	{
		return createRigidActor( pose, PxBoxGeometry(dimensions), m, fd, &density, NULL, bCollsion );
	}

	physx::PxRigidActor* CreateSphere(	const PxTransform& pose, PxReal ir, const PxReal density, PxMaterial& m, const PxFilterData* fd, BOOL bCollsion )
	{
		return createRigidActor( pose, PxSphereGeometry(ir), m, fd, &density, NULL, bCollsion );
	}

	physx::PxRigidActor* CreateCapsule(	const PxTransform& pose, PxReal radius, PxReal halfHeight, const PxReal density, PxMaterial& m, const PxFilterData* fd, BOOL bCollsion )
	{
		return createRigidActor( pose, PxCapsuleGeometry(radius,halfHeight), m, fd, &density, NULL, bCollsion );
	}

	BOOL CreateActorBox( physx::PxRigidActor** ppActor, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vHalfDir )
	{
		if ( (*ppActor) )
			DeleteActor( (*ppActor) );

		// [2012.4.4] �ؿ� �ڵ尡 �ǹ��� ��.
		//				������ ���� �����ϴ� ���ΰ�~?
		//				�׷��ٸ� ���� �ȵǰ� �����ϴ� ��찡 �ִ°�~?
		size_t nFailCount(0);
		physx::PxRigidDynamic* pActor(NULL);
		while( !pActor )
		{
			PxVec3 pos(vPos.x,vPos.y,vPos.z);
			PxVec3 dir(vHalfDir.x,vHalfDir.y,vHalfDir.z);
			pActor = CreateBox( PxTransform(pos), dir, 1.f, *g_pMaterial, NULL, TRUE )->is<PxRigidDynamic>();
			++nFailCount;
			if( nFailCount > 1000 )
			{
				return FALSE;
			}
		}

		pActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		pActor->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, true );
		(*ppActor) = pActor;

		return TRUE;
	}

	BOOL CreateActorSphere( physx::PxRigidActor** ppActor, const D3DXVECTOR3& vPos, float fRadius )
	{
		if ( (*ppActor) )
			DeleteActor( (*ppActor) );

		// [2012.4.4] �ؿ� �ڵ尡 �ǹ��� ��.
		//				������ ���� �����ϴ� ���ΰ�~?
		//				�׷��ٸ� ���� �ȵǰ� �����ϴ� ��찡 �ִ°�~?
		size_t nFailCount(0);
		physx::PxRigidDynamic* pActor(NULL);
		while( !pActor )
		{
			PxVec3 pos(vPos.x,vPos.y,vPos.z);
			pActor = CreateSphere( PxTransform(pos), fRadius, 1.f, *g_pMaterial, NULL, TRUE )->is<PxRigidDynamic>();
			++nFailCount;
			if( nFailCount > 1000 )
			{
				return FALSE;
			}
		}

		pActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		pActor->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, true );
		(*ppActor) = pActor;

		return TRUE;
	}

	BOOL CreateActorCapsule( physx::PxRigidActor** ppActor, const D3DXVECTOR3& vPos, float fRadius, float fHalfHeight )
	{
		if ( (*ppActor) )
			DeleteActor( (*ppActor) );

		// [2012.4.4] �ؿ� �ڵ尡 �ǹ��� ��.
		//				������ ���� �����ϴ� ���ΰ�~?
		//				�׷��ٸ� ���� �ȵǰ� �����ϴ� ��찡 �ִ°�~?
		size_t nFailCount(0);
		physx::PxRigidDynamic* pActor(NULL);
		while( !pActor )
		{
			PxVec3 pos(vPos.x,vPos.y,vPos.z);
			pActor = CreateCapsule( PxTransform(pos), fRadius, fHalfHeight, 1.f, *g_pMaterial, NULL, TRUE )->is<PxRigidDynamic>();
			++nFailCount;
			if( nFailCount > 1000 )
			{
				return FALSE;
			}
		}

		pActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		pActor->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, true );
		(*ppActor) = pActor;

		return TRUE;
	}

	BOOL CreateJoint( const VEC_BONE_TRANS_JOINT_DATA& vecBoneTrans, const JiggleBoneData::VEC_JOINTDATA& vecJointData, VEC_NXACTOR& vecNxActor, VEC_NXJOINT& vecNxJoint, float fScale, BOOL bCollision )
	{
		typedef std::map<TSTRING,physx::PxRigidActor*>	MAP_NXACTOR;
		typedef MAP_NXACTOR::iterator		MAP_NXACTOR_ITER;
		MAP_NXACTOR mapNxActor;			// ������ ������.

		float segHeight = 0.01f;
		float segRadius = 0.1f;
		size_t nbSegments = vecBoneTrans.size();

		PxVec3 pos(0.f,0.f,0.f);
		PxVec3 dir(0.9f,0.1f,0.1f);
		for ( size_t i = 0; i < nbSegments-1; i++)
		{
			if ( vecBoneTrans[i].m_rBoneTransChild )
			{
				D3DXVECTOR3 vDir = D3DXVECTOR3( vecBoneTrans[i].m_rBoneTransChild->m_pQuatPosORIG.m_vPos.x, 
												vecBoneTrans[i].m_rBoneTransChild->m_pQuatPosORIG.m_vPos.y,
												vecBoneTrans[i].m_rBoneTransChild->m_pQuatPosORIG.m_vPos.z );

				float fHalfLength = D3DXVec3Length( &vDir ) * 0.5f * fScale;
				dir.x = 1.0f * fHalfLength;
				dir.y = 0.1f * fHalfLength;
				dir.z = 0.1f * fHalfLength;
			}

			// [2012.4.4] �ؿ� �ڵ尡 �ǹ��� ��.
			//				������ ���� �����ϴ� ���ΰ�~?
			//				�׷��ٸ� ���� �ȵǰ� �����ϴ� ��찡 �ִ°�~?
			size_t nFailCount(0);
			physx::PxRigidDynamic* pActor(NULL);
			while( !pActor )
			{
				pActor = CreateBox( PxTransform(pos), dir, vecBoneTrans[i].m_sJointData.m_fDensity*1.f, *g_pMaterial, NULL, bCollision ? ((i==0)?FALSE:TRUE) : FALSE )->is<PxRigidDynamic>();
				++nFailCount;
				if( nFailCount > 1000 )
				{
					for ( DWORD z=0; z<vecNxActor.size(); ++z )
					{
						DeleteActor( vecNxActor[z] );
					}
					vecNxActor.clear();
					return FALSE;
				}
			}

			if ( i == 0 )
			{
				pActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
				pActor->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, true );

			//	// NX_BF_DISABLE_GRAVITY
			//	pActor->raiseBodyFlag( NX_BF_KINEMATIC );
			//	pActor->raiseBodyFlag( NX_BF_DISABLE_GRAVITY );
			//	pActor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
			}
			else
			{
				pActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);
				pActor->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, false );
				pActor->setLinearDamping(1.f);
				pActor->setAngularDamping(1.f);
				//pActor->setSolverIterationCounts(255, 255);
			//	//pActor->setLinearDamping( 0.5f );
			//	pActor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
			}

			vecNxActor.push_back( pActor );															// ���� ������ ������ �ִ� ��
			mapNxActor.insert( std::make_pair( vecBoneTrans[i].m_rBoneTransJoint->GetName(), pActor ) );	// ��� ����
		}

		D3DXVECTOR3 vJointOffset(0.f,0.f,0.f);
		D3DXVECTOR3 vDir;
		for ( size_t i = 1; i < nbSegments-1; i++)
		{
			if ( vecBoneTrans[i].m_rBoneTransChild )
			{
				vJointOffset = vecBoneTrans[i].m_rBoneTransChild->m_pQuatPosORIG.m_vPos * fScale;
			}

			// [2012.4.4] �ؿ� �ڵ尡 �ǹ��� ��.
			//				������ ���� �����ϴ� ���ΰ�~?
			//				�׷��ٸ� ���� �ȵǰ� �����ϴ� ��찡 �ִ°�~?
			int nFailCount(0);
			physx::PxD6Joint* pJoint(NULL);
			while( !pJoint )
			{
				MAP_NXACTOR_ITER iterParent = mapNxActor.find( vecBoneTrans[i].m_rBoneTransParent->GetName() );
				if ( iterParent==mapNxActor.end() )
				{
					break;
				}

				MAP_NXACTOR_ITER iterJoint = mapNxActor.find( vecBoneTrans[i].m_rBoneTransJoint->GetName() );
				if ( iterJoint==mapNxActor.end() )
				{
					break;
				}

				D3DXQUATERNION vQuat;
				if ( vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vScale.x < 0.f )
				{
					D3DXQUATERNION vQuat180;
					D3DXVECTOR3 vDirY(0.f,1.f,0.f);
					D3DXQuaternionRotationAxis( &vQuat180, &vDirY, D3DX_PI );
					D3DXQuaternionMultiply( &vQuat, &vQuat180, &vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vQuat );
				}
				else
				{
					vQuat = vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vQuat;
				}

				//D3DXMATRIX matLocalRotate;
				D3DXVECTOR3 vLocalRotate = D3DXVECTOR3( vecJointData[i-1]->m_vRotate.x*D3DX_PI/180.f,
														vecJointData[i-1]->m_vRotate.y*D3DX_PI/180.f, 
														vecJointData[i-1]->m_vRotate.z*D3DX_PI/180.f );

				D3DXQUATERNION vLocalQuat;
				if ( vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vScale.x < 0.f )
				{
					D3DXQuaternionRotationYawPitchRoll( &vLocalQuat, vLocalRotate.y, vLocalRotate.x, -vLocalRotate.z );
					D3DXQuaternionMultiply( &vQuat, &vLocalQuat, &vQuat );
				}
				else
				{
					D3DXQuaternionRotationYawPitchRoll( &vLocalQuat, vLocalRotate.y, vLocalRotate.x, vLocalRotate.z );
					D3DXQuaternionMultiply( &vQuat, &vLocalQuat, &vQuat );
				}

				//D3DXQUATERNION vQuatChild;
				//if ( vecBoneTrans[i].m_rBoneTransChild->m_pQuatPosORIG.m_vScale.x < 0.f )
				//{
				//	D3DXQUATERNION vQuat180;
				//	D3DXVECTOR3 vDirY(0.f,1.f,0.f);
				//	D3DXQuaternionRotationAxis( &vQuat180, &vDirY, D3DX_PI );
				//	D3DXQuaternionMultiply( &vQuatChild, &vQuat180, &vecBoneTrans[i].m_rBoneTransChild->m_pQuatPosORIG.m_vQuat );

				//	D3DXQuaternionMultiply ( &vQuat, &vQuat, &vQuatChild );
				//}
				//else
				//{
				//	vQuatChild = vecBoneTrans[i].m_rBoneTransChild->m_pQuatPosORIG.m_vQuat;

				//	D3DXQuaternionMultiply ( &vQuat, &vQuat, &vQuatChild );
				//}

				physx::PxTransform poseParent(PxTransform::createIdentity());
				if ( i == 1 )
				{
					// ���� Bone �̱� ������ Joint ������ ���̸� �ȵȴ�.
					// ���� Bone ���� �Ÿ��� �����Ǿ� �Ǳ� ������ 0.5f �� �ϸ� �ȵȴ�.
					poseParent.p = PxVec3( vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vPos.x * fScale, 
											vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vPos.y * fScale, 
											vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vPos.z * fScale );
				}
				else
				{
					poseParent.p = PxVec3( vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vPos.x*0.5f * fScale, 
											vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vPos.y*0.5f * fScale, 
											vecBoneTrans[i].m_rBoneTransJoint->m_pQuatPosORIG.m_vPos.z*0.5f * fScale );
				}
				poseParent.q.x = vQuat.x;
				poseParent.q.y = vQuat.y;
				poseParent.q.z = vQuat.z;
				poseParent.q.w = vQuat.w;
				//poseParent.q.createIdentity();

				physx::PxTransform poseJoint(PxTransform::createIdentity());
				//physx::PxTransform poseJoint;
				//poseJoint.q.x = vQuatChild.x;
				//poseJoint.q.y = vQuatChild.y;
				//poseJoint.q.z = vQuatChild.z;
				//poseJoint.q.w = vQuatChild.w;
				poseJoint.p = PxVec3( -vJointOffset.x*0.5f, -vJointOffset.y*0.5f, -vJointOffset.z*0.5f );

				pJoint = PxD6JointCreate
				(
					*g_pPhysicsSDK, 
					(*iterParent).second,			// the position and orientation of the joint relative to actor0	// actor0 �� �Ǵ� �������� joint ��ġ�� ����
					poseParent,
					(*iterJoint).second, 
					poseJoint						// the position and orientation of the joint relative to actor1	// actor1 �� �Ǵ� �������� joint ��ġ�� ����
				);

				pJoint->setMotion( PxD6Axis::eX,		PxD6Motion::eLOCKED );
				pJoint->setMotion( PxD6Axis::eY,		PxD6Motion::eLOCKED );
				pJoint->setMotion( PxD6Axis::eZ,		PxD6Motion::eLOCKED );
				pJoint->setMotion( PxD6Axis::eTWIST,	PxD6Motion::eLOCKED );
				pJoint->setMotion( PxD6Axis::eSWING1,	PxD6Motion::eLIMITED );
				pJoint->setMotion( PxD6Axis::eSWING2,	PxD6Motion::eLIMITED );

				PxJointLimit sJointLimit(0,0);
				pJoint->setLinearLimit( sJointLimit );

				physx::PxJointLimitCone sCone( static_cast<float>( vecBoneTrans[i].m_sJointData.m_dwSwingY )*D3DX_PI/180.f,
												static_cast<float>( vecBoneTrans[i].m_sJointData.m_dwSwingZ )*D3DX_PI/180.f,
												0.1f );
				sCone.restitution = vecBoneTrans[i].m_sJointData.m_fSwingRestitution;
				sCone.spring = vecBoneTrans[i].m_sJointData.m_fSwingSpring;
				sCone.damping = vecBoneTrans[i].m_sJointData.m_fSwingDamping;
				if ( sCone.isValid() )
				{
					pJoint->setSwingLimit( sCone );
				}

				if ( vecBoneTrans[i].m_sJointData.m_dwTwist == 0 )
				{
					pJoint->setMotion( PxD6Axis::eTWIST,	PxD6Motion::eLOCKED );
				}
				else
				{
					physx::PxJointLimitPair sLimit( -static_cast<float>( vecBoneTrans[i].m_sJointData.m_dwTwist )*D3DX_PI/180.f,
													static_cast<float>( vecBoneTrans[i].m_sJointData.m_dwTwist )*D3DX_PI/180.f,
													0.01f );
					sLimit.restitution = vecBoneTrans[i].m_sJointData.m_fTwistRestitution;
					sLimit.spring = vecBoneTrans[i].m_sJointData.m_fTwistSpring;
					sLimit.damping = vecBoneTrans[i].m_sJointData.m_fTwistDamping;
					if ( sLimit.isValid() )
					{
						pJoint->setTwistLimit( sLimit );
					}
				}

				pJoint->setProjectionLinearTolerance( 0.1f );
				pJoint->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
				////pJoint->setProjectionAngularTolerance( 1.f );

				++nFailCount;
				if( nFailCount > 1000 )
				{
					for ( DWORD z=0; z<vecNxActor.size(); ++z )
					{
						DeleteActor( vecNxActor[z] );
					}
					for ( DWORD z=0; z<vecNxJoint.size(); ++z )
					{
						DeleteJoint( vecNxJoint[z] );
					}
					vecNxActor.clear();
					vecNxJoint.clear();
					return FALSE;
				}
			}

			// �߰�
			if ( pJoint )
			{
				vecNxJoint.push_back( pJoint );
				g_setNxJoint.insert( pJoint );
			}
		}

		return TRUE;
	}

	void DeleteJoint( physx::PxJoint* pNxJoint )
	{
		//if( pNxJoint && g_pScene )
		if( pNxJoint && g_pPhysicsSDK )
		{
			SET_NXJOINT_ITER iter = g_setNxJoint.find( pNxJoint );
			if ( iter != g_setNxJoint.end() )
				g_setNxJoint.erase( pNxJoint );

			pNxJoint->release();
			pNxJoint = NULL;
			//g_pScene->releaseJoint( *pNxJoint );
		}
	}



	//////////////////////////////////////////////////////////////////////////
	void GetMatrix( D3DXMATRIX& matOUT, physx::PxRigidActor* pActor )
	{
		if( !g_pPhysicsSDK )
			return;

		if ( !pActor )
			return;

		physx::PxTransform pose = pActor->getGlobalPose();

		D3DXQUATERNION sQuat;
		sQuat.x = pose.q.x;
		sQuat.y = pose.q.y;
		sQuat.z = pose.q.z;
		sQuat.w = pose.q.w;

		D3DXMatrixRotationQuaternion( &matOUT, &sQuat );
		matOUT._41 = pose.p.x;
		matOUT._42 = pose.p.y;
		matOUT._43 = pose.p.z;
	}

	void GetMatrix( D3DXMATRIX& matOUT, physx::PxRigidActor* pTrans, physx::PxRigidActor* pRotate )
	{
		if( !g_pPhysicsSDK )
			return;

		if ( !pTrans )
			return;

		if ( !pRotate )
			return;

		physx::PxTransform pose = pRotate->getGlobalPose();

		D3DXQUATERNION sQuat;
		sQuat.x = pose.q.x;
		sQuat.y = pose.q.y;
		sQuat.z = pose.q.z;
		sQuat.w = pose.q.w;

		D3DXMatrixRotationQuaternion( &matOUT, &sQuat );

		pose = pTrans->getGlobalPose();
		matOUT._41 = pose.p.x;
		matOUT._42 = pose.p.y;
		matOUT._43 = pose.p.z;
	}

	//void SetMatrix( DxPiece* pPiece, const D3DXMATRIX& matWorld )
	//{
	//	if( !g_pPhysicsSDK )
	//		return;

	//	physx::PxRigidActor* pActor = pPiece->GetNxActor();
	//	if( !pActor )
	//		return;

	//	PxVec3 vPos0;
	//	PxVec3 vPos1;
	//	PxVec3 vPos2;
	//	vPos0.x = matWorld._11;
	//	vPos0.y = matWorld._12;
	//	vPos0.z = matWorld._13;
	//	vPos1.x = matWorld._21;
	//	vPos1.y = matWorld._22;
	//	vPos1.z = matWorld._23;
	//	vPos2.x = matWorld._31;
	//	vPos2.y = matWorld._32;
	//	vPos2.z = matWorld._33;

	//	physx::PxTransform pose;
	//	pose.M.setColumn( 0, vPos0 );
	//	pose.M.setColumn( 1, vPos1 );
	//	pose.M.setColumn( 2, vPos2 );
	//	pose.t = PxVec3( matWorld._41, matWorld._42, matWorld._43 );
	//	pActor->setGlobalPose( pose );

	//	PxVec3 vZeroVel(0.f,0.f,0.f);
	//	pActor->setLinearVelocity( vZeroVel );
	//	pActor->setAngularVelocity( vZeroVel );
	//}

	//class ActorUserData
	//{
	//public:
	//	NxU32 id;
	//	NxU32 contactEvents;
	//	NxU32 flags;

	//	ActorUserData()
	//	{
	//		id = 0;
	//		contactEvents = 0;
	//		flags = 0;
	//	}
	//};



	//////////////////////////////////////////////////////////////////////////
	physx::PxCloth* createClothFromMeshDesc( PxClothMeshDesc& meshDesc, const PxTransform& pose, PxClothCollisionData* collData, const PxVec3& gravityDir)
	{
		PxClothFabric* clothFabric = NSClothHelpers::createFabric( *g_pPhysicsSDK, *g_pCooking, meshDesc, gravityDir );
		if (!clothFabric)
			return 0;

		// create cloth particle to set initial position and inverse mass (constraint)
		SampleArray<PxClothParticle> clothParticles(meshDesc.points.count);
		NSClothHelpers::createDefaultParticles(meshDesc, clothParticles.begin());

		// flags to set GPU solver, CCD, etc.
		PxClothFlags flags;

		// create the cloth actor
		physx::PxCloth* cloth(NULL);	
		if (collData)
			cloth = g_pPhysicsSDK->createCloth( pose, *clothFabric, &clothParticles[0], *collData, flags);
		else
		{
			PxClothCollisionData collisionData;
			cloth = g_pPhysicsSDK->createCloth( pose, *clothFabric, &clothParticles[0], collisionData, flags);
		}

		cloth->setSolverFrequency(60.0f); // don't know how to get target simulation frequency, just hardcode for now

		PX_ASSERT(cloth);	

		// add this cloth into the scene
		g_pScene->addActor(*cloth);

		//// create render material
		//RenderMaterial* clothMaterial = createRenderMaterialFromTextureFile(textureFile);

		//// create the render object in sample framework
		//createRenderObjectsFromCloth(*cloth, meshDesc, clothMaterial, uv, true, color, scale);

		return cloth;
	}

	physx::PxCloth* CreateCloth( LPD3DXMESH pMesh, 
								const D3DXMATRIX* arrayBoneMatrices, 
								DWORD dwNumInfl,
								DWORD dwNumPaletteEntries,
								const SMATERIAL_PIECE& sMaterialSystem,
								CONST D3DVERTEXELEMENT9 *pDeclaration,
								float fAttParentBoneScale, 
								const D3DXMATRIX& matWorld, 
								const float* arrayInvWeight,
								std::vector<LockParticle>& vecLockParticle,
								const DxClothColl* rClothColl )
	{
		//////////////////////////////////////////////////////////////////////////
		// PxClothMeshDesc ���� ������.
		// create the cloth cape mesh from file
		PxClothMeshDesc meshDesc;
		std::vector<PxVec3> vertices;
		std::vector<WORD> primitives;
		PxReal clothScale = fAttParentBoneScale;	// * 0.3f;
		PxVec3 offset = PxVec3(0,0,0); //PxVec3(0,-1.5,0); 
		PxQuat rot = PxQuat(0, PxVec3(0,1,0));
		NSClothHelpers::createMeshFromObj(pMesh, arrayBoneMatrices, matWorld, dwNumInfl, dwNumPaletteEntries, pDeclaration, clothScale, NULL, NULL, vertices, primitives, meshDesc);

		if (!meshDesc.isValid())
		{
			AfxMessageBox( "CreateCloth Failed" );
			return NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		// PxCloth ���� ������.
		// create the cloth
		std::vector<PxClothCollisionSphere> vecSpheres;
		std::vector<DWORD>					vecIndexPairs;
		rClothColl->GetAllClothCollData( vecSpheres, vecIndexPairs, fAttParentBoneScale );	// const �� rClothColl �� ������ ����.

		PxTransform rootPose;
		PxClothCollisionData collisionData;
		collisionData.numSpheres = static_cast<PxU32>(vecSpheres.size());
		if ( collisionData.numSpheres )
		{
			collisionData.spheres = &vecSpheres[0];
		}
		collisionData.numPairs = static_cast<PxU32>(vecIndexPairs.size()) / 2; // number of capsules
		if ( collisionData.numPairs )
		{
			collisionData.pairIndexBuffer = reinterpret_cast<PxU32*>( &vecIndexPairs[0] );
		}

		physx::PxCloth& cloth = *createClothFromMeshDesc( meshDesc, rootPose, &collisionData, PxVec3(0,-1.f,0) );

		// Manager �� �ֱ�
		g_setNxCloth.insert( &cloth );

		// attach top verts
		PxClothReadData* readData = cloth.lockClothReadData();
		PX_ASSERT(readData);
		PxU32 numParticles = cloth.getNbParticles();
		SampleArray<PxClothParticle> particles(numParticles);
		SampleArray<PxVec3> particlePositions(numParticles);

		vecLockParticle.clear();
		for(PxU32 i = 0; i < numParticles; i++)
		{
			particles[i] = readData->particles[i];

			// Mesh �� Diffuse Alpha ������ ��ġ�� ���Ѵ�.
			if ( arrayInvWeight )
			{
				if ( arrayInvWeight[i] == 0.f )
				{
					particles[i].invWeight = 0.f;
				}
				else
				{
					particles[i].invWeight = arrayInvWeight[i] + 10.f;	// ū���̴�. Ʀ������ ���ϼ� �ִ�. ���������� �����.
				}
			}
			else
			{
				particles[i].invWeight = 0.f;
			}

			//////////////////////////////////////////////////////////////////////////
			// AttachParentBone �� Scale ��ȭ �� 3 ���� ( /fAttParentBoneScale )
			//////////////////////////////////////////////////////////////////////////
			// 3. LockParticle �� Bone_Matrix �� ������ �Ǳ� ������, �۰� ����� �ȵǰ�, ����� ���� ������� �Ǿ� �־�� �Ѵ�.
			//////////////////////////////////////////////////////////////////////////
			//
			// LockParticle �� ��´�.
			if ( particles[i].invWeight == 0.f )
			{
				LockParticle sLockParticle;
				sLockParticle.m_dwIndex = i;
				sLockParticle.m_vPosition.x = particles[i].pos.x/fAttParentBoneScale;
				sLockParticle.m_vPosition.y = particles[i].pos.y/fAttParentBoneScale;
				sLockParticle.m_vPosition.z = particles[i].pos.z/fAttParentBoneScale;
				vecLockParticle.push_back( sLockParticle );
			}

			// [shhan][2014.11.20] ���� ��ġ�� ��������� �Ѵ�.
			//						�̰� ���� ������ Cloth �� Ƣ�� ������ ���ϰ� ���� ���̴�.
			D3DXVECTOR3* pPos = reinterpret_cast<D3DXVECTOR3*>( &particles[i].pos );
			D3DXVec3TransformCoord( pPos, pPos, &matWorld );

		}
		readData->unlock();
		cloth.setParticles(particles.begin(), particles.begin());

		// set solver settings
		//cloth.setSolverFrequency(240);
		cloth.setSolverFrequency( static_cast<physx::PxReal>( sMaterialSystem.m_nClothSolverFrequency ) );		// 

		// damp global particle velocity to 90% every 0.1 seconds
		//cloth.setDampingCoefficient(0.1f); // damp local particle velocity // ���� ���
		cloth.setDampingCoefficient( sMaterialSystem.m_fClothDampingCoefficient ); // damp local particle velocity
		//cloth.setDragCoefficient(0.1f); // transfer frame velocity // ���� ���� ���
		cloth.setDragCoefficient(0.1f); // transfer frame velocity	

		// reduce effect of local frame acceleration
		//cloth.setInertiaScale(0.3f);	// �۷ι� ���� ��ȭ�� ���� ȿ���� �����ϱ� ���� ���� ��ô ������ �����մϴ�. 
		cloth.setInertiaScale(0.f);


		//const bool useCustomConfig = true;

		//// virtual particles
		//if (useVirtualParticles)
		//{
		//	//////////////////////////////////////////////////////////////////////////
		//	// õ �ùķ��̼��� �浹Bone �� õ�� �ﰢ�� ���̴� ������ ���� �߻� �� �� �ִ�.
		//	// ������ ������ ���� �귯�����鼭 ���´�.
		//	// createVirtualParticles ������ �ϸ��� ���̸鼭 ������ ���ϰ� ���ӵǴ� ������ ���δ�.
		//	// 
		//	//NSClothHelpers::createVirtualParticles(cloth, meshDesc, 4);
		//	//NSClothHelpers::createVirtualParticles(cloth, meshDesc, 7);
		//	//NSClothHelpers::createVirtualParticles(cloth, meshDesc, 13);
		//}

		//////////////////////////////////////////////////////////////////////////
		// õ �ùķ��̼��� �浹Bone �� õ�� �ﰢ�� ���̴� ������ ���� �߻� �� �� �ִ�.
		// ������ ������ ���� �귯�����鼭 ���´�.
		// eSWEPT_CONTACT : true ������ �ϸ��� ���̸鼭 ������ ���ϰ� ���ӵǴ� ������ ���δ�.
		//
		// ccd
		//cloth.setClothFlag(PxClothFlag::eSWEPT_CONTACT, useSweptContact);

		// use GPU or not
#if PX_SUPPORT_GPU_PHYSX
		//cloth.setClothFlag(PxClothFlag::eGPU, mUseGPU);
#endif

		// custom fiber configuration
		//if (useCustomConfig)
		{
			PxClothPhaseSolverConfig config;

			config = cloth.getPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING);
			config.solverType = PxClothPhaseSolverConfig::eSTIFF;
			//config.stiffness = 1.0f;
			config.stiffness = sMaterialSystem.m_vClothConfiguration.x;
			cloth.setPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING, config);

			config = cloth.getPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING_HORIZONTAL);
			config.solverType = PxClothPhaseSolverConfig::eFAST;
			//config.stiffness = 1.0f;
			config.stiffness = sMaterialSystem.m_vClothConfiguration.y;
			cloth.setPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING_HORIZONTAL, config);

			config = cloth.getPhaseSolverConfig(PxClothFabricPhaseType::eSHEARING);
			config.solverType = PxClothPhaseSolverConfig::eFAST;
			//config.stiffness = 0.75f;
			config.stiffness = sMaterialSystem.m_vClothConfiguration.z;
			cloth.setPhaseSolverConfig(PxClothFabricPhaseType::eSHEARING, config);

			config = cloth.getPhaseSolverConfig(PxClothFabricPhaseType::eBENDING_ANGLE);
			config.solverType = PxClothPhaseSolverConfig::eBENDING;
			//config.stiffness = 0.5f;
			config.stiffness = sMaterialSystem.m_vClothConfiguration.w;
			cloth.setPhaseSolverConfig(PxClothFabricPhaseType::eBENDING_ANGLE, config);
		}

		//////////////////////////////////////////////////////////////////////////
		// set cloth pose
		cloth.setGlobalPose(PxTransform::createIdentity());

		// clear previous state of collision shapes etc.
		cloth.clearInterpolation();

		return &cloth;
	}

	void DeleteCloth( physx::PxCloth** ppNxCloth )
	{
		//if( pNxJoint && g_pScene )
		if( (*ppNxCloth) && g_pPhysicsSDK )
		{
			SET_NXCLOTH_ITER iter = g_setNxCloth.find( (*ppNxCloth) );
			if ( iter != g_setNxCloth.end() )
				g_setNxCloth.erase( (*ppNxCloth) );

			(*ppNxCloth)->release();
			(*ppNxCloth) = NULL;
			//g_pScene->releaseJoint( *pNxJoint );
		}
	}

	void UpdateCloth( LPD3DXMESH pMesh, 
						CONST D3DVERTEXELEMENT9 *pDeclaration, 
						DWORD dwNormalOffset,
						const WORD* pIndices, 
						physx::PxCloth* pCloth, 
						const D3DXMATRIX& matWorld,
						const float* arrayInvWeight,
						const std::vector<LockParticle>& vecLockParticle,
						const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres )
	{
		////////////////////////////////////////////////////////////////////////////
		//D3DXVECTOR3 vScale;
		//D3DXQUATERNION vQuat;
		//D3DXVECTOR3 vPos;
		//D3DXMatrixToSQT( vScale, vQuat, vPos, matWorld );

		//D3DXMATRIX matIdentity;
		//D3DXMatrixIdentity( &matIdentity );
		//D3DXMatrixToSQT( vScale, vQuat, vPos, matIdentity );

		//physx::PxTransform pose;
		//pose.q.x = vQuat.x;
		//pose.q.y = vQuat.y;
		//pose.q.z = vQuat.z;
		//pose.q.w = vQuat.w;
		//pose.p.x = vPos.x;
		//pose.p.y = vPos.y;
		//pose.p.z = vPos.z;

		// setInertiaScale �� 0 �� ��� �۾��� �ʿ䰡 ����.
		// set cloth pose
		//pCloth->setTargetPose(pose);

		// ��ġ������ �����.
		SampleArray<PxVec3> positions;
		NSClothHelpers::getParticlePositions( *pCloth, positions );

		//////////////////////////////////////////////////////////////////////////
		// Normal �� ��´�.
		int numVerts = pCloth->getNbParticles();
		int mNumFaces = pMesh->GetNumFaces();

		PX_ASSERT(numVerts > 0);

		// update render normal
		std::vector<PxVec3> normals(numVerts);
		PxBuildSmoothNormals(mNumFaces, numVerts, &positions[0], NULL, reinterpret_cast<const physx::PxU16*>(pIndices), &normals[0], false);

		////////////////////////////////////////////////////////////////////////////
		PxVec3 *vPhysPos(NULL);
		D3DXVECTOR3 vPosAffectMatrix;
		D3DXVECTOR3 vPosDiffAdd(0.f,0.f,0.f);
		float fInvWeight_AddValue(0.f);

		if ( !vecLockParticle.empty() )
		{
			vPhysPos = (PxVec3*)&positions[vecLockParticle[0].m_dwIndex];
			D3DXVec3TransformCoord( &vPosAffectMatrix, &vecLockParticle[0].m_vPosition, &matWorld );

			// �󸶳� �̵��߳��� ���� InvWeight ���� ��ȭ��Ų��.
			vPosDiffAdd.x = vPosAffectMatrix.x - vPhysPos->x;
			vPosDiffAdd.y = vPosAffectMatrix.y - vPhysPos->y;
			vPosDiffAdd.z = vPosAffectMatrix.z - vPhysPos->z;
		}

		bool useConstrainedOnly(true);
		if ( D3DXVec3Length( &vPosDiffAdd ) > 6.f )
		{
			// rm #3228 - [������]������ ���� ���� ��û
			// �ʹ� �̵��� ũ�� Ʀ������ ������� ���δ�. ��ü���� �����̵��� �Ѵ�.
			useConstrainedOnly = false;
			for( DWORD i=0; i<numVerts && i<positions.size(); ++i )
			{
				vPhysPos = (PxVec3*)&positions[i];
				vPhysPos->x += vPosDiffAdd.x;
				vPhysPos->y += vPosDiffAdd.y;
				vPhysPos->z += vPosDiffAdd.z;
			}
		}
		else
		{
			for( DWORD i=0; i<vecLockParticle.size(); ++i )
			{
				vPhysPos = (PxVec3*)&positions[vecLockParticle[i].m_dwIndex];
				D3DXVec3TransformCoord( &vPosAffectMatrix, &vecLockParticle[i].m_vPosition, &matWorld );
				
				// �󸶳� �̵��߳��� ���� InvWeight ���� ��ȭ��Ų��.
				vPosDiffAdd.x = vPosAffectMatrix.x - vPhysPos->x;
				vPosDiffAdd.y = vPosAffectMatrix.y - vPhysPos->y;
				vPosDiffAdd.z = vPosAffectMatrix.z - vPhysPos->z;
				fInvWeight_AddValue += D3DXVec3Length( &vPosDiffAdd );

				memcpy( vPhysPos, vPosAffectMatrix, sizeof(float)*3 );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// �̰����� ���� ĳ���Ϳ� ���� õ �������� Ŭ ���� õ Ʀ������ �ٿ��ְ�,
		//				ĳ���Ϳ� ���� õ �������� ���� ���� ĳ���Ϳ� �浹ó���� �ɸ������� �ٿ��ش�.
		// ��ġ�� ũ�� Ʀ������ ����, �浹������ ��������.
		// ��ġ�� ������ Ʀ������ ����, �浹������ �پ���.
		fInvWeight_AddValue = fInvWeight_AddValue/vecLockParticle.size();
		if ( fInvWeight_AddValue > 3.f )
		{
			// �������� ������ ���沨 ���Ƽ� �̸� �����Ѵ�.
			fInvWeight_AddValue = 81.f;
		}
		else
		{
			fInvWeight_AddValue = fInvWeight_AddValue * fInvWeight_AddValue;	// ������ ũ�� �����.
			fInvWeight_AddValue = fInvWeight_AddValue * fInvWeight_AddValue;	// ������ ũ�� �����.
			fInvWeight_AddValue *= 1.f;		// MagicNumber
		}

		//////////////////////////////////////////////////////////////////////////
		// ���� �־��ֵ��� �Ѵ�.
		PxVec3* pPos(NULL);
		PxVec3* pNor(NULL);
		UINT nVertSize = D3DXGetDeclVertexSize( pDeclaration, 0 );		

		int numVertices	= pMesh->GetNumVertices();

		//////////////////////////////////////////////////////////////////////////
		//��� ��ƼŬ ���� ������ Vertex Buffer�� �����Ѵ�.
		BYTE* pBufferVB(NULL);
		PxVec3 *vSrc = (PxVec3*)&positions[0];
		PxVec3 *vNor = (PxVec3*)&normals[0];
		pMesh->LockVertexBuffer( D3DLOCK_DISCARD, (VOID**)&pBufferVB );
		for( int i = 0 ; i < numVertices ; i++, vSrc++, vNor++ )
		{
			pPos = reinterpret_cast<PxVec3*>( pBufferVB+(i*nVertSize) );
			*pPos = *vSrc;

			pNor = reinterpret_cast<PxVec3*>( pBufferVB+(i*nVertSize)+dwNormalOffset );
			*pNor = *vNor;
		}
		pMesh->UnlockVertexBuffer();



		//// set collision sphere positions
		//SampleArray<PxClothCollisionSphere> spheres(positions.size());
		//SampleArray<PxClothCollisionPlane> planes(pCloth->getNbCollisionPlanes());
		//SampleArray<PxU32> convexMasks(pCloth->getNbCollisionConvexes());

		//cloth.getCollisionData(spheres.begin(), indexPairs.begin(), planes.begin(), convexMasks.begin());
		//for (PxU32 i = 0; i < positions.size(); i++)
		//	spheres[i].pos = positions[i];
		//pCloth->setCollisionSpheres(spheres.begin());

		//// set positions for constrained particles
		//SampleArray<PxVec3> particlePositions;
		//mSkin.computeNewPositions(mCharacter, particlePositions);

		NSClothHelpers::setParticlePositions(*pCloth, positions, arrayInvWeight, fInvWeight_AddValue, useConstrainedOnly, useConstrainedOnly);
		//NSClothHelpers::setParticlePositions(*pCloth, positions, useConstrainedOnly, useConstrainedOnly);



		////////////////////////////////////////////////////////////////////////////
		//SampleArray<PxClothCollisionSphere> spheres(positions.size());
		//SampleArray<PxU32> indexPairs;
		//SampleArray<PxClothCollisionPlane> planes(pCloth->getNbCollisionPlanes());
		//SampleArray<PxU32> convexMasks(pCloth->getNbCollisionConvexes());

		//pCloth->getCollisionData(spheres.begin(), indexPairs.begin(), planes.begin(), convexMasks.begin());



		////////////////////////////////////////////////////////////////////////////
		//// PxCloth ���� ������.	�浹����.
		//std::vector<PxClothCollisionSphere> vecSpheres;
		//pClothColl->GetAllClothCollData( vecSpheres );
		//if ( !vecSpheres.empty() )
		//{
		//	pCloth->setCollisionSpheres( vecSpheres );
		//}
		if ( pvecClothCollSpheres && !pvecClothCollSpheres->empty() )
		{
			pCloth->setCollisionSpheres( &(*pvecClothCollSpheres)[0] );
		}
		else
		{
			pCloth->setCollisionSpheres( NULL );
		}



		////////////////////////////////////////////////////////////////////////////
		//PxU32 NbPA = pCloth->getNbParticleAccelerations();
		//std::vector<PxVec4> vecparticleAccelerationsBuffer;
		//vecparticleAccelerationsBuffer.resize( NbPA );
		//if ( !vecparticleAccelerationsBuffer.empty() )
		//{
		//	pCloth->getParticleAccelerations( &vecparticleAccelerationsBuffer[0] );

		//	int a=0;
		//}
		////pCloth->setNbParticleAccelerations( &vecparticleAccelerationsBuffer[0] );
	}



	//////////////////////////////////////////////////////////////////////////
	void DrawBox( physx::PxShape* box, const D3DXMATRIX& matWorld, DWORD dwColor )
	{
		physx::PxBoxGeometry sBoxGeometry;
		if ( !box->getBoxGeometry( sBoxGeometry ) )
			return;

		EDITMESHS::RENDERBOX( D3DXVECTOR3( 0.f, 0.f, 0.f ), 
			static_cast<float>( sBoxGeometry.halfExtents.x*2.f ), 
			static_cast<float>( sBoxGeometry.halfExtents.y*2.f ), 
			static_cast<float>( sBoxGeometry.halfExtents.z*2.f ), 
			dwColor, 
			&matWorld );
	}

	void DrawSphere( physx::PxShape* sphere, const D3DXMATRIX& matWorld, DWORD dwColor )
	{
		physx::PxSphereGeometry sSphereGeometry;
		if ( !sphere->getSphereGeometry( sSphereGeometry ) )
			return;

		D3DXMATRIX matRotate = matWorld;
		matRotate._41 = 0.f;
		matRotate._42 = 0.f;
		matRotate._43 = 0.f;

		EDITMESHS::RENDERSPHERE( D3DXVECTOR3( matWorld._41, matWorld._42, matWorld._43 ),
			D3DXVECTOR3( sSphereGeometry.radius, sSphereGeometry.radius, sSphereGeometry.radius ),
			FALSE, 
			&matRotate,
			dwColor );
	}

	void DrawCapsule( physx::PxShape* Capsule, const D3DXMATRIX& matWorld, DWORD dwColor )
	{
		physx::PxCapsuleGeometry sCapsuleGeometry;
		if ( !Capsule->getCapsuleGeometry( sCapsuleGeometry ) )
			return;

		//D3DXMATRIX matTrans;
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		//matTrans = matIdentity;
		//matTrans._41 = matWorld._41;
		//matTrans._42 = matWorld._42;
		//matTrans._43 = matWorld._43;

		D3DXMATRIX matRotateY, matResult;
		D3DXMatrixRotationY( &matRotateY, D3DX_PI*0.5f );
		D3DXMatrixMultiply( &matResult, &matRotateY, &matWorld );

		EDITMESHS::RENDERCYLINDER( D3DXVECTOR3( 0.f, 0.f, 0.f ), 
									static_cast<float>( sCapsuleGeometry.radius ),
									static_cast<float>( sCapsuleGeometry.halfHeight*2.f ),
									dwColor, 
									&matResult );

		D3DXVECTOR3 vOffset( sCapsuleGeometry.halfHeight, 0.f, 0.f );
		D3DXVec3TransformCoord( &vOffset, &vOffset, &matWorld );
		EDITMESHS::RENDERSPHERE( D3DXVECTOR3( vOffset.x, vOffset.y, vOffset.z ),
									D3DXVECTOR3( sCapsuleGeometry.radius, sCapsuleGeometry.radius, sCapsuleGeometry.radius ),
									FALSE, 
									&matIdentity,
									dwColor );

		vOffset = D3DXVECTOR3( -sCapsuleGeometry.halfHeight, 0.f, 0.f );
		D3DXVec3TransformCoord( &vOffset, &vOffset, &matWorld );
		EDITMESHS::RENDERSPHERE( D3DXVECTOR3( vOffset.x, vOffset.y, vOffset.z ),
									D3DXVECTOR3( sCapsuleGeometry.radius, sCapsuleGeometry.radius, sCapsuleGeometry.radius ),
									FALSE, 
									&matIdentity,
									dwColor );
	}

	//void DrawWireShape(NxShape *shape, const PxVec3& color, bool useShapeUserData)
	//{
	//	switch(shape->getType())
	//	{
	//		case NX_SHAPE_PLANE:
	//			DrawWirePlane(shape, color);
	//		break;
	//		case NX_SHAPE_BOX:
	//			DrawWireBox(shape, color);
	//		break;
	//		case NX_SHAPE_SPHERE:
	//			DrawWireSphere(shape, color);
	//		break;
	//		case NX_SHAPE_CAPSULE:
	//			DrawWireCapsule(shape, color);
	//		break;
	//		case NX_SHAPE_CONVEX:
	//			DrawWireConvex(shape, color, useShapeUserData);
	//		break;		
	//		case NX_SHAPE_MESH:
	//			DrawWireMesh(shape, color, useShapeUserData);
	//			break;
	//	}
	//}

	//void DrawShape(NxShape* shape, bool useShapeUserData)
	//{
	//	//glEnable(GL_NORMALIZE);

	//	//switch(shape->getType())
	//	//{
	//	//	//case NX_SHAPE_PLANE:
	//	//	//	DrawPlane(shape);
	//	//	//break;
	//	//case NX_SHAPE_BOX:
	//	//	DrawBox(shape);
	//	//	break;
	//	//case NX_SHAPE_SPHERE:
	//	//	DrawSphere(shape);
	//	//	break;
	//	//	//case NX_SHAPE_CAPSULE:
	//	//	//	DrawCapsule(shape);
	//	//	//break;
	//	//	//case NX_SHAPE_CONVEX:
	//	//	//	DrawConvex(shape, useShapeUserData);
	//	//	//break;
	//	//	//case NX_SHAPE_MESH:
	//	//	//	DrawMesh(shape, useShapeUserData);
	//	//	//break;
	//	//	//case NX_SHAPE_WHEEL:
	//	//	//	DrawWheelShape(shape);
	//	//	//	break;
	//	//	//case NX_SHAPE_HEIGHTFIELD:
	//	//	//	DrawHeightfield(shape);
	//	//	//	break;
	//	//}
	//}

	void DrawActor(physx::PxRigidActor* actor, physx::PxRigidActor* selectedActor, bool useShapeUserData)
	{
		////// We render some actors using light source 1 instead of light source 0
		////ActorUserData* ud;
		////ud = (ActorUserData*)(actor->userData);
		////if (ud && (ud->flags & UD_RENDER_USING_LIGHT1))
		////{
		////	glDisable(GL_LIGHT0);
		////	glEnable(GL_LIGHT1);
		////}

		//NxShape*const* shapes = actor->getShapes();
		//NxU32 nShapes = actor->getNbShapes();
		//while (nShapes--)
		//{
		//	DrawShape(shapes[nShapes], useShapeUserData);
		//}

		////// We render some actors using light source 1 instead of light source 0
		////ud = (ActorUserData*)(actor->userData);
		////if (ud && (ud->flags & UD_RENDER_USING_LIGHT1))
		////{
		////	glDisable(GL_LIGHT1);
		////	glEnable(GL_LIGHT0);
		////}

		//nShapes = actor->getNbShapes();
		//if (actor == selectedActor) 
		//{
		//	while (nShapes--)
		//	{
		//		//	DrawWireShape(shapes[nShapes], PxVec3(1,1,1), useShapeUserData);
		//	}
		//}
	}

	//void RenderActors(bool shadows)
	//{
	//	// Render all the actors in the g_pScene
	//	NxU32 nbActors = g_pScene->getNbActors();
	//	physx::PxRigidActor** actors = g_pScene->getActors();
	//	while (nbActors--)
	//	{
	//		physx::PxRigidActor* actor = *actors++;
	//		DrawActor(actor, gSelectedActor, false);

	//		//// Handle shadows
	//		//if (shadows)
	//		//{
	//		//	DrawActorShadow(actor, false);
	//		//}
	//	}
	//}

	//void DrawForce(physx::PxRigidActor* actor, PxVec3& forceVec, const PxVec3& color)
	//{
	//	// Draw only if the force is large enough
	//	NxReal force = forceVec.magnitude();
	//	if (force < 0.1)  return;

	//	forceVec = 3*forceVec/force;

	//	PxVec3 pos = actor->getCMassGlobalPosition();
	//	DrawArrow(pos, pos + forceVec, color);
	//}

	float fElapsedTimeBack(0.01f);
	void Render( float fElapsedTime )
	{
		if( !g_pPhysicsSDK )
			return;

		if ( fElapsedTime > 0.1f )
		{
			fElapsedTime = 0.1f;
			fElapsedTimeBack = 0.1f;
		}
		else
		{
			if ( fElapsedTime > fElapsedTimeBack*1.1f )
			{
				fElapsedTimeBack *= 1.1f;
				fElapsedTime = fElapsedTimeBack;
			}
			else if ( fElapsedTime < fElapsedTimeBack*0.9f )
			{
				fElapsedTimeBack *= 0.9f;
				fElapsedTime = fElapsedTimeBack;
			}
		}

		//fElapsedTime = 0.033f;

		if( g_pScene )	//&& !bPause )
		{
			//GetPhysicsResults();
			//ProcessInputs();
			StartPhysics( fElapsedTime );	

			//////////////////////////////////////////////////////////////////////////
			// [shhan][2014.05.09] Cloth ������ GetPhysicsResults ������ ����Ǿ���.
			//						���������� �����߿��� cloth.lockClothReadData(); ���� �������� �� �ϴ�.
			//						�׷��� ����� ��ٸ��� ��.
			GetPhysicsResults();
		}

		//// ���̾������� ������.
		//// Display g_pScene
		//bool bShadows(FALSE);
		//	RenderActors(bShadows);

		//DrawForce(gSelectedActor, gForceVec, PxVec3(1,1,0));
		//gForceVec = PxVec3(0,0,0);

		//// Render the HUD
		//hud.Render();

		//glFlush();
		//glutSwapBuffers();
	}
}