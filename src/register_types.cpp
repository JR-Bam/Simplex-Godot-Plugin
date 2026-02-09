#include "register_types.hpp"
#include "Simplex.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_simplex_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	printf("[SimplexNoise] Initializing module...\n");
	GDREGISTER_CLASS(Simplex);
	printf("[SimplexNoise] Module initialized.\n");
}

void uninitialize_simplex_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}
}

extern "C" {
    // Initialization.
GDExtensionBool GDE_EXPORT simplexnoise(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_simplex_module);
    init_obj.register_terminator(uninitialize_simplex_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_CORE);

    return init_obj.init();
}
}