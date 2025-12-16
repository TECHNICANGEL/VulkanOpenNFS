#include "UIManager.h"

#include <ranges>

#include "UIButton.h"
#include "UIImage.h"
#include "UIResource.h"
#include "UITextField.h"
#include "../Util/ImageLoader.h"

#include <fstream>
#include <json/json.hpp>
#include <regex>

namespace OpenNFS {
    UIManager::UIManager() {
        m_menuResourceMap = UIResource::LoadResources("../resources/ui/menu/resources.json");
        LOG(INFO) << m_menuResourceMap.size() << " UI resources loaded successfully";
        LoadUIElements("../resources/ui/menu/layout/ui_elements.json");
    }

    float UIManager::ParseCoordinate(const std::string &coord, float maxDimension) {
        // Handle explicit numbers first
        try {
            return std::stof(coord);
        } catch (...) {}

        // Handle Expressions
        std::string expression = coord;
        static const std::regex re("(CENTER|RIGHT|LEFT|TOP|BOTTOM)\\s*([+-]?)\\s*([0-9.]*)");
        std::smatch match;

        if (std::regex_search(expression, match, re)) {
            float base = 0;
            if (match[1] == "CENTER") base = maxDimension / 2.0f;
            else if (match[1] == "RIGHT" || match[1] == "TOP") base = maxDimension;
            else if (match[1] == "LEFT" || match[1] == "BOTTOM") base = 0;

            if (match[2].length() > 0 && match[3].length() > 0) {
                float offset = std::stof(match[3]);
                if (match[2] == "+") base += offset;
                else if (match[2] == "-") base -= offset;
            }
            return base;
        }

        return 0;
    }

    void UIManager::LoadUIElements(std::string const &layoutFile) {
        using json = nlohmann::json;
        std::ifstream jsonFile(layoutFile);
        if (!jsonFile.is_open()) {
            LOG(WARNING) << "Could not open UI layout file " << layoutFile;
            return;
        }

        json layoutJson;
        jsonFile >> layoutJson;

        for (auto &el : layoutJson["elements"]) {
            std::string type = el["type"];
            float scale = el["scale"];
            uint32_t layer = el["layer"];

            // Parse Position
            float x = ParseCoordinate(el["position"]["x"].is_string() ? el["position"]["x"].get<std::string>() : std::to_string(el["position"]["x"].get<float>()), Config::get().resX);
            float y = ParseCoordinate(el["position"]["y"].is_string() ? el["position"]["y"].get<std::string>() : std::to_string(el["position"]["y"].get<float>()), Config::get().resY);
            glm::vec2 position(x, y);

            if (type == "Button") {
                std::string resourceName = el["resource"];
                std::string text = el["text"];
                glm::vec4 color(el["textColor"][0], el["textColor"][1], el["textColor"][2], el["textColor"][3]);

                if (m_menuResourceMap.count(resourceName)) {
                    m_uiElements.emplace_back(std::make_unique<UIButton>(m_menuResourceMap[resourceName], text, color, scale, layer, position));
                } else {
                    LOG(WARNING) << "Resource " << resourceName << " not found for Button";
                }
            } else if (type == "Image") {
                std::string resourceName = el["resource"];

                if (m_menuResourceMap.count(resourceName)) {
                    auto image = std::make_unique<UIImage>(m_menuResourceMap[resourceName], scale, layer, position);
                    if (el.contains("action")) {
                        std::string action = el["action"];
                        if (action == "ONFS_LOGO_CLICK") {
                            image->SetOnClick([](){LOG(INFO) << "Clicked onfsLogo";});
                        }
                    }
                    m_uiElements.emplace_back(std::move(image));
                } else {
                    LOG(WARNING) << "Resource " << resourceName << " not found for Image";
                }
            } else if (type == "TextField") {
                std::string text = el["text"];
                // Replace {VERSION}
                if (text.find("{VERSION}") != std::string::npos) {
                    text.replace(text.find("{VERSION}"), 9, ONFS_VERSION);
                }

                glm::vec4 color(el["textColor"][0], el["textColor"][1], el["textColor"][2], el["textColor"][3]);
                m_uiElements.emplace_back(std::make_unique<UITextField>(text, color, scale, layer, position));
            }
        }
    }
    UIManager::~UIManager() {
        for (auto &resource : m_menuResourceMap | std::views::values) {
            glDeleteTextures(1, &resource.textureID);
        }
    }

    auto UIManager::Update(InputManager::Inputs const &inputs) -> void {
        // Cursor Y needs to be inverted + coordinates need normalising
        float const windowToResRatioX {(float)Config::get().resX/(float)Config::get().windowSizeX};
        float const windowToResRatioY {(float)Config::get().resY/(float)Config::get().windowSizeY};
        glm::vec2 const cursorPosition{inputs.cursorX * windowToResRatioX, Config::get().resY - (inputs.cursorY * windowToResRatioY)};

        m_uiRenderer.BeginRenderPass();
        for (auto const &uiElement : m_uiElements) {
            uiElement->Update(cursorPosition, inputs.mouseLeft);

            switch (uiElement->type) {
            case UIElementType::Button:
                m_uiRenderer.RenderButton(static_cast<UIButton *>(uiElement.get()));
                break;
            case UIElementType::TextField:
                m_uiRenderer.RenderTextField(static_cast<UITextField *>(uiElement.get()));
                break;
            case UIElementType::Image:
                m_uiRenderer.RenderImage(static_cast<UIImage *>(uiElement.get()));
                break;
            }
        }
        m_uiRenderer.EndRenderPass();
    }
} // namespace OpenNFS
