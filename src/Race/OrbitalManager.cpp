#include "OrbitalManager.h"
#include "glm/gtc/constants.hpp"

namespace OpenNFS {
    OrbitalManager::OrbitalManager()
        : m_sun(std::make_unique<GlobalLight>(glm::vec3(0, 0, 0), glm::vec3(0, SKYDOME_RADIUS, 0))),
          m_moon(std::make_unique<GlobalLight>(glm::vec3(0, 0, 0), glm::vec3(0, -SKYDOME_RADIUS, 0))) {
    }

    void OrbitalManager::Update(BaseCamera const &camera, float const timeScaleFactor) {
        m_sun->ChangeTarget(camera.position);
        m_sun->Update(timeScaleFactor);

        m_moon->ChangeTarget(camera.position);
        m_moon->Update(timeScaleFactor);

        m_currentAngle += timeScaleFactor * RADIANS_PER_TICK;
        if (m_currentAngle > glm::two_pi<float>()) {
            m_currentAngle -= glm::two_pi<float>();
        }
    }

    GlobalLight *OrbitalManager::GetActiveGlobalLight() const {
        // Switch this over to be time based
        if (m_currentAngle >= glm::half_pi<float>() && m_currentAngle <= glm::three_over_two_pi<float>()) {
            return m_moon.get();
        }

        return m_sun.get();
    }
} // namespace OpenNFS
