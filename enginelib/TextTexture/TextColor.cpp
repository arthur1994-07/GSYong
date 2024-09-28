#include "pch.h"
#include "./TextColor.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

COLOR_32 Conv16to32( COLOR_16 src )
{
	return COLOR_32( src.GetA() * 17, src.GetR() * 17, src.GetG() * 17, src.GetB() * 17 );
}

COLOR_16 Conv32to16( COLOR_32 src )
{
	return COLOR_16( src.GetA() / 17, src.GetR() / 17, src.GetG() / 17, src.GetB() / 17 );
}