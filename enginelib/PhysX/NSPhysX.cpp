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

#include "../../SigmaCore/Math/Random.h"	// 삭제

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

	SET_NXACTOR	g_setNxActor;	// 관리 Manager
	SET_NXJOINT	g_setNxJoint;	// 관리 Manager
	SET_NXCLOTH	g_setNxCloth;	// 관리 Manager

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
				SFile << nPhysX_OK;		// 아직은 정상이라 할 수 없다.
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
				SFile << nPhysX_OK;		// 이제 정상이다라고 할 수 있다.
			}
			SFile.EndBlock();

			SFile.CloseFile();
		}
	}

	void InitNx_AvoidUnknownError( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pAppName )
	{
		// 파일 있는지 체크한다.
		// 있으면 InitNx 하지 않는다. ( 문제 PC )
		// 없으면 생성하고 InitNx 를 하도록 하고 파일을 삭제한다.. ( 문제 없는 PC )

		//////////////////////////////////////////////////////////////////////////
		// 경로를 만든다.
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
			// 들어온적 있다.

			DWORD dwVer, dwBufferSize;
			SFile >> dwVer;
			SFile >> dwBufferSize;

			// 만약 다르다면 Save 를 새로 해줘서 정상파일을 만들도록 한다.
			// 이렇게 안하면 유저가 직접 이 파일을 지우지 않는 이상 맵에 접속이 불가능할 수도 있다.
			if ( !SFile.IsSameRemindLoadSize( dwBufferSize ) || (dwBufferSize==0) ) 
			{
				SFile.CloseFile();

				// 이전 Save에서 문제가 발생했을 수도 있으므로(거의 낮은확률,하지만 BugTrap은 왔음) 일단 재 Save 하는 방향으로 한다.
				SaveLog_InitNx( pd3dDevice, strPathname.c_str() );

				return;
			}

			int nPhysX_OK;
			SFile >> nPhysX_OK;

			SFile.CloseFile();

			// 이전에 PhysX 가 정상동작 하였는가~?
			if ( nPhysX_OK )
			{
				//////////////////////////////////////////////////////////////////////////
				// 정상동작 되었었다.
				InitNx( pd3dDevice );
				return;
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				// 이전에 문제가 되었었다.
				return;
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			// 처음 작업된다.

			// 파일이 없다면 파일을 만들고 로딩을 하고 삭제를 다시 한다.
			int fh;

			// 처음이면 폴더는 없을 수 있다.
			CreateDirectory ( strFolderPath.c_str(), NULL );

			// 파일을 새로 만들도록 한다.
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
			// g_setNxActor 정리
			{
				SET_NXACTOR_ITER iter = g_setNxActor.begin();
				for ( ; iter!=g_setNxActor.end(); )
				{
					SET_NXACTOR_ITER iterDel = iter++;
					DeleteActor( (*iterDel) );
				}
			}

			// g_setNxJoint 정리
			{
				SET_NXJOINT_ITER iter = g_setNxJoint.begin();
				for ( ; iter!=g_setNxJoint.end(); )
				{
					SET_NXJOINT_ITER iterDel = iter++;
					DeleteJoint( (*iterDel) );
				}
			}

			// g_setNxCloth 정리
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

		// 충돌처리를 켜거나 끕니다.
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

		// [2012.4.4] 밑에 코드가 의문이 듬.
		//				여러번 돌면 성공하는 건인가~?
		//				그렇다면 말이 안되게 실패하는 경우가 있는가~?
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

		// [2012.4.4] 밑에 코드가 의문이 듬.
		//				여러번 돌면 성공하는 건인가~?
		//				그렇다면 말이 안되게 실패하는 경우가 있는가~?
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

		// [2012.4.4] 밑에 코드가 의문이 듬.
		//				여러번 돌면 성공하는 건인가~?
		//				그렇다면 말이 안되게 실패하는 경우가 있는가~?
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
		MAP_NXACTOR mapNxActor;			// 데이터 참조용.

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

			// [2012.4.4] 밑에 코드가 의문이 듬.
			//				여러번 돌면 성공하는 건인가~?
			//				그렇다면 말이 안되게 실패하는 경우가 있는가~?
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

			vecNxActor.push_back( pActor );															// 실제 정보를 가지고 있는 곳
			mapNxActor.insert( std::make_pair( vecBoneTrans[i].m_rBoneTransJoint->GetName(), pActor ) );	// 허상 정보
		}

		D3DXVECTOR3 vJointOffset(0.f,0.f,0.f);
		D3DXVECTOR3 vDir;
		for ( size_t i = 1; i < nbSegments-1; i++)
		{
			if ( vecBoneTrans[i].m_rBoneTransChild )
			{
				vJointOffset = vecBoneTrans[i].m_rBoneTransChild->m_pQuatPosORIG.m_vPos * fScale;
			}

			// [2012.4.4] 밑에 코드가 의문이 듬.
			//				여러번 돌면 성공하는 건인가~?
			//				그렇다면 말이 안되게 실패하는 경우가 있는가~?
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
					// 기준 Bone 이기 때문에 Joint 쪽으로 붙이면 안된다.
					// 다음 Bone 과의 거리는 유지되야 되기 때문에 0.5f 를 하면 안된다.
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
					(*iterParent).second,			// the position and orientation of the joint relative to actor0	// actor0 를 판단 기준으로 joint 위치와 방향
					poseParent,
					(*iterJoint).second, 
					poseJoint						// the position and orientation of the joint relative to actor1	// actor1 를 판단 기준으로 joint 위치와 방향
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

			// 추가
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
		// PxClothMeshDesc 정보 얻어오기.
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
		// PxCloth 정보 얻어오기.
		// create the cloth
		std::vector<PxClothCollisionSphere> vecSpheres;
		std::vector<DWORD>					vecIndexPairs;
		rClothColl->GetAllClothCollData( vecSpheres, vecIndexPairs, fAttParentBoneScale );	// const 라서 rClothColl 에 쓰지는 않음.

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

		// Manager 에 넣기
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

			// Mesh 의 Diffuse Alpha 값으로 수치를 정한다.
			if ( arrayInvWeight )
			{
				if ( arrayInvWeight[i] == 0.f )
				{
					particles[i].invWeight = 0.f;
				}
				else
				{
					particles[i].invWeight = arrayInvWeight[i] + 10.f;	// 큰값이다. 튐현상을 줄일수 있다. 끼임현상은 생긴다.
				}
			}
			else
			{
				particles[i].invWeight = 0.f;
			}

			//////////////////////////////////////////////////////////////////////////
			// AttachParentBone 의 Scale 변화 의 3 설명 ( /fAttParentBoneScale )
			//////////////////////////////////////////////////////////////////////////
			// 3. LockParticle 은 Bone_Matrix 와 연산이 되기 때문에, 작게 만들면 안되고, 사이즈가 원본 사이즈로 되어 있어야 한다.
			//////////////////////////////////////////////////////////////////////////
			//
			// LockParticle 를 얻는다.
			if ( particles[i].invWeight == 0.f )
			{
				LockParticle sLockParticle;
				sLockParticle.m_dwIndex = i;
				sLockParticle.m_vPosition.x = particles[i].pos.x/fAttParentBoneScale;
				sLockParticle.m_vPosition.y = particles[i].pos.y/fAttParentBoneScale;
				sLockParticle.m_vPosition.z = particles[i].pos.z/fAttParentBoneScale;
				vecLockParticle.push_back( sLockParticle );
			}

			// [shhan][2014.11.20] 실제 위치로 움직여줘야 한다.
			//						이걸 하지 않으면 Cloth 가 튀는 현상이 심하게 보일 것이다.
			D3DXVECTOR3* pPos = reinterpret_cast<D3DXVECTOR3*>( &particles[i].pos );
			D3DXVec3TransformCoord( pPos, pPos, &matWorld );

		}
		readData->unlock();
		cloth.setParticles(particles.begin(), particles.begin());

		// set solver settings
		//cloth.setSolverFrequency(240);
		cloth.setSolverFrequency( static_cast<physx::PxReal>( sMaterialSystem.m_nClothSolverFrequency ) );		// 

		// damp global particle velocity to 90% every 0.1 seconds
		//cloth.setDampingCoefficient(0.1f); // damp local particle velocity // 감쇠 계수
		cloth.setDampingCoefficient( sMaterialSystem.m_fClothDampingCoefficient ); // damp local particle velocity
		//cloth.setDragCoefficient(0.1f); // transfer frame velocity // 공기 저항 계수
		cloth.setDragCoefficient(0.1f); // transfer frame velocity	

		// reduce effect of local frame acceleration
		//cloth.setInertiaScale(0.3f);	// 글로벌 포즈 변화에 관성 효과를 조정하기 위해 가속 축척 비율을 설정합니다. 
		cloth.setInertiaScale(0.f);


		//const bool useCustomConfig = true;

		//// virtual particles
		//if (useVirtualParticles)
		//{
		//	//////////////////////////////////////////////////////////////////////////
		//	// 천 시뮬레이션은 충돌Bone 과 천의 삼각형 끼이는 현상이 자주 발생 할 수 있다.
		//	// 보통은 끼여도 대충 흘러내리면서 나온다.
		//	// createVirtualParticles 셋팅을 하면은 끼이면서 나오질 못하고 지속되는 문제가 보인다.
		//	// 
		//	//NSClothHelpers::createVirtualParticles(cloth, meshDesc, 4);
		//	//NSClothHelpers::createVirtualParticles(cloth, meshDesc, 7);
		//	//NSClothHelpers::createVirtualParticles(cloth, meshDesc, 13);
		//}

		//////////////////////////////////////////////////////////////////////////
		// 천 시뮬레이션은 충돌Bone 과 천의 삼각형 끼이는 현상이 자주 발생 할 수 있다.
		// 보통은 끼여도 대충 흘러내리면서 나온다.
		// eSWEPT_CONTACT : true 셋팅을 하면은 끼이면서 나오질 못하고 지속되는 문제가 보인다.
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

		// setInertiaScale 가 0 일 경우 작업할 필요가 없다.
		// set cloth pose
		//pCloth->setTargetPose(pose);

		// 위치정보를 얻었음.
		SampleArray<PxVec3> positions;
		NSClothHelpers::getParticlePositions( *pCloth, positions );

		//////////////////////////////////////////////////////////////////////////
		// Normal 을 얻는다.
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

			// 얼마나 이동했나에 따라서 InvWeight 값을 변화시킨다.
			vPosDiffAdd.x = vPosAffectMatrix.x - vPhysPos->x;
			vPosDiffAdd.y = vPosAffectMatrix.y - vPhysPos->y;
			vPosDiffAdd.z = vPosAffectMatrix.z - vPhysPos->z;
		}

		bool useConstrainedOnly(true);
		if ( D3DXVec3Length( &vPosDiffAdd ) > 6.f )
		{
			// rm #3228 - [아이템]마술부 망또 수정 요청
			// 너무 이동이 크면 튐현상의 문제라고 보인다. 전체점을 움직이도록 한다.
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
				
				// 얼마나 이동했나에 따라서 InvWeight 값을 변화시킨다.
				vPosDiffAdd.x = vPosAffectMatrix.x - vPhysPos->x;
				vPosDiffAdd.y = vPosAffectMatrix.y - vPhysPos->y;
				vPosDiffAdd.z = vPosAffectMatrix.z - vPhysPos->z;
				fInvWeight_AddValue += D3DXVec3Length( &vPosDiffAdd );

				memcpy( vPhysPos, vPosAffectMatrix, sizeof(float)*3 );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 이값으로 인해 캐릭터에 의한 천 움직임이 클 때는 천 튐현상을 줄여주고,
		//				캐릭터에 의한 천 움직임이 적을 때는 캐릭터와 충돌처리의 걸림현상을 줄여준다.
		// 수치가 크면 튐현상이 적고, 충돌현상은 많아진다.
		// 수치가 적으면 튐현상은 많고, 충돌현상은 줄어든다.
		fInvWeight_AddValue = fInvWeight_AddValue/vecLockParticle.size();
		if ( fInvWeight_AddValue > 3.f )
		{
			// 제곱연산 오류가 생길꺼 같아서 미리 대입한다.
			fInvWeight_AddValue = 81.f;
		}
		else
		{
			fInvWeight_AddValue = fInvWeight_AddValue * fInvWeight_AddValue;	// 범위를 크게 만든다.
			fInvWeight_AddValue = fInvWeight_AddValue * fInvWeight_AddValue;	// 범위를 크게 만든다.
			fInvWeight_AddValue *= 1.f;		// MagicNumber
		}

		//////////////////////////////////////////////////////////////////////////
		// 형상에 넣어주도록 한다.
		PxVec3* pPos(NULL);
		PxVec3* pNor(NULL);
		UINT nVertSize = D3DXGetDeclVertexSize( pDeclaration, 0 );		

		int numVertices	= pMesh->GetNumVertices();

		//////////////////////////////////////////////////////////////////////////
		//모든 파티클 정점 정보를 Vertex Buffer로 복사한다.
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
		//// PxCloth 정보 얻어오기.	충돌관련.
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
			// [shhan][2014.05.09] Cloth 때문에 GetPhysicsResults 순서가 변경되었다.
			//						추측이지만 연산중에는 cloth.lockClothReadData(); 값을 못얻어오는 듯 하다.
			//						그래서 결과를 기다리는 듯.
			GetPhysicsResults();
		}

		//// 와이어프레임 렌더링.
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