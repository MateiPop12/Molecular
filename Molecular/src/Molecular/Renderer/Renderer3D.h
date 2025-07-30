#pragma once
#include "OrthographicCamera.h"

namespace Molecular
{

    class Renderer3D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();

        static void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color);
    };

}

