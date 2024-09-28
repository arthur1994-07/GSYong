#pragma once

namespace SPLINE
{
	class RNS
	{
	public:
	  void Init(){ nodeCount = 0; }
	  void AddNode(const D3DXVECTOR3 &pos);
	  void BuildSpline();
	  D3DXVECTOR3 GetPosition(float time);
	  int GetIndex(float time);

	  struct splineData
	  {
		D3DXVECTOR3 position;
		D3DXVECTOR3 velocity;
		float distance;
	  } node[100];
	  float maxDistance;
	  int nodeCount;
	protected:
	  D3DXVECTOR3 GetStartVelocity(int index);
	  D3DXVECTOR3 GetEndVelocity(int index);
	};

	class SNS : public RNS
	{
	public:
	  void BuildSpline(){ RNS::BuildSpline(); Smooth(); Smooth(); Smooth(); }
	  void Smooth();
	};

	class TNS : public SNS
	{
	public:
	  void AddNode(const D3DXVECTOR3 &pos, float timePeriod);
	  void BuildSpline()	{ RNS::BuildSpline(); Smooth(); Smooth(); Smooth(); }
	  void Smooth(){ SNS::Smooth(); Constrain(); }
	  void Constrain(); 
	};
}