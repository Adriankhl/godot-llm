#ifndef LLAMA_EMBEDDDING_RUNNER_H
#define LLAMA_EMBEDDDING_RUNNER_H

#include "common.h"
#include "llama.h"
#include <string>
#include <vector>
class LlamaEmbeddingRunner {
    private:
        static std::vector<std::string> split_lines(const std::string & s);
        static void batch_add_seq(llama_batch & batch, const std::vector<int32_t> & tokens, int seq_id);
        static void batch_decode(llama_context * ctx, llama_batch & batch, float * output, int n_seq, int n_embd);
    public:
        LlamaEmbeddingRunner();
        ~LlamaEmbeddingRunner();
        std::vector<float> compute_embedding(std::string prompt, gpt_params params);
};

#endif //LLAMA_EMBEDDDING_RUNNER_H