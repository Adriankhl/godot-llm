#ifndef GDLLAMA_H
#define GDLLAMA_H

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
            void set_model_path(const String model_path);
    };
}

#endif