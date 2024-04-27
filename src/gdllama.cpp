#include "gdllama.h"
#include "common/common.h"
#include "llama.h"
#include <cstdint>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/variant/string.hpp>
#include <iostream>
#include <ostream>
#include <string>

namespace godot {
    void GDLlama::_bind_methods() {
    	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlama::get_model_path);
    	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlama::set_model_path);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

    	ClassDB::bind_method(D_METHOD("get_n_gpu_layer"), &GDLlama::get_n_gpu_layer);
    	ClassDB::bind_method(D_METHOD("set_n_gpu_layer", "p_n_gpu_layer"), &GDLlama::set_n_gpu_layer);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_gpu_layer", PROPERTY_HINT_NONE), "set_n_gpu_layer", "get_n_gpu_layer");

    	ClassDB::bind_method(D_METHOD("get_escape"), &GDLlama::get_escape);
    	ClassDB::bind_method(D_METHOD("set_escape", "p_escape"), &GDLlama::set_escape);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "escape", PROPERTY_HINT_NONE), "set_escape", "get_escape");

    	ClassDB::bind_method(D_METHOD("get_n_predict"), &GDLlama::get_n_predict);
    	ClassDB::bind_method(D_METHOD("set_n_predict", "p_n_predict"), &GDLlama::set_n_predict);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_predict", PROPERTY_HINT_NONE), "set_n_predict", "get_n_predict");
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

    int32_t GDLlama::get_n_gpu_layer() const {
        return params->n_gpu_layers;
    }

    void GDLlama::set_n_gpu_layer(const int32_t p_n_gpu_layers) {
        params->n_gpu_layers = p_n_gpu_layers;
    }

    bool GDLlama::get_escape() const {
        return params->escape;
    }

    void GDLlama::set_escape(const bool p_escape) {
        params->escape = p_escape;
    }

    int32_t GDLlama::get_n_predict() const {
        return params->n_predict;
    }

    void GDLlama::set_n_predict(const int32_t p_n_predict) {
        params->n_predict = p_n_predict;
    }
}