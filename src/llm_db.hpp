#ifndef LLM_DB_HPP
#define LLM_DB_HPP

#include "gdembedding.hpp"
namespace godot {

class LlmDB : public GDEmbedding {
    protected:
	    static void _bind_methods();

    public:
        LlmDB();
        ~LlmDB();
};

} // namespace godot

#endif