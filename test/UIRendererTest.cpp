#include "gtest/gtest.h"
#include "../src/Config.h"
#include <algorithm>

using namespace OpenNFS;

// Verify that the constants are defined and have expected values
TEST(ConfigTest, VerifyDefaultResolutionConstants) {
    EXPECT_EQ(DEFAULT_X_RESOLUTION, 2560);
    EXPECT_EQ(DEFAULT_Y_RESOLUTION, 1600);
}

// Verify the scaling logic logic (mimicking UIRenderer implementation)
TEST(UIRendererLogicTest, VerifyScalingMath) {
    uint32_t currentResX = 1280;
    uint32_t currentResY = 800;

    float ratioX = static_cast<float>(currentResX) / static_cast<float>(DEFAULT_X_RESOLUTION);
    float ratioY = static_cast<float>(currentResY) / static_cast<float>(DEFAULT_Y_RESOLUTION);
    float scale = std::min(ratioX, ratioY);

    EXPECT_FLOAT_EQ(scale, 0.5f);

    // Test aspect ratio preservation (widescreen)
    currentResX = 1920; // 0.75 ratio
    currentResY = 800;  // 0.5 ratio

    ratioX = static_cast<float>(currentResX) / static_cast<float>(DEFAULT_X_RESOLUTION);
    ratioY = static_cast<float>(currentResY) / static_cast<float>(DEFAULT_Y_RESOLUTION);
    scale = std::min(ratioX, ratioY);

    EXPECT_FLOAT_EQ(scale, 0.5f); // Should be constrained by Y
}
