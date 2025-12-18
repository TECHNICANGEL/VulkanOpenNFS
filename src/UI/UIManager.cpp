#include "UIManager.h"

#include <json/json.hpp>

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
        RegisterOnClick("ONFS_LOGO_CLICK", []() { LOG(INFO) << "Clicked onfsLogo"; });

        // TODO: Load these from JSON too
        auto testButton = std::make_unique<UIButton>(m_menuResourceMap["onfsLogo"], "TEST", glm::vec4(0.5, 0.5, 0, 1), 0.1f, 2,
                                                     glm::vec2(Config::get().resX / 2, Config::get().resY / 2));
        auto onfsLogoImage = std::make_unique<UIImage>(m_menuResourceMap["onfsLogo"], 0.1f, 0, glm::vec2(Config::get().resX - 75, 5));
        auto onfsVersionText = std::make_unique<UITextField>("OpenNFS v" + ONFS_VERSION + " Pre Alpha", glm::vec4(0.6, 0.6, 0.6, 1.0), 0.2f,
                                                             0, glm::vec2(Config::get().resX - 270, 35));

        onfsLogoImage->SetOnClick(GetOnClick("ONFS_LOGO_CLICK"));
        // m_uiElements.push_back(std::move(testButton));
        m_uiElements.push_back(std::move(onfsLogoImage));
        m_uiElements.push_back(std::move(onfsVersionText));

        LoadUI("../resources/ui/menu/layout/mainMenu.json");
    }
    UIManager::~UIManager() {
        for (auto &resourcePair : m_menuResourceMap) {
            glDeleteTextures(1, &resourcePair.second.textureID);
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

    void UIManager::RegisterOnClick(std::string name, OnClickCallback callback) {
        m_onClickCallbacks[name] = callback;
    }

    UIManager::OnClickCallback UIManager::GetOnClick(std::string name) {
        if (m_onClickCallbacks.count(name)) {
            return m_onClickCallbacks[name];
        }
        return []() {};
    }

    void UIManager::LoadUI(std::string const &layoutFile) {
        using json = nlohmann::json;

        std::ifstream jsonFile(layoutFile);
        if (!jsonFile.is_open()) {
            LOG(WARNING) << "Couldn't open UI layout file " << layoutFile;
            return;
        }

        json layoutJson;
        jsonFile >> layoutJson;

        auto parseElement = [&](json const &el) {
            // Determine type. Simple heuristic for now based on fields.
            bool isButton = el.contains("text") && el.contains("idleAsset");
            // bool isImage = !el.contains("text") && el.contains("resource"); // Example
            // bool isText = el.contains("text") && !el.contains("idleAsset"); // Example

            // Parse common properties
            float x = 0;
            float y = 0;
            if (el.contains("position")) {
                x = el["position"]["x"];
                y = el["position"]["y"];
            }
            float scale = el.value("scale", 1.0f);
            uint32_t layer = el.value("layer", 0u);

            if (isButton) {
                std::string text = el["text"];
                std::string textColorStr = el.value("textColour", "#FFFFFF");
                // TODO: Parse hex color to vec4
                glm::vec4 textColour(1.0f); // Default white

                std::string idleAsset = el["idleAsset"];
                // Look up resource
                // Note: The resource map keys in LoadResources come from "name" field in resources.json
                // We assume idleAsset matches one of those names.
                // But m_menuResourceMap uses names from resources.json.
                // We need to match PILL_BUTTON to a resource.

                // Assuming PILL_BUTTON is a key in m_menuResourceMap if it was loaded.
                // If not, we fallback or skip.
                if (m_menuResourceMap.count(idleAsset)) {
                     auto button = std::make_unique<UIButton>(
                        m_menuResourceMap.at(idleAsset),
                        text,
                        textColour,
                        scale,
                        layer,
                        glm::vec2(x, y)
                    );

                    if (el.contains("action")) {
                        std::string action = el["action"];
                        button->SetOnClick(GetOnClick(action));
                    }

                    m_uiElements.push_back(std::move(button));
                } else {
                    LOG(WARNING) << "Resource " << idleAsset << " not found for button";
                }
            }
        };

        if (layoutJson.is_array()) {
            for (auto &el : layoutJson) {
                parseElement(el);
            }
        } else if (layoutJson.is_object()) {
            parseElement(layoutJson);
        }

        jsonFile.close();
    }
} // namespace OpenNFS
