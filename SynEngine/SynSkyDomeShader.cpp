#include "synskydomeshader.h"

SynSkyDomeShader::SynSkyDomeShader()
{
	SAFE_INIT(m_vertexShader);
	SAFE_INIT(m_pixelShader);
	SAFE_INIT(m_layout);
	SAFE_INIT(m_matrixBuffer);
	SAFE_INIT(m_gradientBuffer);
}

SynSkyDomeShader::SynSkyDomeShader(const SynSkyDomeShader& other)
{

}

SynSkyDomeShader::~SynSkyDomeShader()
{

}

bool SynSkyDomeShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../SynEngine/skydomevertexshader.hlsl", L"../SynEngine/skydomepixelshader.hlsl");
	SAFE_CHECKEXIST(result);

	return true;
}

void SynSkyDomeShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool SynSkyDomeShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix,
	DirectX::XMMATRIX projectionMatrix, DirectX::XMFLOAT4 apexColor, DirectX::XMFLOAT4 centerColor)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor);
	SAFE_CHECKEXIST(result);

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool SynSkyDomeShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC gradientBufferDesc;

	// Initialize the pointers this function will use to null.
	SAFE_INIT(errorMessage);
	SAFE_INIT(vertexShaderBuffer);
	SAFE_INIT(pixelShaderBuffer);
	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
	&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBoxW(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBoxW(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the gradient constant buffer that is in the pixel shader.
	gradientBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	gradientBufferDesc.ByteWidth = sizeof(GradientBufferType);
	gradientBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gradientBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gradientBufferDesc.MiscFlags = 0;
	gradientBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&gradientBufferDesc, NULL, &m_gradientBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SynSkyDomeShader::ShutdownShader()
{
	// Release the gradient constant buffer.
	SAFE_RELEASE(m_gradientBuffer);

	// Release the matrix constant buffer.	
	SAFE_RELEASE(m_matrixBuffer);

	// Release the layout.	
	SAFE_RELEASE(m_layout);

	// Release the pixel shader.	
	SAFE_RELEASE(m_pixelShader);

	// Release the vertex shader.	
	SAFE_RELEASE(m_vertexShader);

	return;
}

void SynSkyDomeShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	SAFE_RELEASE(errorMessage);

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBoxW(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool SynSkyDomeShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix,
	DirectX::XMMATRIX projectionMatrix, DirectX::XMFLOAT4 apexColor, DirectX::XMFLOAT4 centerColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	GradientBufferType* dataPtr2;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	//D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	//D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);
	//D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Lock the gradient constant buffer so it can be written to.
	result = deviceContext->Map(m_gradientBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (GradientBufferType*)mappedResource.pData;

	// Copy the gradient color variables into the constant buffer.
	dataPtr2->apexColor = apexColor;
	dataPtr2->centerColor = centerColor;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_gradientBuffer, 0);

	// Set the position of the gradient constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the gradient constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_gradientBuffer);

	return true;
}

void SynSkyDomeShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render the triangles.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}