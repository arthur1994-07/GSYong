#include "pch.h"

#include "../../../RanThirdParty/PhysX_3_2/include/foundation/PxTransform.h"
#include "../../../RanThirdParty/PhysX_3_2/include/geometry/PxGeometry.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxRigidDynamic.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxRigidActor.h"
#include "../../../RanThirdParty/PhysX_3_2/include/PxShape.h"

#include "../../DxTools/DxMethods.h"
#include "../../Common/StlFunctions.h"
#include "../../PhysX/NSPhysX.h"

#include "./DxSkeletonMaintain.h"
#include "./DxSkeletonManager.h"

#include "DxJiggleBone.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------
//					D	x		J	i	g	g	l	e		B	o	n	e		D	a	t	a
//------------------------------------------------------------------------------------------------------------
JiggleBoneData::JiggleBoneData()
	: m_strStartJiggleBone(_T(""))
	, m_bJiggleBoneColl(FALSE)
	, m_dwJointNumberUSED(0)
{

}

JiggleBoneData::~JiggleBoneData()
{
	m_vecJointData.clear();
	m_bJiggleBoneColl = FALSE;
	m_dwJointNumberUSED = 0;

	CleanUp();
}

void JiggleBoneData::CleanUp()
{
	m_vecBoneTransJointData.clear();

	for ( DWORD z=0; z<m_vecNxActor.size(); ++z )
	{
		NSPhysX::DeleteActor( m_vecNxActor[z] );
	}
	for ( DWORD z=0; z<m_vecNxJoint.size(); ++z )
	{
		NSPhysX::DeleteJoint( m_vecNxJoint[z] );
	}
	m_vecNxActor.clear();
	m_vecNxJoint.clear();
}

