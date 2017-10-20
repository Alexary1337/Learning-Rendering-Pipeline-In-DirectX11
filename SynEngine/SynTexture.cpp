#include "syntexture.h"

SynTexture::SynTexture()
{
	SAFE_INIT(m_texture);
}

SynTexture::SynTexture(const SynTexture& other)
{

}

SynTexture::~SynTexture()
{

}

bool SynTexture::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFileW(device, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SynTexture::Shutdown()
{
	// Release the texture resource.
	SAFE_RELEASE(m_texture);

	return;
}

ID3D11ShaderResourceView* SynTexture::GetTexture()
{
	return m_texture;
}