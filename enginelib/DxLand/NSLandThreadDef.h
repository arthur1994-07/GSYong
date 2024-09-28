#pragma once

namespace NSLandThread
{
	enum EM_FIELD_CULLING_RENDERING_TYPE
	{
		//TYPE_BASEMESH_CULL,				// 그림자 Surface 에 그리기 위한 Mesh Culling 정보 ( 기본, CSM #1 )// BackBuffer 에 그리기 위한 Mesh 정보
		//TYPE_SHADOW_FIRST_CULL,			// 그림자 Surface 에 그리기 위한 Mesh Culling 정보 ( 기본, CSM #1 )
		//TYPE_SHADOW_SECOND_CULL,		// 그림자 Surface 에 그리기 위한 Mesh Culling 정보 ( 기본, CSM #2 )

		TYPE_SHADOW_FIRST_CULL			= 0,	// 그림자 Surface 에 그리기 위한 Mesh Culling 정보 ( 기본, CSM #1 )
		TYPE_SHADOW_SECOND_CULL			= 1,	// 그림자 Surface 에 그리기 위한 Mesh Culling 정보 ( 기본, CSM #2 )
		TYPE_INSTANCING_PURE_MESH_SIZE	= 2,	// [ 형상 ] 으로만 쓰인 것들 갯수 (0 ~ 2) 인스턴싱만

		TYPE_BASEMESH_CULL				= 2,	// BackBuffer 에 그리기 위한 Mesh 정보	[ 형상과 LM 정보 ]
		TYPE_REFLECTION_CULL			= 3,	// 반사Surface 에 그리기 위한 Mesh Culling 정보
		TYPE_INSTANCING_ALL_SIZE		= 4,	// 인스턴싱만 쓰인 갯수 (0 ~ 4)

		TYPE_SPOTSHADOW_FIRST_CULL		= 4, 	// SpotLight #1 Surface 에 그리기 위한 Mesh Culling 정보 [인스턴싱하지 않는다]
		TYPE_SPOTSHADOW_SECOND_CULL		= 5,	// SpotLight #2 Surface 에 그리기 위한 Mesh Culling 정보 [인스턴싱하지 않는다]
		TYPE_CULL_MODE_SIZE				= 6,	// 현재 6개

		TYPE_NO_FIELDMESH				= 7,	// FieldMesh 가 아니다. Piece 란 이야기다.	
											// DxSingleMaterialMesh 단으로 들어가면, Piece 일 경우는 굳이 CV 를 하지 않아서 렌더가 Field 랑 다르다.
	};
};