#include "gdllama.h"
#include "common/common.h"
#include "llama_runner.h"
#include <cstdint>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <memory>
#include <mutex>
#include <string>

namespace godot {
    void GDLlama::_bind_methods() {
    	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlama::get_model_path);
    	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlama::set_model_path);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

       	ClassDB::bind_method(D_METHOD("get_n_ctx"), &GDLlama::get_n_ctx);
    	ClassDB::bind_method(D_METHOD("set_n_ctx", "p_n_ctx"), &GDLlama::set_n_ctx);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "context_size", PROPERTY_HINT_NONE), "set_n_ctx", "get_n_ctx");

       	ClassDB::bind_method(D_METHOD("get_n_batch"), &GDLlama::get_n_batch);
    	ClassDB::bind_method(D_METHOD("set_n_batch", "p_n_batch"), &GDLlama::set_n_batch);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_batch", PROPERTY_HINT_NONE), "set_n_batch", "get_n_batch");

       	ClassDB::bind_method(D_METHOD("get_n_ubatch"), &GDLlama::get_n_ubatch);
    	ClassDB::bind_method(D_METHOD("set_n_ubatch", "p_n_ubatch"), &GDLlama::set_n_ubatch);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_ubatch", PROPERTY_HINT_NONE), "set_n_ubatch", "get_n_ubatch");

        ClassDB::bind_method(D_METHOD("get_n_threads"), &GDLlama::get_n_threads);
        ClassDB::bind_method(D_METHOD("set_n_threads", "p_n_threads"), &GDLlama::set_n_threads);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_threads", PROPERTY_HINT_NONE), "set_n_threads", "get_n_threads");

    	ClassDB::bind_method(D_METHOD("get_n_gpu_layer"), &GDLlama::get_n_gpu_layer);
    	ClassDB::bind_method(D_METHOD("set_n_gpu_layer", "p_n_gpu_layer"), &GDLlama::set_n_gpu_layer);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_gpu_layer", PROPERTY_HINT_NONE), "set_n_gpu_layer", "get_n_gpu_layer");

    	ClassDB::bind_method(D_METHOD("get_escape"), &GDLlama::get_escape);
    	ClassDB::bind_method(D_METHOD("set_escape", "p_escape"), &GDLlama::set_escape);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "escape", PROPERTY_HINT_NONE), "set_escape", "get_escape");

    	ClassDB::bind_method(D_METHOD("get_n_predict"), &GDLlama::get_n_predict);
    	ClassDB::bind_method(D_METHOD("set_n_predict", "p_n_predict"), &GDLlama::set_n_predict);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_predict", PROPERTY_HINT_NONE), "set_n_predict", "get_n_predict");

        ClassDB::bind_method(D_METHOD("get_temperature"), &GDLlama::get_temperature);
        ClassDB::bind_method(D_METHOD("set_temperature", "p_temperature"), &GDLlama::set_temperature);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::FLOAT, "temperature", PROPERTY_HINT_NONE), "set_temperature", "get_temperature");

        ClassDB::bind_method(D_METHOD("generate_text", "prompt"), &GDLlama::generate_text);
        ClassDB::bind_method(D_METHOD("stop_generate_text"), &GDLlama::stop_generate_text);


        ADD_SIGNAL(MethodInfo("generate_text_updated", PropertyInfo(Variant::STRING, "new_text")));
    }

    GDLlama::GDLlama() : params {gpt_params()},
        llama_runner {new LlamaRunner()}
    {}

    GDLlama::~GDLlama() {}

    String GDLlama::get_model_path() const {
        return String(params.model.c_str());
    }

    void GDLlama::set_model_path(const String p_model_path) {
        params.model = std::string(p_model_path.trim_prefix(String("res://")).utf8().get_data());
    }

    int32_t GDLlama::get_n_ctx() const {
        return params.n_ctx;
    }

    void GDLlama::set_n_ctx(const int32_t p_n_ctx) {
        params.n_ctx = p_n_ctx;
    }

    int32_t GDLlama::get_n_batch() const {
        return params.n_batch;
    }

    void GDLlama::set_n_batch(const int32_t p_n_batch) {
        params.n_batch = p_n_batch;
    }

    int32_t GDLlama::get_n_ubatch() const {
        return params.n_ubatch;
    }

    void GDLlama::set_n_ubatch(const int32_t p_n_ubatch) {
        params.n_ubatch = p_n_ubatch;
    }

    int32_t GDLlama::get_n_threads() const {
        return params.n_threads;
    }

    void GDLlama::set_n_threads(const int32_t p_n_threads) {
        params.n_threads = p_n_threads;
    }

    int32_t GDLlama::get_n_gpu_layer() const {
        return params.n_gpu_layers;
    }

    void GDLlama::set_n_gpu_layer(const int32_t p_n_gpu_layers) {
        params.n_gpu_layers = p_n_gpu_layers;
    }

    bool GDLlama::get_escape() const {
        return params.escape;
    }

    void GDLlama::set_escape(const bool p_escape) {
        params.escape = p_escape;
    }

    int32_t GDLlama::get_n_predict() const {
        return params.n_predict;
    }

    void GDLlama::set_n_predict(const int32_t p_n_predict) {
        params.n_predict = p_n_predict;
    }

    float GDLlama::get_temperature() const {
        return params.sparams.temp;
    }

    void GDLlama::set_temperature(const float p_temperature) {
        params.sparams.temp = p_temperature;
    }

    String GDLlama::generate_text(String prompt) {
        std::lock_guard<std::mutex> guard(generate_text_mutex);

        llama_runner.reset(new LlamaRunner());

        std::string text = llama_runner->llama_generate_text(
            std::string(prompt.utf8().get_data()),
                params,
                [this](std::string s) {
                    String new_text {s.c_str()};
                    call_deferred("emit_signal", "generate_text_updated", new_text);
                }
        );

        return String(text.c_str());
    }

    void GDLlama::stop_generate_text() {
        llama_runner->llama_stop_generate_text();
    }
}