#pragma once
#include "Molecular.h"

class Sandbox2D : public Molecular::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Molecular::Timestep ts) override;
    void OnImGuiRender() override;
    void OnEvent(Molecular::Event& e) override;
private:
    Molecular::OrthographicCameraController m_cameraController;

    Molecular::Ref<Molecular::Shader> m_shader;
    Molecular::Ref<Molecular::VertexArray> m_vertexArray;
    Molecular::Ref<Molecular::Texture2D> m_texture;

    glm::vec3 m_objectColor = { 0.5f, 0.0f, 0.0f };
};
