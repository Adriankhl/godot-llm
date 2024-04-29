#ifndef GDLLAMA_H
#define GDLLAMA_H

#include <cstdint>
#include <godot_cpp/classes/node.hpp>
#include <common/common.h>
#include <llama.h>

namespace godot {
    class GDLlama : public Node {
        GDCLASS(GDLlama, Node)

        private:
            gpt_params params;


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

            int32_t get_n_predict() const;
            void set_n_predict(const int32_t p_n_predict);

            int32_t get_n_threads() const;
            void set_n_threads(const int32_t n_threads);

            String generate_text(String prompt);
    };
}

#endif