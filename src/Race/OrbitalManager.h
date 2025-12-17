#pragma once

#include "../Camera/BaseCamera.h"
#include "../Scene/GlobalLight.h"

namespace OpenNFS {
    class OrbitalManager {
    public:
        explicit OrbitalManager();
        void Update(const BaseCamera &camera, float timeScaleFactor);
        [[nodiscard]] GlobalLight* GetActiveGlobalLight() const;

    private:
        const float SKYDOME_RADIUS {200.f};

        std::unique_ptr<GlobalLight> m_sun;
        std::unique_ptr<GlobalLight> m_moon;
        float m_currentAngle{0.f};
    };
}
