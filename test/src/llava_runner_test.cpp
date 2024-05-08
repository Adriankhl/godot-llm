#include "../../src/llava_runner.h"
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

    std::unique_ptr<LlavaRunner> lr {new LlavaRunner()};
    std::cout << "Created llava runner" << std::endl;

    std::string prompt = "Provide a full description";
    std::cout << "Prompt: " << prompt << std::endl;

    std::string image_base64 = "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAIAAAD8GO2jAAAARklEQVR4nGLpPPSRgRSw2aaBJPVMJKkmA4xaMGrBqAWjFoxaQA3A+NnMjCQN72pkSVI/9INo1IJRC0YtGLVgRFgACAAA//8j+AbTk2+QtAAAAABJRU5ErkJggg==";
    std::cout << "Image base64: " << image_base64 << std::endl;

    gpt_params params1 {gpt_params()};
    params1.model = "../../../models/llava-phi-3-mini-int4.gguf";
    params1.mmproj = "../../../models/llava-phi-3-mini-mmproj-f16.gguf";

    std::cout << "Model: " << params1.model << std::endl;
    std::cout << "mmproj: " << params1.mmproj << std::endl;

    std::string text1 = lr->llava_generate_text_base64(prompt, image_base64, params1, [](auto a) {}, [](auto a) {});
    std::cout << "Generated text: " << text1 << std::endl;

    if (text1.empty()) {
        // Empty string
        return 1;
    }

    return 0;
}