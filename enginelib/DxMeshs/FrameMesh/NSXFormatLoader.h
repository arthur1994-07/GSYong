#pragma once

// MeshLoader, XParser ������ ���� ������.
// svn - http://wwrzaldxlearning.googlecode.com/svn/trunk ����
// ����� X-File Loader ����� ������ ����.
// �ܼ��� Material Name�� ������ ���� ��ɸ� ��.

#include "../../DxTools/DxVertexFVF.h"

namespace NSXFormatLoader
{
	BOOL ParseChildObjects_MaterialName( LPD3DXFILEDATA pDataObj, VEC_TSTRING& vecMaterialName );
}