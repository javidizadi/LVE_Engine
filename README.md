# Simple Game Engine

A Vulkan-based game engine.

## Prerequisites

Before building, ensure all required dependencies are installed. If you are using Arch Linux, you can install them via `pacman`:

```sh
sudo pacman -S glfw doctest vulkan-headers

```

*(Note: If you are using a different Linux distribution or operating system, install the equivalent packages using your system's package manager.)*

## Build and Run

To compile the project, run the included makefile:

```sh
make build

```

Once the build is complete, you can start the engine by running the executable:

```sh
./build/LVE_Engine

```