// ��� Thread �� ���۵����ʴ� ��Ȳ���� ȣ��ȴ�.
BOOL JiggleBoneData::CreateJiggleBone( DxSkeleton* pSkeleton, float fScale )
{
	CleanUp();

	if ( !NSPhysX::IsPhysX() )
		return FALSE;

	if ( m_vecJointData.size() <= 1 )
		return FALSE;

	if ( m_dwJointNumberUSED <= 0 )
		return FALSE;

	DxBoneTransJointData sData;

	DxBoneTrans* pBone(NULL);
	DxBoneTrans* pBoneNext(NULL);
	D3DXMATRIX matInvese;
	sData.m_rBoneTransParent = pSkeleton->FindBone_t( m_vecJointData[0]->m_strBoneParent );
	sData.m_rBoneTransJoint = pSkeleton->FindBone_t( m_vecJointData[0]->m_strBoneParent );
	sData.m_rBoneTransChild = pSkeleton->FindBone_t( m_vecJointData[0]->m_strBoneJoint );
	sData.m_sJointData = *m_vecJointData[0];
	m_vecBoneTransJointData.push_back( sData );

	for ( DWORD i=0; i<m_vecJointData.size() && i<m_dwJointNumberUSED; ++i )
	{
		sData.m_rBoneTransParent = pSkeleton->FindBone_t( m_vecJointData[i]->m_strBoneParent );
		sData.m_rBoneTransJoint = pSkeleton->FindBone_t( m_vecJointData[i]->m_strBoneJoint );
		if ( m_vecJointData[i]->m_strBoneChild.empty() )
		{
			sData.m_rBoneTransChild = NULL;
		}
		else
		{
			sData.m_rBoneTransChild = pSkeleton->FindBone_t( m_vecJointData[i]->m_strBoneChild );
		}
		sData.m_sJointData = *m_vecJointData[i];
		m_vecBoneTransJointData.push_back( sData );
	}

	// ���⼭ Matrix�� �����������.
	D3DXMATRIX matCur;
	D3DXMATRIX matRotate;
	D3DXMATRIX matRotat2;
	D3DXMATRIX matParent;
	D3DXMATRIX matScale;
	D3DXMATRIX matCombined;
	D3DXVECTOR3 vRotate;
	pBone = pSkeleton->FindBone_t( m_vecJointData[0]->m_strBoneParent );
	if ( !pBone )
	{
		TSTRING strText;
		strText = _T("JiggleBone Error - ");
		strText += m_vecJointData[0]->m_strBoneParent;
		AfxMessageBox( strText.c_str() );
		return FALSE;
	}
	matParent = pBone->matCombined;

	for ( DWORD i=0; i<m_vecJointData.size() && i<m_dwJointNumberUSED; ++i )
	{
		pBone = pSkeleton->FindBone_t( m_vecJointData[i]->m_strBoneJoint );
		if ( !pBone )
		{
			TSTRING strText;
			strText = _T("JiggleBone Error - ");
			strText += m_vecJointData[i]->m_strBoneJoint;
			AfxMessageBox( strText.c_str() );
			return FALSE;
		}
		
		vRotate = D3DXVECTOR3( m_vecJointData[i]->m_vRotate.x*D3DX_PI/180.f,
								m_vecJointData[i]->m_vRotate.y*D3DX_PI/180.f, 
								m_vecJointData[i]->m_vRotate.z*D3DX_PI/180.f );

		D3DXMatrixScaling( &matScale, pBone->m_pQuatPosCUR.m_vScale.x, pBone->m_pQuatPosCUR.m_vScale.y, pBone->m_pQuatPosCUR.m_vScale.z );
		D3DXMatrixRotationQuaternion( &matRotate, &pBone->m_pQuatPosCUR.m_vQuat );
		D3DXMatrixMultiply( &matCombined, &matScale, &matRotate );

		D3DXMatrixRotationYawPitchRoll( &matCur, vRotate.y, vRotate.x, vRotate.z );
		D3DXMatrixMultiply( &matCombined, &matCur, &matCombined );

		matCombined._41 = pBone->m_pQuatPosCUR.m_vPos.x;
		matCombined._42 = pBone->m_pQuatPosCUR.m_vPos.y;
		matCombined._43 = pBone->m_pQuatPosCUR.m_vPos.z;
		D3DXMatrixMultiply( &pBone->matCombined, &matCombined, &matParent );
		matParent = pBone->matCombined;
	}

	// Jiggle Bone �̶�� ���� Bone �� ������ ���ֵ��� �Ѵ�.
	{
		// JiggleBone �̶� �����Ѵ�.
		for ( DWORD i=0; i<m_vecJointData.size()-1 && i<m_dwJointNumberUSED-1; ++i )
		{
			DxBoneTrans* pBoneTransJoint = pSkeleton->FindBone_t( m_vecJointData[i]->m_strBoneJoint );
			if ( pBoneTransJoint )
			{
				pBoneTransJoint->SetFlag_JiggleBone( TRUE );
			}
		}
	}

	// ���⼭ Edit �� ������ �����ش�.
	// m_vecJointData - for������ ������ �̰� ���
	// m_vecBoneTransJointData �� m_vecJointData ���� 1 �� ���� ������ ��� ������.
	for ( DWORD i=0; i<m_vecJointData.size()-1 && i<m_dwJointNumberUSED-1; ++i )
	{
		pBone = m_vecBoneTransJointData[i+1].m_rBoneTransJoint;
		pBoneNext = m_vecBoneTransJointData[i+2].m_rBoneTransJoint;
		m_vecJointData[i]->m_vBoneDir.x = pBoneNext->matCombined._41 - pBone->matCombined._41;
		m_vecJointData[i]->m_vBoneDir.y = pBoneNext->matCombined._42 - pBone->matCombined._42;
		m_vecJointData[i]->m_vBoneDir.z = pBoneNext->matCombined._43 - pBone->matCombined._43;
		D3DXMatrixInverse( &matInvese, NULL, &pBone->matCombined );
		D3DXVec3TransformNormal( &m_vecJointData[i]->m_vBoneDir, &m_vecJointData[i]->m_vBoneDir, &matInvese );
	}

	if ( NSPhysX::CreateJoint( m_vecBoneTransJointData, m_vecJointData, m_vecNxActor, m_vecNxJoint, fScale, m_bJiggleBoneColl ) )
		return TRUE;

	return FALSE;
}

// ���� Thread ���� ȣ�� �ȴ�.
void JiggleBoneData::JiggleBoneMove_MainThread( const D3DXVECTOR3& vDir )
{
	if ( m_vecJointData.empty() )
		return;

	physx::PxTransform sTransform;
	for ( DWORD i=1; i<m_vecNxActor.size(); ++i )
	{
		sTransform = m_vecNxActor[i]->getGlobalPose();
		sTransform.p.x += vDir.x;
		sTransform.p.y += vDir.y;
		sTransform.p.z += vDir.z;
		m_vecNxActor[i]->setGlobalPose( sTransform );
	}
}

