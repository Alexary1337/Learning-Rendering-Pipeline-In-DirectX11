#ifndef _MODEL_H_
#define _MODEL_H_

#include "common.h"
#include <d3d11.h>
#include <d3dx10math.h>
#include "syntexture.h"
#include <assimp\cimport.h>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <vector>

class SynModel
{

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	SynModel();
	SynModel(const SynModel&);
	~SynModel();
	bool Initialize(ID3D11Device*, WCHAR*, char*, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*, char*, int);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();
	
	bool LoadModelAssimp(char*, int);

	void ReleaseModel();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	SynTexture* m_Texture;

	ModelType* m_model;
	unsigned long* m_indices;
};

#endif