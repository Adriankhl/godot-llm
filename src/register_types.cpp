#include "register_types.hpp"

#include "gdllama.hpp"
#include "gdembedding.hpp"
#include "gdllava.hpp"
#include "llm_db.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace godot {

LlmDBMetaData* llmDBMetaData;

void initialize_llm_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<GDLlama>();
	ClassDB::register_class<GDEmbedding>();
	ClassDB::register_class<GDLlava>();
	ClassDB::register_class<LlmDB>();
	ClassDB::register_class<LlmDBMetaData>();

	llmDBMetaData = memnew(LlmDBMetaData);
	Engine::get_singleton()->register_singleton("LlmDBMetaData", llmDBMetaData);
}

void uninitialize_llm_module(ModuleInitializationLevel p_level) {

	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	Engine::get_singleton()->unregister_singleton("LlmDBMetaData");
	memdelete(llmDBMetaData);
}

extern "C" {
	// Initialization.
	GDExtensionBool GDE_EXPORT llm_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(initialize_llm_module);
		init_obj.register_terminator(uninitialize_llm_module);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}

} // namespace godot
