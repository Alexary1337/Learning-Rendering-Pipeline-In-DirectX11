#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "common.h"
#include <d3d11.h>
#include <d3dx11tex.h>

class SynTexture
{
public:
	SynTexture();
	SynTexture(const SynTexture&);
	~SynTexture();
	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};

#endif