// ���� Thread ���� ȣ�� �ȴ�.
void JiggleBoneData::FrameMove_MainThread( const DxSkeletonMaintain& sSkeleton )
{
	if ( m_vecBoneTransJointData.empty() )
		return;

	//const DxBoneTrans* pBone = m_vecBoneTransJointData[0].m_rBoneTransJoint;
	const DxBoneTransMaintain* pBoneTransMaintain = sSkeleton.FindBone( m_vecBoneTransJointData[0].m_rBoneTransJoint->m_strName );
	if ( !pBoneTransMaintain )
		return;

	const D3DXMATRIX& matParent = pBoneTransMaintain->m_matCombined;

	D3DXVECTOR3 vScale;
	D3DXQUATERNION vQuat;
	D3DXVECTOR3 vPos;
	D3DXMatrixToSQT( vScale, vQuat, vPos, matParent );

	physx::PxTransform pose;
	pose.q.x = vQuat.x;
	pose.q.y = vQuat.y;
	pose.q.z = vQuat.z;
	pose.q.w = vQuat.w;
	pose.p.x = matParent._41;
	pose.p.y = matParent._42;
	pose.p.z = matParent._43;
	m_vecNxActor[0]->setGlobalPose( pose );		// ������ �� �ִ�. PxRigidActor �� setGlobalPose Thread Safe �Ѱ�~?
}


// �ٸ� Thread ���� ȣ�� �ȴ�.
// m_pActor->setGlobalPose( pose ); ����ȴ�.
// m_vecBoneTransJointData[i].m_rBoneTransJoint->matCombined ���� ����ȴ�.
void JiggleBoneData::FrameMove_PureThread( DxSkeleton* pSkeleton, float fKeyJiggleWeight )
{
	if ( m_vecBoneTransJointData.empty() )
		return;

	const DxBoneTrans* pBone = m_vecBoneTransJointData[0].m_rBoneTransJoint;

	D3DXMATRIX matParent = pBone->matCombined;

	D3DXVECTOR3 vScale;
	D3DXQUATERNION vQuat;
	D3DXVECTOR3 vPos;
	D3DXMatrixToSQT( vScale, vQuat, vPos, matParent );

	//physx::PxTransform pose;
	//pose.q.x = vQuat.x;
	//pose.q.y = vQuat.y;
	//pose.q.z = vQuat.z;
	//pose.q.w = vQuat.w;
	//pose.p.x = matParent._41;
	//pose.p.y = matParent._42;
	//pose.p.z = matParent._43;
	//m_vecNxActor[0]->setGlobalPose( pose );		// ������ �� �ִ�. PxRigidActor �� setGlobalPose Thread Safe �Ѱ�~?

	D3DXQUATERNION vQuatInv;
	D3DXQuaternionInverse( &vQuatInv, &vQuat );

	// �θ� Matrix
	D3DXMATRIX matCur;
	D3DXMatrixIdentity( &matCur );

	D3DXVECTOR3 vDir(0.f,0.f,0.f);
	D3DXMATRIX matActor;
	//physx::PxRigidDynamic* pActor(NULL);
	for ( DWORD i=1; i<m_vecNxActor.size(); ++i )
	{
		//// ���� Bone�� ���� Matrix�� ��´�. < PhysX ���� ��������� >
		//pActor = m_vecNxActor[i]->is<physx::PxRigidDynamic>();
		//pActor->addForce( physx::PxVec3(0.f,-0.01f*fElapsedTime,0.f) );					// �������� ���缭 NX_BF_KINEMATIC�� ������ ����� �� ����

		{
			const physx::PxRigidActor* pRigidActor = m_vecNxActor[i];
			if ( pRigidActor )
			{
				const physx::PxTransform& pose = pRigidActor->getGlobalPose();

				D3DXQUATERNION vQuatJiggle;
				vQuatJiggle.x = pose.q.x;
				vQuatJiggle.y = pose.q.y;
				vQuatJiggle.z = pose.q.z;
				vQuatJiggle.w = pose.q.w;

				// Key �� JiggleBone�� ������ �Ѵ�.
				D3DXQuaternionSlerp( &vQuatJiggle, &vQuat, &vQuatJiggle, fKeyJiggleWeight );				

				// ���� Bone ��ü�� ȸ���� ��´�.
				D3DXQUATERNION vQuatNew;
				D3DXQuaternionMultiply( &vQuatNew, &vQuatJiggle, &vQuatInv );

				// Matrix �� �����.
				D3DXMatrixRotationQuaternion( &matCur, &vQuatNew );

				// ���� Bone�� ȸ���� ��� ���� Inv �� ���Ѵ�.
				D3DXQuaternionInverse( &vQuatInv, &vQuatJiggle );
			}
		}


		// ���� Bone ��ü�� �̵��� ��´�.
		const SQuatPos* pQuatPos = &m_vecBoneTransJointData[i].m_rBoneTransJoint->m_pQuatPosORIG;
		matCur._41 = pQuatPos->m_vPos.x;
		matCur._42 = pQuatPos->m_vPos.y;
		matCur._43 = pQuatPos->m_vPos.z;

		// PhysX�� ȸ���� �Ϲ� ��ġ�� ���� Matrix�� matCombined�� �ִ´�.
		D3DXMatrixMultiply( &m_vecBoneTransJointData[i].m_rBoneTransJoint->matCombined, &matCur, &matParent );

		// ���ȣ���� ���� ���� �ִ´�.
		matParent = m_vecBoneTransJointData[i].m_rBoneTransJoint->matCombined;
	}

	// �� Bone �� JiggleBone�� ������ �޵��� ������.
	if ( m_vecNxActor.size() < m_vecBoneTransJointData.size() )
	{
		DxBoneTrans* pBone = m_vecBoneTransJointData[m_vecNxActor.size()].m_rBoneTransJoint;
		if ( pBone && pSkeleton )
		{
			// �� ���� Bone �鵵 ������ֵ��� ����.
			pSkeleton->UpdateBones_PureThread( pBone, matParent, 1.f, 1.f );
		}
	}
}



