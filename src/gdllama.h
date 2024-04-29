#ifndef GDLLAMA_H
#define GDLLAMA_H

#include "llama_runner.h"
#include <cstdint>
#include <godot_cpp/classes/node.hpp>
#include <common/common.h>
#include <llama.h>
#include <memory>
#include <mutex>

namespace godot {
    class GDLlama : public Node {
        GDCLASS(GDLlama, Node)

        private:
            gpt_params params;
            std::mutex generate_text_mutex;
            std::unique_ptr<LlamaRunner> llama_runner;


        protected:
    	    static void _bind_methods();
        
        public:
            GDLlama();
            ~GDLlama();

            String get_model_path() const;
            void set_model_path(const String p_model_path);

            int32_t get_n_ctx() const;
            void set_n_ctx(const int32_t p_n_ctx);

            int32_t get_n_batch() const;
            void set_n_batch(const int32_t p_n_batch);

            int32_t get_n_ubatch() const;
            void set_n_ubatch(const int32_t p_n_ubatch);

            int32_t get_n_gpu_layer() const;
            void set_n_gpu_layer(const int32_t p_n_gpu_layers);

            bool get_escape() const;
            void set_escape(const bool p_escape);

            int32_t get_n_predict() const;
            void set_n_predict(const int32_t p_n_predict);

            int32_t get_n_threads() const;
            void set_n_threads(const int32_t n_threads);

            String generate_text(String prompt);
            void stop_generate_text();
    };
}

#endif