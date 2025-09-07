#pragma once
#include "EditorCamera.h"
#include "OrthographicCamera.h"

namespace Molecular
{

    class Renderer3D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const EditorCamera& camera);
        static void EndScene();

        static void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color);
        static void DrawCube(const glm::vec3& position, float size, const glm::vec4& color);
        static void DrawBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
    };

}

