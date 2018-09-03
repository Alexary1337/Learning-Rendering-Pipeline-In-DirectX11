#ifndef _TEXT_H_
#define _TEXT_H_

#include "common.h"
#include "synfont.h"
#include "synfontshader.h"

class SynText
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 texture;
	};

public:
	SynText();
	SynText(const SynText&);
	~SynText();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, DirectX::XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX);

	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, DirectX::XMMATRIX, DirectX::XMMATRIX);

private:
	SynFont* m_Font;
	SynFontShader* m_FontShader;
	int m_screenWidth, m_screenHeight;
	DirectX::XMMATRIX m_baseViewMatrix;

	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
	SentenceType* m_sentence3;
};

#endif