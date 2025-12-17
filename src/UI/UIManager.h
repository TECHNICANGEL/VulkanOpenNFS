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
        using OnClickCallback = std::function<void()>;

        UIManager();
        ~UIManager();
        void Update(InputManager::Inputs const &inputs);

        void RegisterOnClick(std::string name, OnClickCallback callback);
        OnClickCallback GetOnClick(std::string name);

        void LoadUI(std::string const &layoutFile);

        // TODO: Temporary structure
        std::vector<std::unique_ptr<UIElement>> m_uiElements;

      private:
        void LoadUIElements(std::string const &layoutFile);
        float ParseCoordinate(const std::string &coord, float maxDimension);

        std::map<std::string, UIResource> m_menuResourceMap;
        std::map<std::string, OnClickCallback> m_onClickCallbacks;
        UIRenderer m_uiRenderer;
    };
} // namespace OpenNFS
