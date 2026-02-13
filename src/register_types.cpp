#include "register_types.hpp"
#include "Simplex.hpp"
#include "SimplexTexture.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_simplex_module(ModuleInitializationLevel p_level) {
    // Register Simplex at CORE level
    if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
        printf("[SimplexNoise] Initializing CORE level...\n");
        GDREGISTER_CLASS(Simplex);
        printf("[SimplexNoise] Registered Simplex at CORE level\n");
    }
    
    // Register SimplexTexture at SCENE level
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        printf("[SimplexNoise] Initializing SCENE level...\n");
        GDREGISTER_CLASS(SimplexTexture);
        printf("[SimplexNoise] Registered SimplexTexture at SCENE level\n");
    }
}

void uninitialize_simplex_module(ModuleInitializationLevel p_level) {
    // Cleanup at appropriate levels
    if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
        printf("[SimplexNoise] Uninitializing CORE level...\n");
    }
    
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        printf("[SimplexNoise] Uninitializing SCENE level...\n");
    }
}

extern "C" {
GDExtensionBool GDE_EXPORT simplexnoise(GDExtensionInterfaceGetProcAddress p_get_proc_address, 
                                        GDExtensionClassLibraryPtr p_library, 
                                        GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_simplex_module);
    init_obj.register_terminator(uninitialize_simplex_module);
    
    // Set minimum level to CORE since we need to register Simplex at CORE level
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_CORE);

    return init_obj.init();
}
}