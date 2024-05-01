#ifndef LLAMA_FUNCTIONS_H
#define LLAMA_FUNCTIONS_H

#include "common.h"
#include <functional>
#include <string>

class LlamaRunner {
    private:
        bool should_stop_generation;
        bool is_waiting_input;
        std::string input;

    public:
        LlamaRunner();
        ~LlamaRunner();
        static bool file_exists(const std::string &path);
        static bool file_is_empty(const std::string &path);
        static void llama_log_callback_logTee(ggml_log_level level, const char * text, void * user_data);
        std::string llama_generate_text(
            std::string prompt,
            gpt_params params,
            std::function<void(std::string)> on_generate_text_updated,
            std::function<void()> on_input_wait_started
        );
        void llama_stop_generate_text();
        void set_input(std::string input);
};

#endif