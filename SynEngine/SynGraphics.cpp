#include "syngraphics.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"

float func(float x, float z);
SynGraphics::SynGraphics()
{
	SAFE_INIT(m_D3D);
	SAFE_INIT(m_Camera);
	SAFE_INIT(m_Model);
	SAFE_INIT(m_ColorShader);
	SAFE_INIT(m_Light);
	SAFE_INIT(m_SkyDome);
	SAFE_INIT(m_SkyDomeShader);
	SAFE_INIT(m_Position);
	SAFE_INIT(m_Timer);
	SAFE_INIT(m_FpsCounter);
	SAFE_INIT(m_CpuUsage);
	SAFE_INIT(m_Input);
	SAFE_INIT(m_Terrain);
	SAFE_INIT(m_TerrainShader);
	SAFE_INIT(m_meshCount);
	SAFE_INIT(m_totalIndexCount);
	SAFE_INIT(m_translationVector3D);
	SAFE_INIT(m_GUI);
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

	DirectX::XMMATRIX baseViewMatrix;

	CONSOLE_OUT("Initializing Direct3D component...");
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
	CONSOLE_OUT("Direct3D component initialized.");

	CONSOLE_OUT("Initializing camera component...");
	// Create the camera object.
	m_Camera = new SynCamera;
	SAFE_CHECKEXIST(m_Camera);

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -0.11f); //TODO: fix bug with text rendering

	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	CONSOLE_OUT("Camera component initialized.");

	CONSOLE_OUT("Initializing model component...");
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
	CONSOLE_OUT("Model component initialized.");

	CONSOLE_OUT("Initializing model shader component...");
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
	CONSOLE_OUT("Model shader component initialized.");

	CONSOLE_OUT("Initializing light component...");
	// Create the light object.
	m_Light = new SynLight;
	SAFE_CHECKEXIST(m_Light);

	// Initialize the light object.
	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(0.4f, 0.4f, 0.4f, 1.0f);
	m_Light->SetDirection(1.0f, -1.0f, 0.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);
	CONSOLE_OUT("Light component initialized.");

	CONSOLE_OUT("Initializing skydome component...");
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
	CONSOLE_OUT("Skydome component initialized.");

	CONSOLE_OUT("Initializing skydome shader component...");
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
	CONSOLE_OUT("Skydome shader component initialized.");

	CONSOLE_OUT("Initializing position component...");
	// Create the position object.
	m_Position = new SynPosition;
	SAFE_CHECKEXIST(m_Position);

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition(0.0f, 0.0f, 0.0f);
	CONSOLE_OUT("Position component initialized.");

	CONSOLE_OUT("Initializing timer component...");
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
	CONSOLE_OUT("Timer component initialized.");

	CONSOLE_OUT("Initializing FPS counter component...");
	// Create the fps object.
	m_FpsCounter = new SynFpsCounter;
	SAFE_CHECKEXIST(m_FpsCounter);

	// Initialize the fps object.
	m_FpsCounter->Initialize();
	CONSOLE_OUT("FPS counter component initialized.");

	CONSOLE_OUT("Initializing CPU usage component...");
	// Create the cpu object.
	m_CpuUsage = new SynCpuUsage;
	SAFE_CHECKEXIST(m_CpuUsage);

	// Initialize the cpu object.
	m_CpuUsage->Initialize();
	CONSOLE_OUT("CPU usage component initialized.");

	CONSOLE_OUT("Initializing input component...");
	m_Input = new SynInput;
	SAFE_CHECKEXIST(m_Input);
	m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	CONSOLE_OUT("Input component initialized.");

	CONSOLE_OUT("Initializing terrain component...");
	m_Terrain = new SynTerrain;
	SAFE_CHECKEXIST(m_Terrain);
	//m_Terrain->Initialize(m_D3D->GetDevice(), L"../SynEngine/data/riffle.dds", "../SynEngine/data/test123.bmp");	
	m_Terrain->InitializeByFunction(m_D3D->GetDevice(), L"../SynEngine/data/riffle.dds", 128, -32.0f, -32.0f, func);
	CONSOLE_OUT("Terrain component initialized.");

	CONSOLE_OUT("Initializing terrain shader component...");
	// Create the terrain shader object.
	m_TerrainShader = new SynTerrainShader;
	SAFE_CHECKEXIST(m_TerrainShader);

	// Initialize the terrain shader object.
	result = m_TerrainShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the terrain shader object.", "Error", MB_OK);
		return false;
	}
	CONSOLE_OUT("Terrain shader component initialized.");

	m_translationVector3D = new DirectX::XMFLOAT3(-135.0f, -10.0f, -135.0f);


	m_GUI = new SynGUI();
	m_GUI->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd);


	return true;
}

