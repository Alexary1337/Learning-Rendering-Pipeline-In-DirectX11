#include "syngraphics.h"

SynGraphics::SynGraphics()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
	m_Light = 0;
	m_Text = 0;
}

SynGraphics::SynGraphics(const SynGraphics& other)
{

}

SynGraphics::~SynGraphics()
{

}

bool SynGraphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	D3DXMATRIX baseViewMatrix;


	// Create the Direct3D object.
	m_D3D = new SynD3D;
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}
	
	// Create the camera object.
	m_Camera = new SynCamera;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 50.0f, -250.0f);

	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Create the model object.
	m_Model = new SynModel;
	if (!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), L"../SynEngine/data/nissan.psd", "../SynEngine/data/gtr.3ds");
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model object. Check path in SynGraphics cpp file.", "Error", MB_OK);
		return false;
	}	

	// Create the color shader object.
	m_ColorShader = new SynColorShader;
	if (!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the color shader object.", "Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new SynLight;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(64.0f);


	// Create the text object.
	m_Text = new SynText;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the text object.", "Error", MB_OK);
		return false;
	}

	return true;
}

void SynGraphics::Shutdown()
{
	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
	
	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

bool SynGraphics::Frame(int way)
{
	bool result;

	static float rotation = 0.0f;


	// Update the rotation variable each frame.
	if (way==1)
	{
		rotation += (float)D3DX_PI * 0.003f;
	}
	if (way == 2)
	{
		rotation -= (float)D3DX_PI * 0.003f;
	}

	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}

bool SynGraphics::Render(float rotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();

	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();
	
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	//TODO: fix this scary thing
	if (rotation!=0){
		D3DXMatrixRotationYawPitchRoll(&worldMatrix, rotation, -300, 0);
	}
	else D3DXMatrixRotationYawPitchRoll(&worldMatrix, 0, -300, 0);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the color shader.
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
	return true;
}

bool SynGraphics::TurnWF()
{
	bool result;

	result = m_D3D->Wireframe();
	if (!result)
	{
		return false;
	}

	return true;
}

void SynGraphics::ChangeCameraPosition(bool way)
{	
	D3DXVECTOR3 pos = m_Camera->GetPosition();
	if (way)
	{
		m_Camera->SetPosition(pos.x, pos.y, pos.z + 10.5f);
	}
	else
	{
		m_Camera->SetPosition(pos.x, pos.y, pos.z - 10.5f);
	}
}