#include "gdllama.h"
#include "common/common.h"
#include "llama.h"
#include <godot_cpp/variant/string.hpp>
#include <iostream>
#include <ostream>
#include <string>

namespace godot {
    void GDLlama::_bind_methods() {
    	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlama::get_model_path);
    	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlama::set_model_path);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::STRING, "model_path"), "set_model_path", "get_model_path");
    }

    GDLlama::GDLlama() {
        llama_backend_init();
        params = new gpt_params();
        //llama_numa_init(g_params->numa);
    }

    GDLlama::~GDLlama() {

    }

    String GDLlama::get_model_path() const {
        return String(params->model.c_str());
    }

    void GDLlama::set_model_path(const String p_model_path) {
        params->model = std::string(p_model_path.utf8().get_data());
    }
}