void JiggleBoneData::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX matActor;
	physx::PxRigidDynamic* pActor(NULL);
	for ( DWORD i=1; i<m_vecNxActor.size(); ++i )
	{
		// ���� Bone�� ���� Matrix�� ��´�. < PhysX ���� ��������� >
		pActor = m_vecNxActor[i]->is<physx::PxRigidDynamic>();
		NSPhysX::GetMatrix( matActor, m_vecNxActor[i] );

		physx::PxShape* shape; 
		m_vecNxActor[i]->getShapes(&shape, 1);
		physx::PxGeometryType::Enum emGeometryType = shape->getGeometryType();

		switch ( emGeometryType )
		{
		case physx::PxGeometryType::eSPHERE:
			NSPhysX::DrawSphere( shape, matActor, 0xffffffff );
			break;

		case physx::PxGeometryType::eCAPSULE:
			NSPhysX::DrawCapsule( shape, matActor, 0xffffffff );
			break;

		case physx::PxGeometryType::eBOX:
			NSPhysX::DrawBox( shape, matActor, 0xffffffff );
			break;

		default:
			break;
		};
	}
}

// ��� Thread �� ���۵����ʴ� ��Ȳ���� ȣ��ȴ�.
// ������ ����Ǳ� ���� �����͵��� ������ �����.
BOOL JiggleBoneData::Import( const JiggleBoneData* pSrc, DxSkeleton* pSkeleton, float fScale )
{
	if ( !pSrc )
	{
		return FALSE;
	}

	if ( !pSkeleton )
	{
		return FALSE;
	}

	m_vecJointData = pSrc->m_vecJointData;
	m_bJiggleBoneColl = pSrc->m_bJiggleBoneColl;
	m_dwJointNumberUSED = pSrc->m_dwJointNumberUSED;

	if( CreateJiggleBone( pSkeleton, fScale ) )
		return TRUE;

	return FALSE;
}

BOOL JiggleBoneData::CreateJointData( const DxSkeleton* pSkeleton, const TSTRING& strBoneName )
{
	m_vecJointData.clear();

	m_strStartJiggleBone = strBoneName;

	const DxBoneTrans* pBoneTras = pSkeleton->FindBone_t( strBoneName );
	if ( !pBoneTras )
		return FALSE;

	if ( !pBoneTras->pBoneTransParent )
		return FALSE;

	CreateJointSub( pBoneTras );

	// �ڵ� �е� ����.
	for ( DWORD i=0; i<m_vecJointData.size(); ++i )
	{
		int nCount = static_cast<int>( m_vecJointData.size() ) - i - 2;
		nCount = nCount < 0 ? 0 : nCount;
		float fDensity = static_cast<float>( powl(2,nCount) );
		m_vecJointData[i]->m_fDensity = fDensity;
	}

	m_dwJointNumberUSED = m_vecJointData.size();

	return TRUE;
}

