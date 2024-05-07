#include "../../src/llama_embedding_runner.h"
#include <iostream>
#include <ostream>
#include <vector>

int main(int argc, char ** argv) {
    std::cout << "Start testing: " << std::endl;
    std::unique_ptr<LlamaEmbeddingRunner> ler {new LlamaEmbeddingRunner()};
    std::cout << "Created llama embedding runner" << std::endl;

    std::string prompt = "Godot engine";
    std::cout << "Prompt: " << prompt << std::endl;

    gpt_params params {gpt_params()};
    params.model = "../../../models/Meta-Llama-3-8B-Instruct.Q5_K_M.gguf";
    std::cout << "Model: " << params.model << std::endl;

    std::vector<float> v = ler->compute_embedding(prompt, params);
    std::cout << "Vector: "  << std::endl;
    for (float f: v) {
        std::cout << f << " ";
    }
    std::cout << std::endl;

    return 0;
}