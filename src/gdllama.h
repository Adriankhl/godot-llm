#ifndef GDLLAMA_H
#define GDLLAMA_H

#include <godot_cpp/classes/node.hpp>
#include <common/common.h>
#include <llama.h>

namespace godot {
    class GDLlama : public Node {
        GDCLASS(GDLlama, Node)

        private:
            llama_context           ** g_ctx;
            llama_model             ** g_model;
            gpt_params               * g_params;
            std::vector<llama_token> * g_input_tokens;
            std::ostringstream       * g_output_ss;
            std::vector<llama_token> * g_output_tokens;
            bool is_interacting = false;


        protected:
    	    static void _bind_methods();
        
        public:
            GDLlama();
            ~GDLlama();
    };
}

#endif