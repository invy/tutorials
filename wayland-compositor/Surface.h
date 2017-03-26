#pragma once

#include "Texture.h"
#include "Client.h"

#include <wayland-server.h>

class Surface
{
public:
    wl_resource* surface {nullptr};
    wl_resource* xdg_surface {nullptr};
    wl_resource* buffer {nullptr};
    wl_resource* frame_callback {nullptr};
    int x {0};
    int y {0};
    Texture texture;
    Client* client {nullptr};
    wl_list link {nullptr, nullptr};
};

