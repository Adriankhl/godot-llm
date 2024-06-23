#ifndef GDLLAMA_HPP
#define GDLLAMA_HPP

#include "llama_runner.hpp"
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/core/object_id.hpp>
#include <common/common.h>
#include <llama.h>
#include <cstdint>
#include <memory>
#include <string>

namespace godot {

class GDLlama : public Node {
    GDCLASS(GDLlama, Node)

    private:
        gpt_params params;
        bool should_output_prompt;
        std::string reverse_prompt;
        std::unique_ptr<LlamaRunner> llama_runner;
        Ref<Mutex> generate_text_mutex;
        Ref<Mutex> func_mutex;
        Ref<Thread> generate_text_thread;
        String generate_text_common(String prompt);
        String generate_text_simple_internal(String prompt);
        String generate_text_grammar_internal(String prompt, String grammar);
        String generate_text_json_internal(String prompt, String json);
        std::function<void(std::string)> glog;
        std::function<void(std::string)> glog_verbose;
        std::string generate_text_buffer;

    protected:
	    static void _bind_methods();
    
    public:
        GDLlama();
        ~GDLlama();

        void _ready() override;
        void _exit_tree() override;
        String get_model_path() const;
        void set_model_path(const String p_model_path);
        bool get_interactive() const;
        void set_interactive(const bool p_interactive);
        String get_reverse_prompt() const;
        void set_reverse_prompt(const String p_reverse_prompt);
        bool get_should_output_prompt() const;
        void set_should_output_prompt(const bool p_should_output_prompt);
        bool get_should_output_special() const;
        void set_should_output_special(const bool p_should_output_special);
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
        float get_penalty_repeat() const;
        void set_penalty_repeat(const float p_penalty_repeat);
        int32_t get_penalty_last_n() const;
        void set_penalty_last_n(const int32_t p_penalty_last_n);
        bool get_penalize_nl() const;
        void set_penalize_nl(const bool p_penalize_nl);
        int32_t get_top_k() const;
        void set_top_k(const int32_t p_top_k);
        float get_top_p() const;
        void set_top_p(const float p_top_p);
        float get_min_p() const;
        void set_min_p(const float p_min_p);
        int32_t get_n_threads() const;
        void set_n_threads(const int32_t n_threads);
        int32_t get_n_gpu_layer() const;
        void set_n_gpu_layer(const int32_t p_n_gpu_layers);
        int32_t get_main_gpu() const;
        void set_main_gpu(const int32_t p_main_gpu);
        int32_t get_split_mode();
        void set_split_mode(const int32_t p_split_mode);
        bool get_escape() const;
        void set_escape(const bool p_escape);
        int32_t get_n_batch() const;
        void set_n_batch(const int32_t p_n_batch);
        int32_t get_n_ubatch() const;
        void set_n_ubatch(const int32_t p_n_ubatch);
        String generate_text(String prompt, String grammar, String json);
        String generate_text_simple(String prompt);
        String generate_text_grammar(String prompt, String grammar);
        String generate_text_json(String prompt, String json);
        Error run_generate_text(String prompt, String grammar = "", String json = "");
        bool is_running();
        bool is_waiting_input();
        void stop_generate_text();
        void input_text(String input);
};

} //namespace godot

#endif
