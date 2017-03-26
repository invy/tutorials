project(waylandcompositor)
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

## gcc -o wayland-compositor wayland-compositor.c backend-x11.c xdg-shell.c -lwayland-server -lX11 -lEGL -lGL -lX11-xcb -lxkbcommon-x11 -lxkbcommon

set(SRC
    Texture.cpp
    X11backend.cpp
    DrmGbmBackend.cpp
    XdgShell.cpp
    WaylandCompositor.cpp
)

add_executable(wayland-compositor ${SRC})

include_directories(/usr/include/libdrm)

target_link_libraries(wayland-compositor wayland-server X11 EGL GL X11-xcb xkbcommon-x11 xkbcommon drm gbm)

