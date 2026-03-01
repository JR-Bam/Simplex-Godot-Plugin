# Simplex-Godot-Plugin

A Godot 4.x plugin for advanced procedural noise generation using Simplex and fractal algorithms, designed for terrain, texture, and biome generation.

## Features

- Simplex noise: Fast, high-quality 1D/2D/3D/4D noise generation
- Fractal noise: FBM, Ridged, Ping-Pong, and domain-warped variants
- Godot integration: Custom resources and textures for use in Godot scenes and shaders
- Seamless textures: Generate seamless 2D/3D textures for terrain and masks
- Editor support: Inspector properties, live previews, and resource saving
- Shader support: Example shaders for biome, elevation, erosion, and humidity maps

## Directory Structure

```
demo/                # Example Godot project and assets
godot-cpp/           # Godot C++ bindings (submodule)
src/                 # Plugin source code (C++)
SConstruct           # SCons build script
LICENSE              # MIT License
```

## Getting Started

### Prerequisites

- Godot 4.x
- SCons (for building)
- C++ Compiler (GCC/Clang/MSVC)
- Python 3 (for build scripts)

### Building

1. Clone the repo (with submodules):

    ```sh
    git clone --recursive https://github.com/JR-Bam/Simplex-Godot-Plugin.git
    cd Simplex-Godot-Plugin
    ```

2. Build the plugin library:

    ```sh
    scons platform=windows target=template_debug
    # Or for Linux/macOS:
    # scons platform=linux target=template_debug
    # scons platform=macos target=template_debug
    ```

3. Open the `demo/` project in Godot.

### Usage

- Add the plugin library to your Godot project.
- Use the `Simplex` resource and `SimplexTexture`/`SimplexTexture3D` nodes for procedural generation.
- Example usage and shaders are provided in the `demo/` folder.

## API Overview

- [`Simplex`](src/Simplex.hpp): Core noise resource
- [`SimplexTexture`](src/SimplexTexture.hpp): 2D texture generator
- [`SimplexTexture3D`](src/SimplexTexture3D.hpp): 3D texture generator
- Example scripts: [demo/main.gd](demo/main.gd), [demo/terrain_masks/elevation_map/elevation_map.gd](demo/terrain_masks/elevation_map/elevation_map.gd)

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Credits

- Simplex noise implementation based on [Sebastien Rombauts](https://github.com/SRombauts/SimplexNoise)
- Godot C++ bindings from [godotengine/godot-cpp](https://github.com/godotengine/godot-cpp)

---

For more details, see the source code and demo project. Contributions and issues are welcome!
