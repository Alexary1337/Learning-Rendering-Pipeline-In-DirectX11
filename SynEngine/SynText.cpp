#include "syntext.h"

SynText::SynText()
{
	SAFE_INIT(m_Font);
	SAFE_INIT(m_FontShader);
	SAFE_INIT(m_sentence1);
	SAFE_INIT(m_sentence2);
}

SynText::SynText(const SynText& other)
{

}

SynText::~SynText()
{

}

bool SynText::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight,
	D3DXMATRIX baseViewMatrix)
{
	bool result;

	// Store the screen width and height.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix.
	m_baseViewMatrix = baseViewMatrix;

	// Create the font object.
	m_Font = new SynFont;
	SAFE_CHECKEXIST(m_Font);

	// Initialize the font object.
	result = m_Font->Initialize(device, "../SynEngine/data/fontdata.txt", L"../SynEngine/data/font.dds");
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the font object.", "Error", MB_OK);
		return false;
	}

	// Create the font shader object.
	m_FontShader = new SynFontShader;
	SAFE_CHECKEXIST(m_FontShader);

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the font shader object.", "Error", MB_OK);
		return false;
	}

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence1, 16, device);
	SAFE_CHECKEXIST(result);

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, "FPS: 60", 100, 100, 0.0f, 1.0f, 0.0f, deviceContext);
	SAFE_CHECKEXIST(result);

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence2, 16, device);
	SAFE_CHECKEXIST(result);

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, "F1 -  wireframe", 100, 150, 0.0f, 1.0f, 0.0f, deviceContext);
	SAFE_CHECKEXIST(result);

	return true;
}

void SynText::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&m_sentence1);

	// Release the second sentence.
	ReleaseSentence(&m_sentence2);

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		SAFE_DELETE(m_FontShader);
	}

	// Release the font object.
	if (m_Font)
	{
		m_Font->Shutdown();
		SAFE_DELETE(m_Font);
	}

	return;
}

bool SynText::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	// Draw the first sentence.
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	SAFE_CHECKEXIST(result);

	// Draw the second sentence.
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	SAFE_CHECKEXIST(result);

	return true;
}

bool SynText::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Create a new sentence object.
	*sentence = new SentenceType;
	SAFE_CHECKEXIST(*sentence);

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	SAFE_CHECKEXIST(vertices);

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	SAFE_CHECKEXIST(indices);

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for (i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	SAFE_DELETE_ARRAY(vertices);

	// Release the index array as it is no longer needed.
	SAFE_DELETE_ARRAY(indices);

	return true;
}

bool SynText::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue,
	ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Store the color of the sentence.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	SAFE_CHECKEXIST(vertices);

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	SAFE_DELETE_ARRAY(vertices);

	return true;
}

void SynText::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// Release the sentence vertex buffer.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		SAFE_DELETE(*sentence);
	}

	return;
}

bool SynText::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix,
	D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	bool result;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(),
		pixelColor);
	SAFE_CHECKEXIST(result);

	return true;
}