#pragma once
#include "common.h"
#include <d3d11.h>
#include "syntexture.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

class Lab1Model
{
private:
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	Lab1Model();
	~Lab1Model();

	bool Initialize(ID3D11Device*, char*);
	bool InitializeBezier(ID3D11Device*, DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3);

	void Shutdown();
	bool Render(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* GetTexture();

	int GetIndexCount();

private:

	bool InitializeBuffers(ID3D11Device*, char*);
	bool InitializeBuffersBezier(ID3D11Device*, DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3);

	void ShutdownBuffers();
	void CalculateBezier(std::vector<VertexType> &, std::vector<unsigned long> &, DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT3);

	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	SynTexture* m_Texture;
	int m_vertexCount, m_indexCount;
};