#ifndef _FONT_H_
#define _FONT_H_

#include "common.h"
#include <d3d11.h>
#include <fstream>
using namespace std;
#include "syntexture.h"

class SynFont
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	SynFont();
	SynFont(const SynFont&);
	~SynFont();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	SynTexture* m_Texture;
};

#endif