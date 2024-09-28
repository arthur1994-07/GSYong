#pragma once

#include "../line2d.h"
#include "./SimpleCollisionDef.h"

namespace sc
{
	class SerialFile;
	class BaseStream;
};

//////////////////////////////////////////////////////////////////////////
//						SIMPLE_COLLISION_TYPE
//////////////////////////////////////////////////////////////////////////
enum SIMPLE_COLLISION_TYPE
{
	EMSCT_PLANE,
	EMSCT_BOX,
	EMSCT_CYLINDER,
};


//////////////////////////////////////////////////////////////////////////
//						SimpleCollisionBase
//						Static ( Dont`t Move )
//////////////////////////////////////////////////////////////////////////
class SimpleCollisionBase
{
public:
	enum
	{
		NAME_MAXSZ = 33,
		BASE_VERSION = 0x100,
	};

private:
	SIMPLE_COLLISION_TYPE	m_emType;

public:
	SIMPLE_COLLISION_TYPE GetType() const	{ return m_emType; }

protected:
	char					m_szName[NAME_MAXSZ];
	D3DXVECTOR3				m_vPosition;

public:
	const char*			GetName() const		{ return m_szName; }
	const D3DXVECTOR3&	GetPosition() const	{ return m_vPosition; }

public:
	virtual void SetObjRotate( float fAddAngle );
	virtual void SetPosisionRotation( const D3DXVECTOR3& vPosition, float fRotationY )
	{
		m_vPosition = vPosition;
	}

public:
	// return ���� COLL_TRUE_START_IN ��� vOut �� ĳ���� ��ġ�� �����������.
	virtual SIMPLE_COLLISION_RESULT IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset ) = 0;

	// Only Client
public:
	virtual float GetMinusX() const		{ return 0.f; }
	virtual float GetMinusZ() const		{ return 0.f; }
	virtual float GetPlusX() const		{ return 0.f; }
	virtual float GetPlusZ() const		{ return 0.f; }
	virtual float GetRotationY() const	{ return 0.f; }
	virtual float GetRadius() const		{ return 0.f; }

	virtual BOOL IsMinusX() const		{ return FALSE; }
	virtual BOOL IsMinusZ() const		{ return FALSE; }
	virtual BOOL IsPlusX() const		{ return FALSE; }
	virtual BOOL IsPlusZ() const		{ return FALSE; }
	virtual BOOL IsRotationY() const	{ return FALSE; }
	virtual BOOL IsRadius() const		{ return FALSE; }

	virtual void SetMinusX( float fValue )			{}
	virtual void SetMinusZ( float fValue )			{}
	virtual void SetPlusX( float fValue )			{}
	virtual void SetPlusZ( float fValue )			{}
	virtual void SetRotationY( float fValue )		{}
	virtual void SetRadius( float fValue )			{}

	virtual void Render() const						{}		// Client ������ ����Ǿ� �Ѵ�. Server ������ ȣ��ǵ� �ƹ��� ������ �������.
	virtual void RenderCollisionName() const		{}		// Client ������ ����Ǿ� �Ѵ�. Server ������ ȣ��ǵ� �ƹ��� ������ �������.

public:
	virtual void Save( sc::SerialFile &SFile );
	virtual void Load( sc::BaseStream &SFile );

public:
	friend class CLevelCollision;

public:
	SimpleCollisionBase( SIMPLE_COLLISION_TYPE emType, const char* pName );
	~SimpleCollisionBase();
};



//////////////////////////////////////////////////////////////////////////
//						SimpleCollisionPlane
//						Y �� ���Ѵ�� ���� �ִ� Plane
//////////////////////////////////////////////////////////////////////////
class SimpleCollisionPlane : public SimpleCollisionBase
{
protected:
	enum
	{
		VERSION = 0x100,
	};

protected:
	D3DXVECTOR2 m_vMinMaxX;
	float		m_fRotationY;

