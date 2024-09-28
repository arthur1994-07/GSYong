#pragma once

namespace NSLandThread
{
	enum EM_FIELD_CULLING_RENDERING_TYPE
	{
		//TYPE_BASEMESH_CULL,				// �׸��� Surface �� �׸��� ���� Mesh Culling ���� ( �⺻, CSM #1 )// BackBuffer �� �׸��� ���� Mesh ����
		//TYPE_SHADOW_FIRST_CULL,			// �׸��� Surface �� �׸��� ���� Mesh Culling ���� ( �⺻, CSM #1 )
		//TYPE_SHADOW_SECOND_CULL,		// �׸��� Surface �� �׸��� ���� Mesh Culling ���� ( �⺻, CSM #2 )

		TYPE_SHADOW_FIRST_CULL			= 0,	// �׸��� Surface �� �׸��� ���� Mesh Culling ���� ( �⺻, CSM #1 )
		TYPE_SHADOW_SECOND_CULL			= 1,	// �׸��� Surface �� �׸��� ���� Mesh Culling ���� ( �⺻, CSM #2 )
		TYPE_INSTANCING_PURE_MESH_SIZE	= 2,	// [ ���� ] ���θ� ���� �͵� ���� (0 ~ 2) �ν��Ͻ̸�

		TYPE_BASEMESH_CULL				= 2,	// BackBuffer �� �׸��� ���� Mesh ����	[ ����� LM ���� ]
		TYPE_REFLECTION_CULL			= 3,	// �ݻ�Surface �� �׸��� ���� Mesh Culling ����
		TYPE_INSTANCING_ALL_SIZE		= 4,	// �ν��Ͻ̸� ���� ���� (0 ~ 4)

		TYPE_SPOTSHADOW_FIRST_CULL		= 4, 	// SpotLight #1 Surface �� �׸��� ���� Mesh Culling ���� [�ν��Ͻ����� �ʴ´�]
		TYPE_SPOTSHADOW_SECOND_CULL		= 5,	// SpotLight #2 Surface �� �׸��� ���� Mesh Culling ���� [�ν��Ͻ����� �ʴ´�]
		TYPE_CULL_MODE_SIZE				= 6,	// ���� 6��

		TYPE_NO_FIELDMESH				= 7,	// FieldMesh �� �ƴϴ�. Piece �� �̾߱��.	
											// DxSingleMaterialMesh ������ ����, Piece �� ���� ���� CV �� ���� �ʾƼ� ������ Field �� �ٸ���.
	};
};