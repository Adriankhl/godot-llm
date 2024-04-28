#include "common.h"
#include <functional>

std::string llama_generate_text(std::string prompt, gpt_params* params, std::function<void(std::string)> emit_signal){
    params->prompt = prompt;

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
        return std::string("Error: prompt is too long. ");
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