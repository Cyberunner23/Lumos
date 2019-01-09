#include "SceneSelect.h"

using namespace jm;
using namespace maths;

SceneSelect::SceneSelect(const std::string& SceneName)
		: Scene(SceneName)
{
}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::OnInit()
{
    Scene::OnInit();
    m_SceneNames = Application::Instance()->GetSceneManager()->GetSceneNames();
    m_pCamera = new ThirdPersonCamera(-20.0f, -40.0f, maths::Vector3(-3.0f, 10.0f, 15.0f), 60.0f, 0.1f, 10000.0f, (float) m_ScreenWidth / (float) m_ScreenHeight);
}

void SceneSelect::OnIMGUI()
{
    ImGui::Begin(m_SceneName.c_str());

    for(auto& name : m_SceneNames)
    {
        if(name == "SceneSelect")
            continue;

        if(ImGui::Button(name.c_str()))
            Application::Instance()->GetSceneManager()->JumpToScene(name);
    }


    ImGui::End();
}

void SceneSelect::OnCleanupScene()
{
    if (m_CurrentScene)
    {
        if (m_pCamera)
        {
            delete m_pCamera;
            m_pCamera = nullptr;
        }
    }
    
    Scene::OnCleanupScene();
}