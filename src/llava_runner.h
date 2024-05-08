#ifndef LLAVA_RUNNER_H
#define LLAVA_RUNNER_H

#include "common.h"
#include "llama.h"
#include "llava.h"
#include <string>
#include <vector>

struct llava_context {
    struct clip_ctx * ctx_clip = NULL;
    struct llama_context * ctx_llama = NULL;
    struct llama_model * model = NULL;
};

class LlavaRunner {
    private:
        static bool eval_tokens(struct llama_context * ctx_llama, std::vector<llama_token> tokens, int n_batch, int * n_past);
        static bool eval_id(struct llama_context * ctx_llama, int id, int * n_past);
        static bool eval_string(struct llama_context * ctx_llama, const char* str, int n_batch, int * n_past, bool add_bos);
        static const char * sample(
            struct llama_sampling_context * ctx_sampling,
            struct llama_context * ctx_llama,
            int * n_past
        );
        static void find_image_tag_in_prompt(const std::string& prompt, size_t& begin_out, size_t& end_out);
        static bool prompt_contains_image(const std::string& prompt);
        static llava_image_embed * llava_image_embed_make_with_prompt_base64(struct clip_ctx * ctx_clip, int n_threads, const std::string& prompt);
        static std::string remove_image_from_prompt(const std::string& prompt, const char * replacement = "");
        static void show_additional_info(int /*argc*/, char ** argv);
        static struct llava_image_embed * load_image(llava_context * ctx_llava, gpt_params * params, const std::string & fname);
        static void process_prompt(struct llava_context * ctx_llava, struct llava_image_embed * image_embed, gpt_params * params, const std::string & prompt);
        static struct llama_model * llava_init(gpt_params * params);
        static struct llava_context * llava_init_context(gpt_params * params, llama_model * model);
        static void llava_free(struct llava_context * ctx_llava);
        static void llama_log_callback_logTee(ggml_log_level level, const char * text, void * user_data);
        

    public:
        LlavaRunner();
        ~LlavaRunner();
        std::string llava_generate_text();
};

#endif //LLAVA_RUNNER_H