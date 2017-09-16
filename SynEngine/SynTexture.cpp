#include "syntexture.h"

SynTexture::SynTexture()
{
	m_texture = 0;
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
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* SynTexture::GetTexture()
{
	return m_texture;
}