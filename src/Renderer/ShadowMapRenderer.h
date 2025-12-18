#pragma once

#ifndef __ANDROID__
#include <GL/glew.h>
#else
#include <GLES3/gl3.h>
#endif

#include "../Scene/GlobalLight.h"
#include "../Scene/Track.h"
#include "../Shaders/DepthShader.h"
#include "../Race/Agents/CarAgent.h"

namespace OpenNFS {
    class ShadowMapRenderer {
    public:
        ShadowMapRenderer();
        ~ShadowMapRenderer();
        void Render(float nearPlane,
                    float farPlane,
                    const GlobalLight *light,
                    GLuint trackTextureArrayID,
                    const std::vector<std::shared_ptr<Entity>> &visibleEntities,
                    const std::vector<std::shared_ptr<CarAgent>> &racers);

        GLuint m_depthTextureID = 0;
        DepthShader m_depthShader;

    private:
        GLuint m_fboDepthMap = 0;
    };
} // namespace OpenNFS
