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

bool SynTerrain::Initialize(ID3D11Device* device)
{
	bool result;

	// Load the rendering buffers with the terrain data.
	result = InitializeBuffers(device);
	SAFE_CHECKEXIST(result);

	return true;
}

void SynTerrain::Shutdown()
{
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

int SynTerrain::GetIndexCount()
{
	return m_indexCount;
}

bool SynTerrain::InitializeBuffers(ID3D11Device* device)
{
	//TODO
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
