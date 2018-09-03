#ifndef _SKYDOME_H_
#define _SKYDOME_H_

#include "common.h"
#include <d3d11.h>
#include <assimp\cimport.h>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <vector>

class SynSkyDome
{
private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
	};

public:
	SynSkyDome();
	SynSkyDome(const SynSkyDome&);
	~SynSkyDome();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	DirectX::XMFLOAT4 GetApexColor();
	DirectX::XMFLOAT4 GetCenterColor();

private:
	bool LoadSkyDomeModel(char*);
	void ReleaseSkyDomeModel();

	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ModelType* m_model;
	int m_vertexCount, m_indexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	DirectX::XMFLOAT4 m_apexColor, m_centerColor;
	unsigned long* m_indices;
};

#endif
