#include "pch.h"

#include "Entity.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

Entity::Entity()
	: m_fAlpha(1.f)
{

}

Entity::~Entity()
{

}

void Entity::SetAlpha( float fAlpha )
{
	m_fAlpha = fAlpha;
}

float Entity::GetAlpha() const
{
	return m_fAlpha;
}