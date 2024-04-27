#ifndef GDLLAMA_H
#define GDLLAMA_H

#include <cstdint>
#include <godot_cpp/classes/node.hpp>
#include <common/common.h>
#include <llama.h>
#include <string>

namespace godot {
    class GDLlama : public Node {
        GDCLASS(GDLlama, Node)

        private:
            llama_context            * ctx;
            llama_model              * model;
            gpt_params               * params;
            std::vector<llama_token> * input_tokens;
            std::ostringstream       * output_ss;
            std::vector<llama_token> * output_tokens;
            bool is_interacting = false;


        protected:
    	    static void _bind_methods();
        
        public:
            GDLlama();
            ~GDLlama();
            String get_model_path() const;
            void set_model_path(const String p_model_path);
            int32_t get_n_gpu_layer() const;
            void set_n_gpu_layer(const int32_t p_n_gpu_layers);
            bool get_escape() const;
            void set_escape(const bool p_escape);
    };
}

#endif