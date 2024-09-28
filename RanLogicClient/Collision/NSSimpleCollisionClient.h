#pragma once

class LayerCollision;


namespace NSSimpleCollisionClient
{
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const std::vector<boost::shared_ptr<LayerCollision>>& vecSrc );
	void RenderCollisionName( LPDIRECT3DDEVICEQ pd3dDevice, const std::vector<boost::shared_ptr<LayerCollision>>& vecSrc, size_t unLayer );
};