float func(float x, float z)
{
	//return 100 - 3 / sqrtf(x*x + z*z) + sinf(sqrtf(x*x + z*z)) + sqrtf(200 - x*x + z*z + 10 * sinf(x) + 10 * sinf(z)) / 1000; //todo
	//return sinf(x)*cosf(z);
	return sqrtf(x*x + z * z) + 3 * cosf(sqrtf(x*x + z * z)) + 5;
	//return (x*x) - (z*z);

}

void SynGraphics::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	SAFE_DELETE(m_translationVector3D);

	// Release the terrain shader object.
	if (m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		SAFE_DELETE(m_TerrainShader);
	}

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

	// Release the GUI object.
	SAFE_DELETE(m_GUI);

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

	// Do the input frame processing + camera rotation update.
	m_Position->MouseRotate(m_Input->Frame(!isCursorVisible));
	

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

	
	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	
	if (!isCursorVisible)
	{		
		m_Position->GetRotation(rotX, rotY, rotZ);
		m_Camera->SetRotation(rotX, rotY, rotZ);
	}

	if (m_Input->IsEToggled())
	{
		isCursorVisible = !isCursorVisible;
		ShowCursor(isCursorVisible);
		if (!isCursorVisible)
		{
			
			m_Position->SetRotation(0,0,0);
			//m_Input->CenterCursor();
		}
	}


	if (m_Input->IsF1Toggled())
	{
		m_D3D->WireframeTerrainRaster();
	}

	if (m_Input->IsF2Toggled())
	{
		m_D3D->WireframeDefaultRaster();
	}

	if (m_Input->IsF3Toggled())
	{
		m_D3D->WireframeSkydomeRaster();
	}

	return true;
}

bool SynGraphics::Render()
{

	DirectX::XMFLOAT3 cameraPosition;
	DirectX::XMMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
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
	worldMatrix = DirectX::XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	//D3DXMatrixTranslation(&worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Turn off back face culling.
	m_D3D->TurnSkydomeRaster();

	// Turn off the Z buffer.
	m_D3D->TurnZBufferOff();

	// Render the sky dome using the sky dome shader.
	m_SkyDome->Render(m_D3D->GetDeviceContext());
	m_SkyDomeShader->Render(m_D3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());

	// Turn back face culling back on.
	m_D3D->TurnDefaultRaster();

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	//m_D3D->TurnOnAlphaBlending();

	// Turn off alpha blending after rendering the text.
	//m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	m_D3D->TurnTerrainRaster();
	//D3DXMatrixTransformation(&worldMatrix, NULL, NULL, NULL, NULL, NULL, m_translationVector3D);
	m_Terrain->Render(m_D3D->GetDeviceContext());
	result = m_TerrainShader->Render(m_D3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());
	SAFE_CHECKEXIST(result);
	m_D3D->TurnDefaultRaster();

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	worldMatrix = DirectX::XMMatrixRotationX(1.6f);
	//D3DXMatrixRotationX(&worldMatrix, 1.6f);

	for (int i = 0; i < *m_meshCount; i++)
	{
		m_Model[i].Render(m_D3D->GetDeviceContext());
		result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model[i].GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model[i].GetTexture(), m_Light->GetDirection(),
			m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		SAFE_CHECKEXIST(result);
	}

	// Render user interface
	m_GUI->Render(m_D3D, m_Light);

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
	return true;
}