void JiggleBoneData::CreateJointSub( const DxBoneTrans* pBoneCur )
{
	std::tr1::shared_ptr<DxJointData> spNew = std::tr1::shared_ptr<DxJointData>(new DxJointData);
	spNew->m_strBoneParent = pBoneCur->pBoneTransParent->GetName();
	spNew->m_strBoneJoint = pBoneCur->GetName();
	if ( pBoneCur->pBoneTransFirstChild )
	{
		spNew->m_strBoneChild = pBoneCur->pBoneTransFirstChild->GetName();
	}
	else
	{
		spNew->m_strBoneChild = _T("");
	}
	m_vecJointData.push_back( spNew );

	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
	DxBoneTrans *pBoneChild = pBoneCur->pBoneTransFirstChild;
	while( pBoneChild )
	{
		CreateJointSub( pBoneChild );
		pBoneChild = pBoneChild->pBoneTransSibling;
	}
}

void JiggleBoneData::DeleteJoint( int nIndex )
{
	m_dwJointNumberUSED = nIndex+1;
}

void JiggleBoneData::ResetJoint()
{
	m_dwJointNumberUSED = m_vecJointData.size();
}

void JiggleBoneData::SetCollision( BOOL bCollision )
{
	m_bJiggleBoneColl = bCollision;
}

void JiggleBoneData::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << m_strStartJiggleBone;
		SFile << m_bJiggleBoneColl;
		SFile << m_dwJointNumberUSED;

		DWORD dwSize = static_cast<DWORD>( m_vecJointData.size() );
		SFile << dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			m_vecJointData[i]->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void JiggleBoneData::Load( sc::BaseStream& SFile )
{
	CleanUp();
	m_bJiggleBoneColl = FALSE;
	m_dwJointNumberUSED = 0;
	m_vecJointData.clear();

	DWORD dwVersion, dwBufferSize;
	SFile >> dwVersion;
	SFile >> dwBufferSize;

	if( dwVersion == VERSION )
	{
		SFile >> m_strStartJiggleBone;
		SFile >> m_bJiggleBoneColl;
		SFile >> m_dwJointNumberUSED;

		DWORD dwSize(0);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			std::tr1::shared_ptr<DxJointData> spNew = std::tr1::shared_ptr<DxJointData>(new DxJointData);
			spNew->Load( SFile );
			m_vecJointData.push_back( spNew );
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}

//------------------------------------------------------------------------------------------------------------
//								D	x		J	i	g	g	l	e		B	o	n	e
//------------------------------------------------------------------------------------------------------------
DxJiggleBone::DxJiggleBone()
	: m_vPositionPREV(0.f,0.f,0.f)
{
}

DxJiggleBone::~DxJiggleBone()
{
	CleanUp();
}

void DxJiggleBone::CleanUp()
{
	m_vecJiggleBoneData.clear();
}

// ��� Thread �� ���۵����ʴ� ��Ȳ���� ȣ��ȴ�.
void DxJiggleBone::CreateJiggleBone( DxSkeleton* pSkeleton, float fScale )
{
	for ( DWORD i=0; i<m_vecJiggleBoneData.size(); ++i )
	{
		m_vecJiggleBoneData[i]->CreateJiggleBone( pSkeleton, fScale );
	}
}

// �ٸ� Thread ���� ȣ�� �ȴ�.
void DxJiggleBone::FrameMove_PureThread( DxSkeleton* pSkeleton, float fKeyJiggleWeight )
{
	for ( DWORD i=0; i<m_vecJiggleBoneData.size(); ++i )
	{
		m_vecJiggleBoneData[i]->FrameMove_PureThread( pSkeleton, fKeyJiggleWeight );
	}
}

// Main Thread ���� ȣ�� �ȴ�.
void DxJiggleBone::FrameMove_MainThread( const DxSkeletonMaintain& sSkeleton, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR3 vDir;
	vDir.x = matWorld._41 - m_vPositionPREV.x;
	vDir.y = matWorld._42 - m_vPositionPREV.y;
	vDir.z = matWorld._43 - m_vPositionPREV.z;
	float fLength = D3DXVec3Length( &vDir );
	if ( fLength > 10.f )
	{
		// �ʹ� ū �̵��� ���� ���� PhysX �� ��ü�� ���� �̵� �����ش�.
		for ( DWORD i=0; i<m_vecJiggleBoneData.size(); ++i )
		{
			m_vecJiggleBoneData[i]->JiggleBoneMove_MainThread( vDir );
		}
	}

	for ( DWORD i=0; i<m_vecJiggleBoneData.size(); ++i )
	{
		m_vecJiggleBoneData[i]->FrameMove_MainThread( sSkeleton );
	}
	memcpy( &m_vPositionPREV, &matWorld._41, sizeof(D3DXVECTOR3) );
}

void DxJiggleBone::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecJiggleBoneData.size(); ++i )
	{
		m_vecJiggleBoneData[i]->RenderEdit( pd3dDevice );
	}
}

