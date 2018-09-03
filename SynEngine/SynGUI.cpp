#include "SynGUI.h"

SynGUI::SynGUI()
{

}

SynGUI::~SynGUI()
{

}

void SynGUI::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
{
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplDX11_Init(hwnd, device, deviceContext);

	// Setup style
	ImGui::StyleColorsDark();
}

void SynGUI::Render(SynD3D* D3D, SynLight* light)
{
	ImGui_ImplDX11_NewFrame();

	ImGui::Begin("Options");
	{
		ImGui::Text("Use 'E' button for toggle cursor visibility");



		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	}
	ImGui::End();

	ImGui::Begin("Display Options");
	{
		static bool skyDomeWf = false;
		static bool terrainWf = false;
		static bool modelWf = false;

		if (ImGui::Checkbox("Skydome Wireframe", &skyDomeWf))
			D3D->WireframeSkydomeRaster();
		if (ImGui::Checkbox("Terrain Wireframe", &terrainWf))
			D3D->WireframeTerrainRaster();
		if (ImGui::Checkbox("Model Wireframe", &modelWf))
			D3D->WireframeDefaultRaster();

	}
	ImGui::End();

	ImGui::Begin("Lightning Optoins");
	{

		ImGui::ColorEdit3("Diffuse Color", (float*)&m_diffuseColor);
		light->SetDiffuseColor(m_diffuseColor.x, m_diffuseColor.y, m_diffuseColor.z, m_diffuseColor.w);

		ImGui::ColorEdit3("Ambient Color", (float*)&m_ambientColor);
		light->SetAmbientColor(m_ambientColor.x, m_ambientColor.y, m_ambientColor.z, m_ambientColor.w);

		ImGui::ColorEdit3("Specular Color", (float*)&m_specularColor);
		light->SetSpecularColor(m_specularColor.x, m_specularColor.y, m_specularColor.z, m_specularColor.w);
		
		static float f = 32.0f;
		ImGui::SliderFloat("Specular Power", &f, 0.0f, 100.0f);
		light->SetSpecularPower(f);

	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}