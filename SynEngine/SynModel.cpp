#include "synmodel.h"

SynModel::SynModel()
{
	SAFE_INIT(m_vertexBuffer);
	SAFE_INIT(m_indexBuffer);
	SAFE_INIT(m_Texture);
	SAFE_INIT(m_model);
	SAFE_INIT(m_indices);
}

SynModel::SynModel(const SynModel& other)
{

}

SynModel::~SynModel()
{

}

bool SynModel::Initialize(ID3D11Device* device, WCHAR* textureFilename, char* modelFilename, int meshIndex)
{
	bool result;

	// Load in the model data,
	//result = LoadModelAssimp(modelFilename, meshIndex);
	//SAFE_CHECKEXIST(result);

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device, modelFilename, meshIndex);
	SAFE_CHECKEXIST(result);

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	SAFE_CHECKEXIST(result);

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

bool SynModel::InitializeBuffers(ID3D11Device* device, char* filename, int meshIndex)
{
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	const aiScene* importedModel = aiImportFile(filename, aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded);
	SAFE_CHECKEXIST(importedModel);

	m_vertexCount = importedModel->mMeshes[meshIndex]->mNumVertices;

	std::vector<unsigned long> indicesVector;
	for (int i = 0; i < importedModel->mMeshes[meshIndex]->mNumFaces; i++) {
		const aiFace& Face = importedModel->mMeshes[meshIndex]->mFaces[i];
		if (Face.mNumIndices == 3) {
			indicesVector.push_back(Face.mIndices[0]);
			indicesVector.push_back(Face.mIndices[1]);
			indicesVector.push_back(Face.mIndices[2]);
		}
	}
	m_indexCount = indicesVector.size();

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	const aiMesh *mesh = importedModel->mMeshes[meshIndex];

	// Load the vertex array and index array with data.
	for(int i=0; i<m_vertexCount; i++)
	{
		vertices[i].position = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		if (importedModel->mMeshes[meshIndex]->HasTextureCoords(0)){
			vertices[i].texture = DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		vertices[i].normal = DirectX::XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
	}

	aiReleaseImport(importedModel);

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
	indexData.pSysMem = &indicesVector[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(m_indices);
    indicesVector.clear();
	return true;
}

void SynModel::ShutdownBuffers()
{
	// Release the index buffer.
	SAFE_RELEASE(m_indexBuffer);

	// Release the vertex buffer.
	SAFE_RELEASE(m_vertexBuffer);

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
	SAFE_CHECKEXIST(m_Texture);
	
	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	SAFE_CHECKEXIST(result);

	return true;
}

void SynModel::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		SAFE_DELETE(m_Texture);
	}
	return;
}

bool SynModel::LoadModelAssimp(char* filename, int meshIndex)
{
    // functionality moved to InitializeBuffers function, cause of lack of optimization
	return true;
}

void SynModel::ReleaseModel()
{
	SAFE_DELETE_ARRAY(m_model);
	return;
}