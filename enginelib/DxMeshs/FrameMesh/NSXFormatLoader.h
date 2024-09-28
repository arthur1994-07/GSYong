#pragma once

// MeshLoader, XParser 파일을 보고 참조함.
// svn - http://wwrzaldxlearning.googlecode.com/svn/trunk 참조
// 현재는 X-File Loader 기능을 하지는 않음.
// 단순히 Material Name을 얻어오기 위한 기능만 함.

#include "../../DxTools/DxVertexFVF.h"

namespace NSXFormatLoader
{
	BOOL ParseChildObjects_MaterialName( LPD3DXFILEDATA pDataObj, VEC_TSTRING& vecMaterialName );
}