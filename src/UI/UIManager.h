#pragma once

#include <map>
#include <string>

#include "../Input/InputManager.h"
#include "../Renderer/UIRenderer.h"
#include "UIElement.h"
#include "UIResource.h"

namespace OpenNFS {
    class UIManager {
      public:
        UIManager();
        ~UIManager();
        void Update(InputManager::Inputs const &inputs);

        // TODO: Temporary structure
        std::vector<std::unique_ptr<UIElement>> m_uiElements;

      private:
        void LoadUIElements(std::string const &layoutFile);
        float ParseCoordinate(const std::string &coord, float maxDimension);

        std::map<std::string, UIResource> m_menuResourceMap;
        UIRenderer m_uiRenderer;
    };
} // namespace OpenNFS
