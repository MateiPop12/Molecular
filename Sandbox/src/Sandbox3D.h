#pragma once

#include "Molecular.h"

class Sandbox3D final : public Molecular::Layer{
public:
    Sandbox3D();
    ~Sandbox3D() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Molecular::Timestep ts) override;
    void OnImGuiRender() override;
    void OnEvent(Molecular::Event& e) override;

    //Molecular::OrthographicCameraController m_CameraController;

    Molecular::EditorCamera m_CameraController;

    Molecular::Ref<Molecular::Shader> m_Shader;
    Molecular::Ref<Molecular::VertexArray> m_VertexArray;
    Molecular::Ref<Molecular::Texture2D> m_Texture;
    Molecular::Ref<Molecular::Framebuffer> m_Framebuffer;

private:
    bool m_ViewportFocused = false, m_ViewportHovered = false;
    static void DrawSpace(short type, float size);

    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    glm::vec2 m_ViewportBounds[2];
};




