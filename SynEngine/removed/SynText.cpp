#include "syntext.h"

SynText::SynText()
{
	SAFE_INIT(m_Font);
	SAFE_INIT(m_FontShader);
	SAFE_INIT(m_sentence1);
	SAFE_INIT(m_sentence2);
	SAFE_INIT(m_sentence3);
}

SynText::SynText(const SynText& other)
{

}

SynText::~SynText()
{

}

bool SynText::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight,
	DirectX::XMMATRIX baseViewMatrix)
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

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence2, 16, device);
	SAFE_CHECKEXIST(result);

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence3, 128, device);
	SAFE_CHECKEXIST(result);

	return true;
}

void SynText::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&m_sentence1);

	// Release the second sentence.
	ReleaseSentence(&m_sentence2);

	// Release the second sentence.
	ReleaseSentence(&m_sentence3);

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

bool SynText::Render(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX orthoMatrix)
{
	bool result;

	// Draw the first sentence.
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	SAFE_CHECKEXIST(result);

	// Draw the second sentence.
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	SAFE_CHECKEXIST(result);

	// Draw the second sentence.
	result = RenderSentence(deviceContext, m_sentence3, worldMatrix, orthoMatrix);
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
		SAFE_RELEASE((*sentence)->vertexBuffer);

		// Release the sentence index buffer.
		SAFE_RELEASE((*sentence)->indexBuffer);

		// Release the sentence.
		SAFE_DELETE(*sentence);
	}

	return;
}

bool SynText::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, DirectX::XMMATRIX worldMatrix,
	DirectX::XMMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	DirectX::XMFLOAT4 pixelColor;
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
	pixelColor = DirectX::XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(),
		pixelColor);
	SAFE_CHECKEXIST(result);

	return true;
}

bool SynText::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char fpsString[16];
	float red, green, blue;
	bool result;

	// Truncate the fps to below 10,000.
	if (fps > 9999)
	{
		fps = 9999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// If fps is 60 or above set the fps color to green.
	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 60 set the fps color to yellow.
	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 30 set the fps color to red.
	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);
	SAFE_CHECKEXIST(result);

	return true;
}

bool SynText::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;

	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
	SAFE_CHECKEXIST(result);

	result = UpdateSentence(m_sentence3, "Function: Y = sqrt(x^2 + z^2) + 3 * cos(sqrt(x^2 + z^2)) + 5", 20, 60, 0.0f, 1.0f, 0.0f, deviceContext);
	SAFE_CHECKEXIST(result);

	return true;
}