#ifndef REGISTER_TYPES_H
#define REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

namespace godot {

void initialize_llm_module(ModuleInitializationLevel p_level);
void uninitialize_llm_module(ModuleInitializationLevel p_level);

} //namespace godot

#endif // GDEXAMPLE_REGISTER_TYPES_H
