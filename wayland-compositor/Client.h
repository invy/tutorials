#pragma once

#include <wayland-server.h>

class Client
{
public:
    wl_client* client {nullptr};
    wl_resource* pointer {nullptr};
    wl_resource* keyboard {nullptr};
    wl_list link {nullptr, nullptr};
};
