#ifndef LLAMA_EMBEDDDING_RUNNER_HPP
#define LLAMA_EMBEDDDING_RUNNER_HPP

#include "common.h"
#include "llama.h"
#include <functional>
#include <string>
#include <vector>
class EmbeddingRunner {
    private:
        static std::vector<std::string> split_lines(const std::string & s);
        static void batch_add_seq(llama_batch & batch, const std::vector<int32_t> & tokens, int seq_id);
        static void batch_decode(llama_context * ctx, llama_batch & batch, float * output, int n_seq, int n_embd);
        std::function<void(std::string)> glog;
    public:
        EmbeddingRunner(
            std::function<void(std::string)> glog = [](auto s){}
        );
        ~EmbeddingRunner();
        std::vector<float> compute_embedding(
            std::string prompt,
            gpt_params params,
            std::function<void(std::vector<float>)> on_compute_finished
        );
        float similarity_cos(std::vector<float> embd1, std::vector<float> embd2);
};

#endif //LLAMA_EMBEDDDING_RUNNER_H
