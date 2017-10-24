#include "synskydome.h"

SynSkyDome::SynSkyDome()
{
	SAFE_INIT(m_model);
	SAFE_INIT(m_vertexBuffer);
	SAFE_INIT(m_indexBuffer);
	SAFE_INIT(m_indices);
}

SynSkyDome::SynSkyDome(const SynSkyDome& other)
{

}

SynSkyDome::~SynSkyDome()
{

}

bool SynSkyDome::Initialize(ID3D11Device* device)
{
	bool result;

	// Load in the sky dome model.
	result = LoadSkyDomeModel("../SynEngine/data/skydomev2.3ds");
	SAFE_CHECKEXIST(result);

	// Load the sky dome into a vertex and index buffer for rendering.
	result = InitializeBuffers(device);
	SAFE_CHECKEXIST(result);

	// Set the color at the top of the sky dome.
	m_apexColor = D3DXVECTOR4(0.0f, 0.15f, 0.36f, 1.0f);

	// Set the color at the center of the sky dome.
	m_centerColor = D3DXVECTOR4(0.81f, 0.58f, 0.86f, 1.0f);

	return true;
}

void SynSkyDome::Shutdown()
{
	// Release the vertex and index buffer that were used for rendering the sky dome.
	ReleaseBuffers();

	// Release the sky dome model.
	ReleaseSkyDomeModel();

	return;
}

void SynSkyDome::Render(ID3D11DeviceContext* deviceContext)
{
	// Render the sky dome.
	RenderBuffers(deviceContext);

	return;
}

int SynSkyDome::GetIndexCount()
{
	return m_indexCount;
}

D3DXVECTOR4 SynSkyDome::GetApexColor()
{
	return m_apexColor;
}

D3DXVECTOR4 SynSkyDome::GetCenterColor()
{
	return m_centerColor;
}

bool SynSkyDome::LoadSkyDomeModel(char* filename)
{
	const aiScene* importedModel = aiImportFile(filename, aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded);
	SAFE_CHECKEXIST(importedModel);

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

	m_indices = new unsigned long[m_indexCount];
	SAFE_CHECKEXIST(m_indices);
	for (int i = 0; i < indicesVector.size(); i++)
	{
		m_indices[i] = indicesVector[i];
	}

	m_model = new ModelType[m_vertexCount];
	SAFE_CHECKEXIST(m_model);

	for (int i = 0; i<m_vertexCount; i++)
	{
		m_model[i].x = importedModel->mMeshes[0]->mVertices[i].x;
		m_model[i].y = importedModel->mMeshes[0]->mVertices[i].y;
		m_model[i].z = importedModel->mMeshes[0]->mVertices[i].z;
		if (importedModel->mMeshes[0]->HasTextureCoords(0)){
			m_model[i].tu = importedModel->mMeshes[0]->mTextureCoords[0][i].x;
			m_model[i].tv = importedModel->mMeshes[0]->mTextureCoords[0][i].y;
		}
		m_model[i].nx = importedModel->mMeshes[0]->mNormals[i].x;
		m_model[i].ny = importedModel->mMeshes[0]->mNormals[i].y;
		m_model[i].nz = importedModel->mMeshes[0]->mNormals[i].z;
	}

	aiReleaseImport(importedModel);

	return true;
}

void SynSkyDome::ReleaseSkyDomeModel()
{
	SAFE_DELETE_ARRAY(m_model);
	return;
}

bool SynSkyDome::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	SAFE_CHECKEXIST(vertices);

	// Load the vertex array and index array with data.
	for (i = 0; i<m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
	}

	// Set up the description of the vertex buffer.
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

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
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
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(m_indices);

	return true;
}

void SynSkyDome::ReleaseBuffers()
{
	// Release the index buffer.
	SAFE_RELEASE(m_indexBuffer);

	// Release the vertex buffer.
	SAFE_RELEASE(m_vertexBuffer);

	return;
}

void SynSkyDome::RenderBuffers(ID3D11DeviceContext* deviceContext)
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