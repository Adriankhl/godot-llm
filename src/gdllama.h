#ifndef GDLLAMA_H
#define GDLLAMA_H

#include "llama_runner.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/object_id.hpp>
#include <common/common.h>
#include <llama.h>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

namespace godot {
    class GDLlama : public Node {
        GDCLASS(GDLlama, Node)

        private:
            gpt_params params;
            std::string reverse_prompt;
            std::mutex generate_text_mutex;
            std::unique_ptr<LlamaRunner> llama_runner;
            String generate_text_internal(String prompt);

        protected:
    	    static void _bind_methods();
        
        public:
            GDLlama();
            ~GDLlama();

            String get_model_path() const;
            void set_model_path(const String p_model_path);

            bool get_instruct() const;
            void set_instruct(const bool p_instruct);

            bool get_interactive() const;
            void set_interactive(const bool p_interactive);

            String get_reverse_prompt() const;
            void set_reverse_prompt(const String p_reverse_prompt);

            String get_input_prefix() const;
            void set_input_prefix(const String p_input_prefix);

            String get_input_suffix() const;
            void set_input_suffix(const String p_input_suffix);

            int32_t get_n_ctx() const;
            void set_n_ctx(const int32_t p_n_ctx);

            int32_t get_n_predict() const;
            void set_n_predict(const int32_t p_n_predict);

            int32_t get_n_keep() const;
            void set_n_keep(const int32_t p_n_keep);

            float get_temperature() const;
            void set_temperature(const float p_temperature);

            int32_t get_n_gpu_layer() const;
            void set_n_gpu_layer(const int32_t p_n_gpu_layers);

            bool get_escape() const;
            void set_escape(const bool p_escape);

            int32_t get_n_threads() const;
            void set_n_threads(const int32_t n_threads);

            int32_t get_n_batch() const;
            void set_n_batch(const int32_t p_n_batch);

            int32_t get_n_ubatch() const;
            void set_n_ubatch(const int32_t p_n_ubatch);

            String generate_text(String prompt);
            String generate_text_grammar(String prompt, String grammar);
            String generate_text_json(String prompt, String json);
            void stop_generate_text();
            void input_text(String input);
    };
}

#endif