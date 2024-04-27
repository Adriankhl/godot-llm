#include "gdllama.h"
#include "llama.h"
#include <string>

namespace godot {
    void GDLlama::_bind_methods() {
    	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlama::get_model_path);
    	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlama::set_model_path);
    	ClassDB::add_property("GDLlama", PropertyInfo(Variant::STRING, "model_path"), "set_amodel_path", "get_model_path");
    }

    GDLlama::GDLlama() {
        llama_backend_init();
        //llama_numa_init(g_params->numa);
    }

    GDLlama::~GDLlama() {

    }

    std::string GDLlama::get_model_path() const {
        return params->model;
    }

    void GDLlama::set_model_path(const std::string p_model_path) {
        params->model = p_model_path;
    }
}