	// Dont Save, Load
private:
	Line2D m_sLine;
	//D3DXPLANE	m_sPlane;
	//D3DXVECTOR2 m_vLinePos[2];
	//D3DXMATRIX	m_matInverseRotationY;

private:
	void InitData();

public:
	SIMPLE_COLLISION_RESULT IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset ) override;

public:
	void SetObjRotate( float fAddAngle ) override;

	void SetPosisionRotation( const D3DXVECTOR3& vPosition, float fRotationY ) override;

	float GetMinusX() const override;
	float GetPlusX() const override;
	float GetRotationY() const override;

	BOOL IsMinusX() const override		{ return TRUE; }
	BOOL IsPlusX() const override		{ return TRUE; }
	BOOL IsRotationY() const override	{ return TRUE; }

	void SetMinusX( float fValue ) override			{ m_vMinMaxX.x = fValue; }
	void SetPlusX( float fValue ) override			{ m_vMinMaxX.y = fValue; }
	void SetRotationY( float fValue ) override		{ m_fRotationY = fValue; }

public:
	void Save( sc::SerialFile &SFile ) override;
	void Load( sc::BaseStream &SFile ) override;

public:
	SimpleCollisionPlane();
	~SimpleCollisionPlane();
};



//////////////////////////////////////////////////////////////////////////
//						SimpleCollisionBox
//						Y�� ���ܵ� 4���� �ڽ�
//////////////////////////////////////////////////////////////////////////
class SimpleCollisionBox : public SimpleCollisionBase
{
protected:
	enum
	{
		VERSION = 0x100,
	};

protected:
	D3DXVECTOR2 m_vMinMaxX;
	D3DXVECTOR2 m_vMinMaxZ;
	float		m_fRotationY;

	// Dont Save, Load
private:
	D3DXPLANE	m_sPlane[4];
	D3DXMATRIX	m_matInverseRotationY;

private:
	void InitData();

public:
	SIMPLE_COLLISION_RESULT IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset ) override;

public:
	void SetObjRotate( float fAddAngle ) override;

	void SetPosisionRotation( const D3DXVECTOR3& vPosition, float fRotationY ) override;

	float GetMinusX() const override;
	float GetMinusZ() const override;
	float GetPlusX() const override;
	float GetPlusZ() const override;
	float GetRotationY() const override;

	BOOL IsMinusX() const override		{ return TRUE; }
	BOOL IsMinusZ() const override		{ return TRUE; }
	BOOL IsPlusX() const override		{ return TRUE; }
	BOOL IsPlusZ() const override		{ return TRUE; }
	BOOL IsRotationY() const override	{ return TRUE; }

	void SetMinusX( float fValue ) override			{ m_vMinMaxX.x = fValue; }
	void SetMinusZ( float fValue ) override			{ m_vMinMaxZ.x = fValue; }
	void SetPlusX( float fValue ) override			{ m_vMinMaxX.y = fValue; }
	void SetPlusZ( float fValue ) override			{ m_vMinMaxZ.y = fValue; }
	void SetRotationY( float fValue ) override		{ m_fRotationY = fValue; }

public:
	void Save( sc::SerialFile &SFile ) override;
	void Load( sc::BaseStream &SFile ) override;

public:
	SimpleCollisionBox();
	~SimpleCollisionBox();
};



//////////////////////////////////////////////////////////////////////////
//						SimpleCollisionCylinder
//						Y �� ���Ѵ�� ���� �ִ� Cylinder
//////////////////////////////////////////////////////////////////////////
class SimpleCollisionCylinder : public SimpleCollisionBase
{
protected:
	enum
	{
		VERSION = 0x100,
	};

protected:
	float		m_fRadius;

private:
	void InitData();

public:
	SIMPLE_COLLISION_RESULT IsCollision( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, float fOffset ) override;

public:
	float GetRadius() const override;

	BOOL IsRadius() const override			{ return TRUE; }

	void SetRadius( float fValue ) override			 { m_fRadius = fValue; }

public:
	void Save( sc::SerialFile &SFile ) override;
	void Load( sc::BaseStream &SFile ) override;

public:
	SimpleCollisionCylinder();
	~SimpleCollisionCylinder();
};