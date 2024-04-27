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

    	ClassDB::bind_method(D_METHOD("get_n_threads"), &GDLlama::get_n_threads);
    	ClassDB::bind_method(D_METHOD("set_n_threads", "p_n_threads"), &GDLlama::set_n_threads);
        ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_threads", PROPERTY_HINT_NONE), "set_n_threads", "get_n_threads");
    }

    GDLlama::GDLlama() {
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

    int32_t GDLlama::get_n_threads() const {
        return params->n_threads;
    }

    void GDLlama::set_n_threads(const int32_t p_n_threads) {
        params->n_threads = p_n_threads;
    }

    String GDLlama::generate_text(String prompt) {
        params->prompt = std::string(prompt.utf8().get_data());

        llama_backend_init();

        llama_sampling_params & sparams = params->sparams;
        llama_numa_init(params->numa);

        llama_model * model;
        llama_context * ctx;
        std::tie(model, ctx) = llama_init_from_gpt_params(*params);

        const int n_ctx_train = llama_n_ctx_train(model);
        const int n_ctx = llama_n_ctx(ctx);

        const bool add_bos = llama_should_add_bos_token(model);
        GGML_ASSERT(llama_add_eos_token(model) != 1);

        std::string path_session = params->path_prompt_cache;

        std::vector<llama_token> embd_inp;
        embd_inp = ::llama_tokenize(ctx, params->prompt, true, true);

        // Should not run without any tokens
        if (embd_inp.empty()) {
            embd_inp.push_back(llama_token_bos(model));
        }

        if ((int) embd_inp.size() > n_ctx - 4) {
            return String("Error: prompt is too long. ");
        }

        // debug message about similarity of saved session, if applicable
        size_t n_matching_session_tokens = 0;

        // number of tokens to keep when resetting context
        if (params->n_keep < 0 || params->n_keep > (int) embd_inp.size() || params->instruct || params->chatml) {
            params->n_keep = (int)embd_inp.size();
        } else {
            params->n_keep += add_bos; // always keep the BOS token
        }

        // prefix & suffix for instruct mode
        const auto inp_pfx = ::llama_tokenize(ctx, "\n\n### Instruction:\n\n", true,  true);
        const auto inp_sfx = ::llama_tokenize(ctx, "\n\n### Response:\n\n",    false, true);
        
        // group-attention state
        // number of grouped KV tokens so far (used only if params.grp_attn_n > 1)
        int ga_i = 0;

        const int ga_n = params->grp_attn_n;
        const int ga_w = params->grp_attn_w;

        if (ga_n != 1) {
            GGML_ASSERT(ga_n > 0                    && "grp_attn_n must be positive");                     // NOLINT
            GGML_ASSERT(ga_w % ga_n == 0            && "grp_attn_w must be a multiple of grp_attn_n");     // NOLINT
        }

        bool is_antiprompt        = false;
        bool input_echo           = true;
        bool display              = true;
        bool need_to_save_session = !path_session.empty() && n_matching_session_tokens < embd_inp.size();

        int n_past             = 0;
        int n_remain           = params->n_predict;
        int n_consumed         = 0;
        int n_session_consumed = 0;
        int n_past_guidance    = 0;

        std::vector<int>   input_tokens; 
        std::vector<int>   output_tokens;
        std::ostringstream output_ss;    

        display = params->display_prompt;

        std::vector<llama_token> embd;

        // tokenized antiprompts
        std::vector<std::vector<llama_token>> antiprompt_ids;

        antiprompt_ids.reserve(params->antiprompt.size());
        for (const std::string & antiprompt : params->antiprompt) {
            antiprompt_ids.emplace_back(::llama_tokenize(ctx, antiprompt, false, true));
        }

        struct llama_sampling_context * ctx_sampling = llama_sampling_init(sparams);
    }
}