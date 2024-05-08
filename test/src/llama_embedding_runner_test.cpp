#include "../../src/llama_embedding_runner.h"
#include <iostream>
#include <ostream>
#include <vector>

int main(int argc, char ** argv) {
    std::cout << "Start testing: " << std::endl;
    std::unique_ptr<LlamaEmbeddingRunner> ler {new LlamaEmbeddingRunner()};
    std::cout << "Created llama embedding runner" << std::endl;

    std::string prompt1 = "Godot engine";
    std::cout << "Prompt 1: " << prompt1 << std::endl;

    std::string prompt2 = "Unity engine";
    std::cout << "Prompt 2: " << prompt2 << std::endl;

    std::string prompt3 = "Hello world";
    std::cout << "Prompt 3: " << prompt3 << std::endl;

    gpt_params params {gpt_params()};
    params.model = "../../../models/all-MiniLM-L6-v2-Q5_K_M.gguf";
    std::cout << "Model: " << params.model << std::endl;

    std::vector<float> v1 = ler->compute_embedding(prompt1, params, [](auto a){});

    std::cout << "Vector 1: "  << std::endl;
    for (float f: v1) {
        std::cout << f << " ";
    }
    std::cout << std::endl;

    std::vector<float> v2 = ler->compute_embedding(prompt2, params, [](auto a){});

    std::cout << "Vector 2: "  << std::endl;
    for (float f: v2) {
        std::cout << f << " ";
    }
    std::cout << std::endl;

    std::vector<float> v3 = ler->compute_embedding(prompt3, params, [](auto a){});

    std::cout << "Vector 3: "  << std::endl;
    for (float f: v3) {
        std::cout << f << " ";
    }
    std::cout << std::endl;

    float s12 = ler->similarity_cos(v1, v2);
    float s23 = ler->similarity_cos(v2, v3);

    std::cout << "Similarity '" << prompt1 << "' and '" << prompt2 << "' :"  << s12  << std::endl;
    std::cout << "Similarity '" << prompt2 << "' and '" << prompt3 << "' :"  << s23  << std::endl;

    if (s12 < s23) {
        return 1;
    }

    return 0;
}