#ifndef LLAMA_FUNCTIONS_H
#define LLAMA_FUNCTIONS_H

#include "common.h"
#include <functional>
#include <string>

std::string llama_generate_text(std::string prompt, gpt_params* params, std::function<void(std::string)>);

#endif