// ������ ���� Main Thread �̴�. ��� Thread �� ������ �ʴ´�.
// ������ ����Ǳ� ���� �����͵��� ������ �����.
void DxJiggleBone::Import( const DxJiggleBone* pSrc, DxSkeleton* pSkeleton, float fScale )
{
	CleanUp();

	// JiggleBone �̶�� ������ �ʱ�ȭ �Ѵ�.
	pSkeleton->SetFlag_JiggleBoneAllFalse( pSkeleton->m_skeletonPart.pBoneRoot );

	if ( !pSrc )
		return;

	for ( DWORD i=0; i<pSrc->m_vecJiggleBoneData.size(); ++i )
	{
		std::tr1::shared_ptr<JiggleBoneData> spNew = std::tr1::shared_ptr<JiggleBoneData>(new JiggleBoneData);
		if( spNew->Import( pSrc->m_vecJiggleBoneData[i].get(), pSkeleton, fScale ) )
			m_vecJiggleBoneData.push_back( spNew );
	}
}

void DxJiggleBone::Insert( const DxSkeleton* pSkeleton, const TSTRING& strBoneName )
{
	for ( DWORD i=0; i<m_vecJiggleBoneData.size(); ++i )
	{
		if ( strBoneName == m_vecJiggleBoneData[i]->m_strStartJiggleBone )
		{
			m_vecJiggleBoneData[i]->CreateJointData( pSkeleton, strBoneName );
			return;
		}
	}

	// ���ٸ� ���� �߰��Ѵ�.
	std::tr1::shared_ptr<JiggleBoneData> spNew = std::tr1::shared_ptr<JiggleBoneData>(new JiggleBoneData);
	if ( spNew->CreateJointData( pSkeleton, strBoneName ) )
	{
		m_vecJiggleBoneData.push_back( spNew );
	}
}

void DxJiggleBone::Delete( int nIndex )
{
	int nCount(0);
	VEC_JIGGLEBONEDATA_ITER iter = m_vecJiggleBoneData.begin();
	for ( ; iter!=m_vecJiggleBoneData.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			m_vecJiggleBoneData.erase( iter );
			return;
		}
	}
}

void DxJiggleBone::SetCollision( int nIndex, BOOL bCollision )
{
	int nCount(0);
	VEC_JIGGLEBONEDATA_ITER iter = m_vecJiggleBoneData.begin();
	for ( ; iter!=m_vecJiggleBoneData.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			(*iter)->SetCollision( bCollision );
			return;
		}
	}
}

JiggleBoneData* DxJiggleBone::GetJiggleBone( int nIndex )
{
	int nCount(0);
	VEC_JIGGLEBONEDATA_ITER iter = m_vecJiggleBoneData.begin();
	for ( ; iter!=m_vecJiggleBoneData.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			return (*iter).get();
		}
	}

	return NULL;
}

void DxJiggleBone::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		DWORD dwSize = static_cast<DWORD>( m_vecJiggleBoneData.size() );
		SFile << dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			m_vecJiggleBoneData[i]->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void DxJiggleBone::Load( sc::BaseStream& SFile )
{
	CleanUp();

	DWORD dwVersion, dwBufferSize;
	SFile >> dwVersion;
	SFile >> dwBufferSize;

	if( dwVersion == VERSION )
	{
		DWORD dwSize(0);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			std::tr1::shared_ptr<JiggleBoneData> spNew = std::tr1::shared_ptr<JiggleBoneData>(new JiggleBoneData);
			spNew->Load( SFile );
			m_vecJiggleBoneData.push_back( spNew );
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}
