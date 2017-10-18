#include "synmodel.h"
#include <assimp\cimport.h>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <vector>

SynModel::SynModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
	m_indices = 0;
}

SynModel::SynModel(const SynModel& other)
{

}

SynModel::~SynModel()
{

}

bool SynModel::Initialize(ID3D11Device* device, WCHAR* textureFilename, char* modelFilename)
{
	bool result;

	// Load in the model data,
	result = LoadModelAssimp(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void SynModel::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}

void SynModel::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int SynModel::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* SynModel::GetTexture()
{
	return m_Texture->GetTexture();
}

bool SynModel::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for(int i=0; i<m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
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
	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] m_indices;
	m_indices = 0;
	

	return true;
}

void SynModel::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void SynModel::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool SynModel::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	m_Texture = new SynTexture;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void SynModel::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

bool SynModel::LoadModelAssimp(char* filename)
{
	const aiScene* importedModel = aiImportFile(filename, aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded);
	if (!importedModel)
	{
		return false;
	}

	m_vertexCount = importedModel->mMeshes[0]->mNumVertices;

	std::vector<unsigned long> indicesVector;
	for (int i = 0; i < importedModel->mMeshes[0]->mNumFaces; i++) {
		const aiFace& Face = importedModel->mMeshes[0]->mFaces[i];
		if (Face.mNumIndices == 3) {
			indicesVector.push_back(Face.mIndices[0]);
			indicesVector.push_back(Face.mIndices[1]);
			indicesVector.push_back(Face.mIndices[2]);
		}
	}
	m_indexCount = indicesVector.size();

	// Create the index array.
	m_indices = new unsigned long[indicesVector.size()];
	if (!m_indices)
	{
		return false;
	}

	for (int i = 0; i < indicesVector.size(); i++)
	{
		m_indices[i] = indicesVector[i];
	}

	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	for (int i = 0; i<m_vertexCount; i++)
	{
		m_model[i].x = importedModel->mMeshes[0]->mVertices[i].x;
		m_model[i].y = importedModel->mMeshes[0]->mVertices[i].y;
		m_model[i].z = importedModel->mMeshes[0]->mVertices[i].z;

		m_model[i].tu = 1;
		m_model[i].tv = 1;

		m_model[i].nx = importedModel->mMeshes[0]->mNormals[i].x;
		m_model[i].ny = importedModel->mMeshes[0]->mNormals[i].y;
		m_model[i].nz = importedModel->mMeshes[0]->mNormals[i].z;
	}

	aiReleaseImport(importedModel);

	return true;
}

bool SynModel::LoadModel(char* filename)
{
	return true;
}

void SynModel::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}