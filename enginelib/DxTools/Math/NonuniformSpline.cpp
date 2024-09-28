#include "pch.h"

#include "NonuniformSpline.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace SPLINE
{
	D3DXMATRIX g_matHermite( 2.f,-2.f, 1.f, 1.f,
						   -3.f, 3.f,-2.f,-1.f,
							0.f, 0.f, 1.f, 0.f,
							1.f, 0.f, 0.f, 0.f );

	//D3DXMATRIX g_matHermite( 2.f, -3.f, 0.f, 1.f,
	//						-2.f, 3.f, 0.f, 0.f,
	//						1.f, -2.f, 1.f, 0.f,
	//						1.f, -1.f, 0.f, 0.f );
	//
	//D3DXMATRIX g_matHermite( 1.f, 0.f, 0.f, 0.f,
	//						0.f, 1.f, 0.f, 0.f,
	//						0.f, 0.f, 1.f, 0.f,
	//						0.f, 0.f, 0.f, 1.f );

	//= // D3DXMATRIX H in article
	//{ 
	//  {
	//   { 2.f,-2.f, 1.f, 1.f},
	//   {-3.f, 3.f,-2.f,-1.f},
	//   { 0.f, 0.f, 1.f, 0.f},
	//   { 1.f, 0.f, 0.f, 0.f}
	//  }
	//};

	// cubic curve defined by 2 positions and 2 velocities
	D3DXVECTOR3 GetPositionOnCubic(const D3DXVECTOR3 &startPos, const D3DXVECTOR3 &startVel, const D3DXVECTOR3 &endPos, const D3DXVECTOR3 &endVel, float time)
	{
	  //m.SetRow0(startPos); 
	  //m.SetRow1(endPos);
	  //m.SetRow2(startVel); 
	  //m.SetRow3(endVel);
	  //m.Multiply4x4(&g_matHermite, &m); // multiply by the mixer

	  //D3DXVECTOR3 timeVector = {time*time*time, time*time, time, 1.f};
	  //return ApplyMatrix(timeVector, &m); // vector * matrix

		D3DXMATRIX m;
		m._11 = startPos.x;
		m._12 = startPos.y;
		m._13 = startPos.z;
		m._14 = 1.f;
		m._21 = endPos.x;
		m._22 = endPos.y;
		m._23 = endPos.z;
		m._24 = 1.f;
		m._31 = startVel.x;
		m._32 = startVel.y;
		m._33 = startVel.z;
		m._34 = 0.f;
		m._41 = endVel.x;
		m._42 = endVel.y;
		m._43 = endVel.z;
		m._44 = 0.f;
		D3DXMatrixMultiply( &m, &g_matHermite, &m );

		D3DXVECTOR3 timeVector(time*time*time, time*time, time );
		D3DXVec3TransformCoord( &timeVector, &timeVector, &m );

		D3DXVECTOR3 vPos( timeVector.x, timeVector.y, timeVector.z );
		return vPos;
	}

	/*********************************** R N S **************************************************/

	// adds node and updates segment length
	void RNS::AddNode(const D3DXVECTOR3 &pos)
	{
	  if (nodeCount == 0)
		maxDistance = 0.f;
	  else
	  {
		//node[nodeCount-1].distance = (node[nodeCount-1].position - pos).Magnitude();
		//maxDistance += node[nodeCount-1].distance;
		D3DXVECTOR3 vDir = node[nodeCount-1].position - pos;
		node[nodeCount-1].distance = D3DXVec3Length( &vDir );
		maxDistance += node[nodeCount-1].distance;
	  }
	  node[nodeCount].velocity = D3DXVECTOR3(0.f,0.f,0.f);	// 초기화함. 2개점만 만들면 문제가 생겨서 수정함.
	  node[nodeCount++].position = pos;
	}

	// called after all nodes added. This function calculates the node velocities
	void RNS::BuildSpline()
	{
	  for (int i = 1; i<nodeCount-1; i++)
	  {
		//// split the angle (figure 4)
		//node[i].velocity = Normalise(node[i+1].position - node[i].position) - 
		//                   Normalise(node[i-1].position - node[i].position);
		//node[i].velocity.Normalise();
		  D3DXVECTOR3 vDir1 = node[i+1].position - node[i].position;
		  float fLength = D3DXVec3Length( &vDir1 );
		  if( fLength > 0.f )
		  {
			  D3DXVec3Normalize( &vDir1, &vDir1 );
		  }
		  else
		  {
			  vDir1 = D3DXVECTOR3(0.f,0.f,0.f);
		  }

		  D3DXVECTOR3 vDir2 = node[i-1].position - node[i].position;
		  fLength = D3DXVec3Length( &vDir2 );
		  if( fLength > 0.f )
		  {
			  D3DXVec3Normalize( &vDir2, &vDir2 );
		  }
		  else
		  {
			  vDir2 = D3DXVECTOR3(0.f,0.f,0.f);
		  }
		  
		  node[i].velocity = vDir1 - vDir2;
		  fLength = D3DXVec3Length( &node[i].velocity );
		  if( fLength > 0.f )
		  {
			  D3DXVec3Normalize( &node[i].velocity, &node[i].velocity );
		  }
		  else
		  {
			  node[i].velocity = D3DXVECTOR3(0.f,0.f,0.f);
		  }
	  }

	  // calculate start and end velocities
	  node[0].velocity = GetStartVelocity(0);
	  node[nodeCount-1].velocity = GetEndVelocity(nodeCount-1);

	  {
		  float fLength = D3DXVec3Length( &node[0].velocity );
		  if( fLength > 0.f )
		  {
			  D3DXVec3Normalize( &node[0].velocity, &node[0].velocity );
		  }

		  fLength = D3DXVec3Length( &node[nodeCount-1].velocity );
		  if( fLength > 0.f )
		  {
			  D3DXVec3Normalize( &node[nodeCount-1].velocity, &node[nodeCount-1].velocity );
		  }
	  }
	}

	// spline access function. time is 0 -> 1
	D3DXVECTOR3 RNS::GetPosition(float time)
	{
	  float distance = time * maxDistance;
	  float currentDistance = 0.f;
	  int i = 0;
	  while (currentDistance + node[i].distance < distance
		&& i < 100)
	  {
		currentDistance += node[i].distance;
		i++;
	  }
	  float t = distance - currentDistance;
	  t /= node[i].distance; // scale t in range 0 - 1
	  D3DXVECTOR3 startVel = node[i].velocity * node[i].distance;
	  D3DXVECTOR3 endVel = node[i+1].velocity * node[i].distance;   
	  return GetPositionOnCubic(node[i].position, startVel,
							 node[i+1].position, endVel, t);
	}

	// spline access function. time is 0 -> 1
	int RNS::GetIndex(float time)
	{
	  float distance = time * maxDistance;
	  float currentDistance = 0.f;
	  int i = 0;
	  while (currentDistance + node[i].distance < distance
		&& i < 100)
	  {
		currentDistance += node[i].distance;
		i++;
	  }

	  return i;
	}

	// internal. Based on Equation 14 
	D3DXVECTOR3 RNS::GetStartVelocity(int index)
	{
		if( node[index].distance > 0.f )
		{
			D3DXVECTOR3 temp = 3.f*(node[index+1].position - node[index].position)/node[index].distance;
			return (temp - node[index+1].velocity)*0.5f;
		}
		else
		{
			D3DXVECTOR3 vPos(0.f,0.f,0.f);
			return vPos;
		}
	}

	// internal. Based on Equation 15 
	D3DXVECTOR3 RNS::GetEndVelocity(int index)
	{
		if( node[index-1].distance > 0.f )
		{
			D3DXVECTOR3 temp = 3.f*(node[index].position - node[index-1].position)/node[index-1].distance;
			return (temp - node[index-1].velocity)*0.5f;
		}
		else
		{
			D3DXVECTOR3 vPos(0.f,0.f,0.f);
			return vPos;
		}
	}

	/*********************************** S N S **************************************************/

	// smoothing filter.
	void SNS::Smooth()
	{
	  D3DXVECTOR3 newVel;
	  D3DXVECTOR3 oldVel = GetStartVelocity(0);
	  for (int i = 1; i<nodeCount-1; i++)
	  {
		// Equation 12
		newVel = GetEndVelocity(i)*node[i].distance +
				 GetStartVelocity(i)*node[i-1].distance;
		newVel /= (node[i-1].distance + node[i].distance);
		node[i-1].velocity = oldVel;
		oldVel = newVel;
	  }
	  node[nodeCount-1].velocity = GetEndVelocity(nodeCount-1);
	  node[nodeCount-2].velocity = oldVel;
	}

	/*********************************** T N S **************************************************/

	// as with RNS but use timePeriod in place of actual node spacing
	// ie time period is time from last node to this node
	void TNS::AddNode(const D3DXVECTOR3 &pos, float timePeriod)
	{
	  if (nodeCount == 0)
		maxDistance = 0.f;
	  else
	  {
		node[nodeCount-1].distance = timePeriod;
		maxDistance += node[nodeCount-1].distance;
	  }
	  node[nodeCount].velocity = D3DXVECTOR3(0.f,0.f,0.f);	// 초기화함. 2개점만 만들면 문제가 생겨서 수정함.
	  node[nodeCount++].position = pos;
	}

	// stabilised version of TNS
	void TNS::Constrain()
	{
	  for (int i = 1; i<nodeCount-1; i++)
	  {
		//// Equation 13
		//float r0 = (node[i].position-node[i-1].position).Magnitude()/node[i-1].distance;
		//float r1 = (node[i+1].position-node[i].position).Magnitude()/node[i].distance;
		//node[i].velocity *= 4.f*r0*r1/((r0+r1)*(r0+r1));

		  //D3DXVECTOR3 vDir1 = node[i].position-node[i-1].position;
		  //D3DXVECTOR3 vDir2 = node[i+1].position-node[i].position;
		  //float fLength1 = D3DXVec3Length( &vDir1 );
		  //float fLength2 = D3DXVec3Length( &vDir2 );
		  //float r0 = fLength1/node[i-1].distance;
	   //   float r1 = fLength2/node[i].distance;
		  //node[i].velocity *= 4.f*r0*r1/((r0+r1)*(r0+r1));

		  D3DXVECTOR3 vDir1 = node[i].position-node[i-1].position;
		  D3DXVECTOR3 vDir2 = node[i+1].position-node[i].position;
		  float fLength1 = D3DXVec3Length( &vDir1 );
		  float fLength2 = D3DXVec3Length( &vDir2 );
		  float r0 = fLength1/node[i-1].distance;
		  if( node[i-1].distance <= 0.f )	r0 = 0.f;
		  float r1 = fLength2/node[i].distance;
		  if( node[i].distance <= 0.f )		r1 = 0.f;

		  if( (r0+r1) > 0.f )
		  {
			node[i].velocity *= 4.f*r0*r1/((r0+r1)*(r0+r1));
		  }
		  else
		  {
			 node[i].velocity = D3DXVECTOR3( 0.f, 0.f, 0.f );
		  }
	  }
	}
}