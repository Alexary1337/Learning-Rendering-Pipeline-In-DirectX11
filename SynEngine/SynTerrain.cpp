#include "synterrain.h"

SynTerrain::SynTerrain()
{
	SAFE_INIT(m_vertexBuffer);
	SAFE_INIT(m_indexBuffer);
}

SynTerrain::SynTerrain(const SynTerrain& other)
{

}

SynTerrain::~SynTerrain()
{

}

bool SynTerrain::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	// Load the rendering buffers with the terrain data.
	result = InitializeBuffers(device);
	SAFE_CHECKEXIST(result);

	// Load the texture for terrain.
	result = LoadTexture(device, textureFilename);
	SAFE_CHECKEXIST(result);

	return true;
}

void SynTerrain::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the rendering buffers.
	ShutdownBuffers();

	return;
}

bool SynTerrain::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

ID3D11ShaderResourceView* SynTerrain::GetTexture()
{
	return m_Texture->GetTexture();
}

int SynTerrain::GetIndexCount()
{
	return m_indexCount;
}

bool SynTerrain::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i, j, terrainWidth, terrainHeight, index;

	D3DXVECTOR2 texture = D3DXVECTOR2(1.0f, 1.0f);
	D3DXVECTOR3 normal = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	float positionX, positionZ;

	// Set the height and width of the terrain grid.
	terrainHeight = 256;
	terrainWidth = 256;


	// Calculate the number of vertices in the terrain.
	m_vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 8;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	SAFE_CHECKEXIST(vertices);

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	SAFE_CHECKEXIST(indices);

	// Initialize the index into the vertex and index arrays.
	index = 0;

	// Load the vertex array and index array with data.
	for (j = 0; j<(terrainHeight - 1); j++)
	{
		for (i = 0; i<(terrainWidth - 1); i++)
		{
			// Line 1 - Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = D3DXVECTOR3(positionX, 0.0f, positionZ);
			vertices[index].texture = texture;
			vertices[index].normal = normal;
			indices[index] = index;
			index++;

			// Line 1 - Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = D3DXVECTOR3(positionX, 0.0f, positionZ);
			vertices[index].texture = texture;
			vertices[index].normal = normal;
			indices[index] = index;
			index++;

			// Line 2 - Upper right
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = D3DXVECTOR3(positionX, 0.0f, positionZ);
			vertices[index].texture = texture;
			vertices[index].normal = normal;
			indices[index] = index;
			index++;

			// Line 2 - Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = D3DXVECTOR3(positionX, 0.0f, positionZ);
			vertices[index].texture = texture;
			vertices[index].normal = normal;
			indices[index] = index;
			index++;

			// Line 3 - Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = D3DXVECTOR3(positionX, 0.0f, positionZ);
			vertices[index].texture = texture;
			vertices[index].normal = normal;
			indices[index] = index;
			index++;

			// Line 3 - Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = D3DXVECTOR3(positionX, 0.0f, positionZ);
			vertices[index].texture = texture;
			vertices[index].normal = normal;
			indices[index] = index;
			index++;

			// Line 4 - Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = D3DXVECTOR3(positionX, 0.0f, positionZ);
			vertices[index].texture = texture;
			vertices[index].normal = normal;
			indices[index] = index;
			index++;

			// Line 4 - Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = D3DXVECTOR3(positionX, 0.0f, positionZ);
			vertices[index].texture = texture;
			vertices[index].normal = normal;
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	return true;
}

void SynTerrain::ShutdownBuffers()
{
	// Release the index buffer.
	SAFE_RELEASE(m_indexBuffer);

	// Release the vertex buffer.
	SAFE_RELEASE(m_vertexBuffer);

	return;
}

void SynTerrain::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case lines.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}

bool SynTerrain::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	m_Texture = new SynTexture;
	SAFE_CHECKEXIST(m_Texture);

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	SAFE_CHECKEXIST(result);

	return true;
}

void SynTerrain::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		SAFE_DELETE(m_Texture);
	}
	return;
}
