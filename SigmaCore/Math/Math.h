
#pragma once

namespace scmath
{
	static float RoundingTo( float fValue, int nDigit )
	{
		return floor( (fValue) * pow(10.0f,nDigit) + 0.5f )/pow(10.0f,nDigit);
	}

	// 특정 축을 기준으로 벡터 회전
	static D3DXVECTOR3 RotateVectorFromAxis( const D3DXVECTOR3& v, float fDegree, const D3DXVECTOR3& vAxis )
	{
		const float fRadian = fDegree * D3DX_PI / 180.0f;

		const float fSin = sinf( fRadian );
		const float fCos = cosf( fRadian );

		const float fXX = vAxis.x * vAxis.x;
		const float fYY = vAxis.y * vAxis.y;
		const float fZZ = vAxis.z * vAxis.z;

		const float fXY = vAxis.x * vAxis.y;
		const float fYZ = vAxis.y * vAxis.z;
		const float fZX = vAxis.z * vAxis.x;

		const float fXSin = vAxis.x * fSin;
		const float fYSin = vAxis.y * fSin;
		const float fZSin = vAxis.z * fSin;

		const float fCosInv = 1.0f - fCos;

		return D3DXVECTOR3( 
			(fCosInv * fXX + fCos ) * v.x + (fCosInv * fXY - fZSin) * v.y + (fCosInv * fZX + fYSin) * v.z,
			(fCosInv * fXY + fZSin) * v.x + (fCosInv * fYY + fCos) * v.y + (fCosInv * fYZ - fXSin) * v.z,
			(fCosInv * fZX - fYSin) * v.x + (fCosInv * fYZ + fXSin) * v.y + (fCosInv * fZZ + fCos) * v.z );
	}
}