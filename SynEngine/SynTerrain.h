#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "common.h"
#include <d3d11.h>
#include <d3dx10math.h>
#include "syntexture.h"
#include <stdio.h>

class SynTerrain
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	SynTerrain();
	SynTerrain(const SynTerrain&);
	~SynTerrain();

	bool Initialize(ID3D11Device*, WCHAR*, char*);
	bool InitializeByFunction(ID3D11Device*, WCHAR*, float, float, float, float(*f)(float, float));

	void Shutdown();
	bool Render(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* GetTexture();

	int GetIndexCount();

private:
	bool LoadMapByFunction(float, float, float, float(*f)(float, float));
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();


	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	SynTexture* m_Texture;
	int m_vertexCount, m_indexCount;
	int m_terrainWidth, m_terrainHeight;

	HeightMapType* m_heightMap;
};

#endif