#include <GUIManager.hpp>
#include <EventHandler.hpp>
#include <Stats.hpp>

void StyleColorsAlteredDracula()
{
    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.05f, 0.05f, 0.085f, .8f};
    colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Border
    colors[ImGuiCol_Border] = ImVec4{0.44f, 0.37f, 0.61f, 0.99f};
    colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

    // Text
    colors[ImGuiCol_Text] = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
    colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.44f, 0.37f, 0.61f, 0.94f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};
    colors[ImGuiCol_CheckMark] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

    // Popups
    colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4{0.44f, 0.37f, 0.61f, 0.94f};
    colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f, 0.54f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.13f, 0.13, 0.17, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

    // Seperator
    colors[ImGuiCol_Separator] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
    colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
    colors[ImGuiCol_SeparatorActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f, 0.29f};
    colors[ImGuiCol_ResizeGripActive] = ImVec4{0.84f, 0.58f, 1.0f, 0.29f};

    // Docking
    // colors[ImGuiCol_DockingPreview] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };

    auto &style = ImGui::GetStyle();
    style.TabRounding = 4;
    style.ScrollbarRounding = 9;
    style.WindowRounding = 7;
    style.GrabRounding = 3;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ChildRounding = 4;
}

void GUIManager::Initialize()
{
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // ImGui::StyleColorsDark();
    StyleColorsAlteredDracula();
    ImGui_ImplGlfw_InitForOpenGL(EventHandler::GetInstance().GetWindowPointer(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    int dpiScale = 2;
    float monScaleX, monScaleY;
    glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &monScaleX, &monScaleY);
    dpiScale = std::max((int)monScaleX, (int)monScaleY);
    this->SetScaling(dpiScale);
}

void GUIManager::Draw()
{
    NewFrame();
    ImGui::PushFont(this->font);

    glm::vec2 win = glm::make_vec2(ImGui::GetContentRegionAvail()) * (float)this->scalingFactor;

    ImGui::SetWindowPos("Renderer Controls",
                        make_ImVec2(win / 50.0f), ImGuiCond_Once);
    ImGui::SetWindowSize("Renderer Controls",
                         make_ImVec2(win * glm::vec2(0.9f, 1.25f)), ImGuiCond_Once);
    ImGui::SetWindowPos("Physics Controls",
                        ImVec2(win.x / 50.0f, win.y / 10.0f + 1.25f * win.y), ImGuiCond_Once);
    ImGui::SetWindowSize("Physics Controls",
                         ImVec2(0.9f * win.x, 0.50f * win.y), ImGuiCond_Once);
    // ImGui::SetWindowPos("Timing",
    //     ImVec2(win.x*2.88f, win.y/50.f), ImGuiCond_Always);

    ImGui::Begin("Renderer Controls", 0, windowFlags);

    DrawHairMeshControls();
    DrawSurfaceMeshControls();
    DrawColliderMeshControls();
    DrawLightControls();

    ImGui::End();
    //-------------------------------------------------------------------
    ImGui::Begin("Physics Controls", 0, windowFlags);

    DrawSimulationControls();
    DrawRodParameters();

    ImGui::End();

    //-------------------------------------------------------------------
    ImGui::SetNextWindowPos(
        ImVec2((ImGui::GetIO().DisplaySize.x - 260.f) * (float)this->scalingFactor, 
        70.f * (float)this->scalingFactor),
         ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::Begin("Timing", NULL, ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
    DrawTimerInfo();
    ImGui::End();
    ImGui::PopFont();
    ImGui::Render();
    ImGuiIO &io = ImGui::GetIO();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIManager::DrawHairMeshControls()
{
    if (ImGui::CollapsingHeader("Hair Mesh Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Show", &scene->hairMesh.show);
        ImGui::SameLine();
        ImGui::Checkbox("Shadows", &scene->hairMesh.shadowsEnable);
        ImGui::SeparatorText("Hair Mesh Material");
        ImGui::ColorEdit4("Color##0", &scene->hairMesh.color[0]);
        ImGui::ColorEdit4("Ambient##0", &scene->hairMesh.ambient[0]);
        ImGui::ColorEdit4("Specular##0", &scene->hairMesh.specular[0]);
        ImGui::DragFloat("Shininess##0", &scene->hairMesh.shininess, 2.0f, 0.0f, 500.0f);
        // ImGui::SameLine();
        // ImGui::Checkbox("Show Control Hairs", nullptr);
        // ImGui::InputInt("Guide hair count", nullptr);
        // ImGui::InputFloat("Guide hair length", nullptr);

        if (ImGui::CollapsingHeader("Hair parameters"))
        {
           auto width = ImGui::GetContentRegionAvail().x;
            // Show the shadow maps
            ImGui::BeginGroup();
            ImGui::Text("LUT0");
            ImGui::Image((void *)scene->hairMesh.lut0->glID,
                         ImVec2(width / 2, width / 2));
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("LUT1");
            ImGui::Image((void *)scene->hairMesh.lut1->glID,
                         ImVec2(width / 2, width / 2));
            ImGui::EndGroup();

            const char* items[] = { "Kajiya-Kay", "Marschner LUT", "Marschner Procedural" };
            ImGui::Combo("Shading Model", &scene->hairMesh.shadingModel, items, IM_ARRAYSIZE(items));
            if(scene->hairMesh.shadingModel==1)
            {
                ImGui::SeparatorText("Marshner Parameters");            
                ImGui::DragFloat("diffuseFalloff",&scene->hairMesh.diffuseFalloff, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("diffuseAzimuthFalloff",&scene->hairMesh.diffuseAzimuthFalloff, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("scaleDiffuse",&scene->hairMesh.scaleDiffuse, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("scaleR",&scene->hairMesh.scaleR, 0.01f, 0.0f, 15.0f);
                ImGui::DragFloat("scaleTT",&scene->hairMesh.scaleTT, 0.01f, 0.0f, 15.0f);
                ImGui::DragFloat("scaleTRT",&scene->hairMesh.scaleTRT, 0.01f, 0.0f, 15.0f);
            }
            else if(scene->hairMesh.shadingModel==2)
            {
                ImGui::SeparatorText("Marshner Procedural Parameters");
                ImGui::DragFloat("diffuseFalloff",&scene->hairMesh.diffuseFalloff, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("diffuseAzimuthFalloff",&scene->hairMesh.diffuseAzimuthFalloff, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("scaleDiffuse",&scene->hairMesh.scaleDiffuse, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("roughness",&scene->hairMesh.roughness, 0.01f, -10.0f, 50.0f);
                ImGui::DragFloat("shift",&scene->hairMesh.shift, 0.01f, -10.0f, 0.0f);
                ImGui::DragFloat("refractive index",&scene->hairMesh.refractiveIndex, 0.01f, 0.0f, 15.0f);
                ImGui::DragFloat("scaleR",&scene->hairMesh.procScaleR, 0.01f, 0.0f, 30.0f);
                ImGui::DragFloat("scaleTT",&scene->hairMesh.procScaleTT, 0.01f, 0.0f, 30.0f);
                ImGui::DragFloat("scaleTRT",&scene->hairMesh.procScaleTRT, 0.01f, 0.0f, 30.0f);
            }
        }
    }
}

void GUIManager::DrawSurfaceMeshControls()
{
    if (ImGui::CollapsingHeader("Surface Mesh Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // ImGui::Checkbox("Show Surface Mesh", nullptr);
        ImGui::SeparatorText("Surface Mesh Material");
        ImGui::ColorEdit3("Specular##1", &scene->surface->mesh.material.specular[0]);
        ImGui::ColorEdit3("Diffuse##1", &scene->surface->mesh.material.diffuse[0]);
        ImGui::ColorEdit3("Ambient##1", &scene->surface->mesh.material.ambient[0]);

        if (ImGui::CollapsingHeader("Transform##0"))
        {
            ImGui::DragFloat3("Position##0", &scene->surface->position.x, 0.01f);
            ImGui::DragFloat3("Rotation##0", &scene->surface->rotation.x, 0.01f);
            ImGui::DragFloat3("Scale##0", &scene->surface->scale.x, 0.01f);
            scene->surface->setTransform();
        }
    }
}

void GUIManager::DrawColliderMeshControls()
{
    if (ImGui::CollapsingHeader("Collider Transform"))
    {
        ImGui::DragFloat3("Position##1", &scene->dummy->position.x, 0.01f);
        ImGui::DragFloat3("Rotation##1", &scene->dummy->rotation.x, 0.01f);
        ImGui::DragFloat3("Scale##1", &scene->dummy->scale.x, 0.01f);
        scene->dummy->setTransform();
    }
}

void GUIManager::DrawLightControls()
{
    if (ImGui::CollapsingHeader("Light Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SeparatorText("Light 1");
        auto width = ImGui::GetContentRegionAvail().x;
        ImGui::PushItemWidth(width * 0.10f);
        ImGui::DragFloat("Intensity", &scene->light.intensity, 0.01f, 0.0f, 1.0f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(width * 0.55f);
        ImGui::ColorEdit3("Color##1", &scene->light.color[0]);
        ImGui::PopItemWidth();
        if (ImGui::DragFloat3("Direction", &scene->light.dir[0], 0.01f))
            scene->light.opacityShadowMaps.dirty = true;
        if (ImGui::CollapsingHeader("Op. Shadow Map", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static float dk = scene->light.opacityShadowMaps.dk;
            if (ImGui::DragFloat("dk", &dk, 0.001f, 0.0001f, 1.0f))
            {
                scene->light.opacityShadowMaps.dk = std::max(dk, 0.0001f);
                scene->light.opacityShadowMaps.dirty = true;
            }

            auto width = ImGui::GetContentRegionAvail().x;
            // Show the shadow maps
            ImGui::BeginGroup();
            ImGui::Text("Depth Map");
            ImGui::Image((void *)scene->light.opacityShadowMaps.depthTex->glID,
                         ImVec2(width / 2, width / 2));
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("Opacity Map");
            ImGui::Image((void *)scene->light.opacityShadowMaps.opacitiesTex->glID,
                         ImVec2(width / 2, width / 2));
            ImGui::EndGroup();
        }
        // ImGui::SeparatorText("Light 2");
        // ImGui::ColorEdit3("Color", &scene->light2.color[0]);
        // ImGui::InputFloat3("Position", &scene->light2.position[0]);
    }
}

void GUIManager::DrawSimulationControls()
{
    if (ImGui::CollapsingHeader("Simulation Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        float dt = physicsIntegrator->getDt();
        if (ImGui::DragFloat("dt", &dt, 0.0001f, 0.0f, 0.1f, "%.5f"))
            physicsIntegrator->setDt(dt);
        int numSteps = physicsIntegrator->getNumSteps();
        if (ImGui::InputInt("numSteps", &numSteps, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
            physicsIntegrator->setNumSteps(numSteps);
    }
}

void GUIManager::DrawRodParameters()
{
    if (ImGui::CollapsingHeader("Elactic Rods", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto width = ImGui::GetContentRegionAvail().x;
        ImGui::PushItemWidth(width * 0.45f);

        ImGui::DragFloat3("gravity",
                          &ElasticRod::gravity[0], 0.001f, -50.0f, 50.0f);
        ImGui::DragFloat("drag",
                         &ElasticRod::drag, 1.0f, 0.0f, 400.0f, "%.4f");
        ImGui::DragFloat("bending stiffness",
                         &ElasticRod::bendingStiffness, 0.001f, 0.0f, 0.1f, "%.4f");
        ImGui::DragInt("pbd iterations",
                        &ElasticRod::pbdIterations, 1, 1, 100);
        ImGui::DragFloat("FTL damping",
                         &ElasticRod::FTLdamping, 0.1f, 0.0f, 1.0f, "%.4f");
        ImGui::DragFloat("Voxel Friciton",
                         &ElasticRod::friction, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Sample Scaling",
                         &ElasticRod::sampledVelocityScale, 0.1f, 0.0f, 100.0f);
        ImGui::PopItemWidth();

        if (ImGui::Button("reset"))
        {
            scene->reset();
        }
    }
}

void GUIManager::DrawTimerInfo()
{
    ImGui::TextColored(ImVec4(0, 0, 0, 1), "Frame time: %.3fms (%.1f FPS)",
                      stats::avgFrameTime * 1000.f, 1.f/stats::avgFrameTime);
    ImGui::TextColored(ImVec4(0.1, 0.1, 0.1, 1), "Render time: %.3fms (%.1f UPS)",
                        stats::avgRenderTime * 1000.f, 1.f/stats::avgRenderTime);
    ImGui::TextColored(ImVec4(0.1, 0.1, 0.1, 1), "Physics time: %.3fms (%.1f UPS)",
                        stats::avgPhysicsTime * 1000.f, 1.f/stats::avgPhysicsTime);
}

void GUIManager::Terminate()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUIManager::SetScaling(int scalingFactor)
{

    ImGui::GetStyle().ScaleAllSizes((float)scalingFactor);
    ImGui::GetIO().FontGlobalScale = (float)scalingFactor;
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;
    fontConfig.SizePixels = 16.0f * scalingFactor;
    this->font = ImGui::GetIO().Fonts->AddFontFromFileTTF(
        "resources/fonts/RobotoMono-Medium.ttf", 16.0f, &fontConfig);
    this->scalingFactor = scalingFactor;
}

void GUIManager::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
