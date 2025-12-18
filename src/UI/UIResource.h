#pragma once

#include "../Util/ImageLoader.h"

#ifndef __ANDROID__
#include <GL/glew.h>
#else
#include <GLES3/gl3.h>
#endif
#include <map>
#include <string>

namespace OpenNFS {
    class UIResource {
      public:
        static std::map<std::string, UIResource> LoadResources(std::string const &resourceFile);

        GLuint textureID;
        int width;
        int height;
    };
} // namespace OpenNFS
