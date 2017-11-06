#include "syngraphics.h"

SynGraphics::SynGraphics()
{
	SAFE_INIT(m_D3D);
	SAFE_INIT(m_Camera);
	SAFE_INIT(m_Model);
	SAFE_INIT(m_ColorShader);
	SAFE_INIT(m_Light);
	SAFE_INIT(m_Text);
	SAFE_INIT(m_SkyDome);
	SAFE_INIT(m_SkyDomeShader);
	SAFE_INIT(m_Position);
	SAFE_INIT(m_Timer);
	SAFE_INIT(m_FpsCounter);
	SAFE_INIT(m_CpuUsage);
	SAFE_INIT(m_Input);
	SAFE_INIT(m_Terrain);
	SAFE_INIT(m_meshCount);
	SAFE_INIT(m_totalIndexCount);
}

SynGraphics::SynGraphics(const SynGraphics& other)
{

}

SynGraphics::~SynGraphics()
{

}

bool SynGraphics::Initialize(int screenWidth, int screenHeight, HWND hwnd, HINSTANCE hinstance)
{
	bool result;

	D3DXMATRIX baseViewMatrix;

	// Create the Direct3D object.
	m_D3D = new SynD3D;
	SAFE_CHECKEXIST(m_D3D);

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}
	
	// Create the camera object.
	m_Camera = new SynCamera;
	SAFE_CHECKEXIST(m_Camera);

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -0.11f); //TODO: fix bug with text rendering

	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//TODO: refactor multi mesh rendering pipeline
	m_meshCount = new int;
	SAFE_CHECKEXIST(m_meshCount);

	m_totalIndexCount = new int;
	SAFE_CHECKEXIST(m_totalIndexCount);

	const aiScene* importedModel = aiImportFile("../SynEngine/data/riffle.3ds", aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded);
	*m_meshCount = importedModel->mNumMeshes;
	*m_totalIndexCount = 0;

	// Create the model object.
	m_Model = new SynModel[*m_meshCount];
	SAFE_CHECKEXIST(m_Model);


	for (int i = 0; i < *m_meshCount; i++)
	{
		for (int j = 0; j < importedModel->mMeshes[i]->mNumFaces; j++) {
			if (importedModel->mMeshes[i]->mFaces[j].mNumIndices == 3) {
				*m_totalIndexCount += 3;
			}
		}

		result = m_Model[i].Initialize(m_D3D->GetDevice(), L"../SynEngine/data/riffle.dds", "../SynEngine/data/riffle.3ds", i);
		if (!result)
		{
			MessageBox(hwnd, "Could not initialize the model object. Check path in SynGraphics cpp file.", "Error", MB_OK);
			return false;
		}
	}

	aiReleaseImport(importedModel);

	// Create the color shader object.
	m_ColorShader = new SynColorShader;
	SAFE_CHECKEXIST(m_ColorShader);

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the color shader object.", "Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new SynLight;
	SAFE_CHECKEXIST(m_Light);

	// Initialize the light object.
	m_Light->SetAmbientColor(0.35f, 0.35f, 0.35f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, -1.0f, 0.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// Create the text object.
	m_Text = new SynText;
	SAFE_CHECKEXIST(m_Text);

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the text object.", "Error", MB_OK);
		return false;
	}

	// Create the sky dome object.
	m_SkyDome = new SynSkyDome;
	SAFE_CHECKEXIST(m_SkyDome);
	
	// Initialize the sky dome object.
	result = m_SkyDome->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBoxW(hwnd, L"Could not initialize the sky dome object.", L"Error", MB_OK);
		return false;
	}

	// Create the sky dome shader object.
	m_SkyDomeShader = new SynSkyDomeShader;
	SAFE_CHECKEXIST(m_SkyDomeShader);

	// Initialize the sky dome shader object.
	result = m_SkyDomeShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBoxW(hwnd, L"Could not initialize the sky dome shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new SynPosition;
	SAFE_CHECKEXIST(m_Position);

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition(0.0f, 0.0f, 0.0f);

	// Create the timer object.
	m_Timer = new SynTimer;
	SAFE_CHECKEXIST(m_Timer);

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBoxW(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the fps object.
	m_FpsCounter = new SynFpsCounter;
	SAFE_CHECKEXIST(m_FpsCounter);

	// Initialize the fps object.
	m_FpsCounter->Initialize();

	// Create the cpu object.
	m_CpuUsage = new SynCpuUsage;
	SAFE_CHECKEXIST(m_CpuUsage);

	// Initialize the cpu object.
	m_CpuUsage->Initialize();

	m_Input = new SynInput;
	SAFE_CHECKEXIST(m_Input);
	m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	
	m_Terrain = new SynTerrain;
	SAFE_CHECKEXIST(m_Terrain);
	m_Terrain->Initialize(m_D3D->GetDevice(), L"../SynEngine/data/riffle.dds", "../SynEngine/data/heightmap.bmp");

	return true;
}

void SynGraphics::Shutdown()
{
	// Release the terrain object.
	SAFE_DELETE(m_Terrain);

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		SAFE_DELETE(m_Input);
	}

	// Release the position object.
	SAFE_DELETE(m_CpuUsage);

	// Release the position object.
	SAFE_DELETE(m_FpsCounter);	

	// Release the position object.
	SAFE_DELETE(m_Timer);

	// Release the position object.
	SAFE_DELETE(m_Position);

	// Release the sky dome shader object.
	if (m_SkyDomeShader)
	{
		m_SkyDomeShader->Shutdown();
		SAFE_DELETE(m_SkyDomeShader);
	}

	// Release the sky dome object.
	if (m_SkyDome)
	{
		m_SkyDome->Shutdown();
		SAFE_DELETE(m_SkyDome);
	}

	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		SAFE_DELETE(m_Text);
	}
	
	// Release the light object.
	SAFE_DELETE(m_Light);

	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		SAFE_DELETE(m_ColorShader);
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		SAFE_DELETE_ARRAY(m_Model);
	}

	// Release the camera object.
	SAFE_DELETE(m_Camera);

	if (m_D3D)
	{
		m_D3D->Shutdown();
		SAFE_DELETE(m_D3D);
	}
	return;
}

