#include "../../src/llama_runner.h"
#include "common.h"
#include <iostream>
#include <ostream>
#include <string>

int main(int argc, char ** argv) {
    //Calling godot::String produces user mode data execution error exception
    //Probably need the full engine to call godot-related classes
    //Only test non-godot classes here
    //godot::String prompt;

    std::cout << "Start testing: " << std::endl;

    std::unique_ptr<LlamaRunner> lr {new LlamaRunner()};
    std::cout << "Created llama runner" << std::endl;

    std::string prompt = "Tell me about vulkan: ";
    std::cout << "Prompt: " << prompt << std::endl;

    gpt_params params1 {gpt_params()};
    std::cout << "Model: " << params1.model << std::endl;

    std::string text1 = lr->llama_generate_text(prompt, params1, [](auto a) {}, []() {}, [](auto a){});
    std::cout << "Generated text: " << text1 << std::endl;

    if (text1 != "error: unable to load model") {
        return 1;
    }

    gpt_params params2 {gpt_params()};
    params2.model = "../../models/Meta-Llama-3-8B-Instruct.Q5_K_M.gguf";
    std::cout << "Model: " << params2.model << std::endl;
    params2.n_predict = 10;

    std::string text2 = lr->llama_generate_text(prompt, params2, [](auto a) {}, []() {}, [](auto a){});
    std::cout << "Generated text: " << text2 << std::endl;

    return 0;
}