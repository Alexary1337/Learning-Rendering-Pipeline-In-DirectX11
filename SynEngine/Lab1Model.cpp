#include "Lab1Model.h"

Lab1Model::Lab1Model()
{
	SAFE_INIT(m_vertexBuffer);
	SAFE_INIT(m_indexBuffer);
}

Lab1Model::~Lab1Model()
{

}

bool Lab1Model::Initialize(ID3D11Device* device, char* modelFilename)
{
	bool result;
		
	// Load the rendering buffers with the terrain data.
	result = InitializeBuffers(device, modelFilename);
	SAFE_CHECKEXIST(result);

	return true;
}

void Lab1Model::Shutdown()
{
	// Release the rendering buffers.
	ShutdownBuffers();

	return;
}

bool Lab1Model::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

ID3D11ShaderResourceView* Lab1Model::GetTexture()
{
	return m_Texture->GetTexture();
}

int Lab1Model::GetIndexCount()
{
	return m_indexCount;
}

bool Lab1Model::InitializeBuffers(ID3D11Device* device, char* modelFilename)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	std::vector<unsigned long> indicesVector;
	std::vector<VertexType> verticesVector;

	std::ifstream File(modelFilename);
	string Line;
	string Name;
	int afterCurveIndexOffset = 0;
	int afterCurveIndexBarrier = 0;

	while (std::getline(File, Line)) {
		if (Line == "")
			continue;

		std::istringstream LineStream(Line);
		LineStream >> Name;

		int doCurve = 0;

		float cp1_x = 0.0f;
		float cp1_y = 0.0f;
		float cp2_x = 0.0f;
		float cp2_y = 0.0f;



		if (Name == "v") {
			float Vertex[3];
			sscanf_s(Line.c_str(), "%*s %f %f %f %d %f %f %f %f", &Vertex[0], &Vertex[1], &Vertex[2], &doCurve, &cp1_x, &cp1_y, &cp2_x, &cp2_y);
			VertexType temp = { DirectX::XMFLOAT3(Vertex[0], Vertex[1], Vertex[2]) };

			//todo
			if (doCurve == 1)
			{
				auto previousVertex = verticesVector.back().position;
				auto currentVertex = temp.position;

				verticesVector.pop_back();


				DirectX::XMFLOAT3 cp1 = { cp1_x, cp1_y, previousVertex.z };
				DirectX::XMFLOAT3 cp2 = { cp2_x, cp2_y, currentVertex.z };

				if (afterCurveIndexBarrier == 0)
					afterCurveIndexBarrier = verticesVector.size();


				CalculateBezier(verticesVector, indicesVector, previousVertex, currentVertex, cp1, cp2);

				afterCurveIndexOffset = 50;
			}
			else {
				verticesVector.push_back(temp);
			}
		}

		if (Name == "l") {
			unsigned long index;
			sscanf_s(Line.c_str(), "%*s %d", &index);
			if (index > afterCurveIndexBarrier )
				index += afterCurveIndexOffset;
			indicesVector.push_back(index);
		}
	};

	m_indexCount = indicesVector.size();

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * verticesVector.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &verticesVector[0];
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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indicesVector.size();
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

	verticesVector.clear();
	indicesVector.clear();

	return true;
}

void Lab1Model::ShutdownBuffers()
{
	// Release the index buffer.
	SAFE_RELEASE(m_indexBuffer);

	// Release the vertex buffer.
	SAFE_RELEASE(m_vertexBuffer);

	return;
}

void Lab1Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}




bool Lab1Model::InitializeBezier(ID3D11Device* device, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 cp1, DirectX::XMFLOAT3 cp2)
{
	bool result;

	// Load the rendering buffers with the terrain data.
	result = InitializeBuffersBezier(device, p1, p2, cp1, cp2);
	SAFE_CHECKEXIST(result);

	return true;
}

bool Lab1Model::InitializeBuffersBezier(ID3D11Device* device, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 cp1, DirectX::XMFLOAT3 cp2)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	std::vector<unsigned long> indicesVector;
	std::vector<VertexType> verticesVector;

	CalculateBezier(verticesVector, indicesVector, p1, p2, cp1, cp2);

	m_indexCount = indicesVector.size();

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * verticesVector.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &verticesVector[0];
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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indicesVector.size();
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

	verticesVector.clear();
	indicesVector.clear();

	return true;
}


void Lab1Model::CalculateBezier(std::vector<VertexType> &verticesVector, std::vector<unsigned long> &indicesVector,
	DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 cp1, DirectX::XMFLOAT3 cp2) {

	unsigned long counter;

	if (verticesVector.size() > 0)
		counter = verticesVector.size();
	else counter = 0;


	int numSegments = 25;
	for (int i = 0; i <= numSegments; ++i) {
		float t = i / (float)numSegments;
		// compute coefficients
		float k1 = (1 - t) * (1 - t) * (1 - t);
		float k2 = 3 * (1 - t) * (1 - t) * t;
		float k3 = 3 * (1 - t) * t * t;
		float k4 = t * t * t;

		// weight the four control points using coefficients
		VertexType temp = { DirectX::XMFLOAT3(p1.x * k1 + cp1.x * k2 + cp2.x * k3 + p2.x * k4, p1.y * k1 + cp1.y * k2 + cp2.y * k3 + p2.y * k4, p1.z) };
		verticesVector.push_back({ temp });

		if (counter >= 2)
			indicesVector.push_back(counter - 1);
		indicesVector.push_back(counter);
		counter++;
	}
}