bool SynGraphics::Frame()
{
	bool result;

	// Do the input frame processing.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to quit.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Update the system stats.
	m_Timer->Frame();
	m_FpsCounter->Frame();
	m_CpuUsage->Frame();

	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
 	SAFE_CHECKEXIST(result);

	// Set the frames per second.
	result = m_Text->SetFps(m_FpsCounter->GetFps(), m_D3D->GetDeviceContext());
	SAFE_CHECKEXIST(result);

	// Set the cpu usage.
	result = m_Text->SetCpu(m_CpuUsage->GetCpuPercentage(), m_D3D->GetDeviceContext());
	SAFE_CHECKEXIST(result);

	// Render the graphics scene.
	result = Render();
	SAFE_CHECKEXIST(result);

	return true;
}

//float radius=0.0f;
bool SynGraphics::HandleInput(float frameTime)
{
	bool result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	m_Position->TurnLeft(m_Input->IsKeyPressed(DIK_LEFT));
	m_Position->TurnRight(m_Input->IsKeyPressed(DIK_RIGHT));
	m_Position->MoveForward(m_Input->IsKeyPressed(DIK_W));

	m_Position->MoveLeft(m_Input->IsKeyPressed(DIK_A));
	m_Position->MoveRight(m_Input->IsKeyPressed(DIK_D));

	m_Position->MoveBackward(m_Input->IsKeyPressed(DIK_S));
	m_Position->MoveUpward(m_Input->IsKeyPressed(DIK_SPACE));
	m_Position->MoveDownward(m_Input->IsKeyPressed(DIK_LCONTROL));
	m_Position->LookUpward(m_Input->IsKeyPressed(DIK_UP));
	m_Position->LookDownward(m_Input->IsKeyPressed(DIK_DOWN));

	m_Position->MouseTest(m_Input->GetMouseXLocation(),m_Input->GetMouseYLocation());

	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);
	//radius += 0.5f;

	if (m_Input->IsF1Toggled())
	{
		TurnWF();
	}

	return true;
}

bool SynGraphics::Render()
{
	D3DXVECTOR3 cameraPosition;
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

	// Get the position of the camera.
	cameraPosition = m_Camera->GetPosition();

	// Translate the sky dome to be centered around the camera position.
	D3DXMatrixTranslation(&worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Turn off back face culling.
	m_D3D->TurnOffCulling();

	// Turn off the Z buffer.
	m_D3D->TurnZBufferOff();

	// Render the sky dome using the sky dome shader.
	m_SkyDome->Render(m_D3D->GetDeviceContext());
	m_SkyDomeShader->Render(m_D3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());

	// Turn back face culling back on.
	m_D3D->TurnOnCulling();

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();

	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	SAFE_CHECKEXIST(result);

	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();
	
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();


	m_Terrain->Render(m_D3D->GetDeviceContext());
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Terrain->GetTexture(), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	SAFE_CHECKEXIST(result);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	D3DXMatrixRotationX(&worldMatrix, 1.6f);
	for (int i = 0; i < *m_meshCount; i++)
	{
		m_Model[i].Render(m_D3D->GetDeviceContext());
		result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model[i].GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model[i].GetTexture(), m_Light->GetDirection(),
				m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		SAFE_CHECKEXIST(result);
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
	return true;
}

bool SynGraphics::TurnWF()
{
	bool result;

	result = m_D3D->Wireframe();
	SAFE_CHECKEXIST(result);

	return true;
}