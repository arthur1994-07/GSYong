#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include "./SimpleCollisionDef.h"

namespace sc
{
	class SerialFile;
	class BaseStream;
};

class SimpleCollisionBase;

// Layer 별로 셋팅된 Collision
class LayerCollision
{
private:
	enum
	{
		VERSION = 0x100,	
	};

private:
	BOOL												m_bActiveForClient;		// Server 쪽은 MobLayer 활성화 여부로 알 수 있다.
	std::vector<boost::shared_ptr<SimpleCollisionBase>>	m_vecSimpleCollision;

public:
	void SetActiveLayer();
	void SetDeActiveLayer();
	BOOL IsActiveLayer() const;
	BOOL IsExistSimpleCollision() const;
	SIMPLE_COLLISION_RESULT IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset ) const;

public:
	// Edit
	const SimpleCollisionBase*	FindCollision ( const char* szName ) const;
	void						AddCollision ( boost::shared_ptr<SimpleCollisionBase> spNew );
	void						ChangeCollision( UINT unIndex, boost::shared_ptr<SimpleCollisionBase> spNew );
	void						DelCollision ( UINT unIndex );
	void						SetObjRotate( float fAddAngle );
	const std::vector<boost::shared_ptr<SimpleCollisionBase>>&	GetvecSimpleCollision() const;

public:
	friend class CLevelCollision;

public:
	void Save( sc::SerialFile &SFile );
	void Load( sc::BaseStream &SFile );

public:
	LayerCollision( BOOL bActive );
	virtual ~LayerCollision();
};