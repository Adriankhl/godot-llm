# Godot LLM
Isn't it cool to utilize large language model (LLM) to generate contents for your game? LLM has great potential in NPC models, game mechanics and design assisting. Thanks for technology like [llama.cpp](https://github.com/ggerganov/llama.cpp), "small" LLM, such as [llama-3-8B](https://huggingface.co/meta-llama/Meta-Llama-3-8B), run reasonably well locally on lower-end machine without a good GPU.
I want to experiment LLM in Godot but I couldn't find any good library, so I decided to create one here.

# How to use
1. Download the zip file from the [release page](https://github.com/Adriankhl/godot-llm/releases), and unzip it to place it in the `addons` folder in your godot project
2. Download an LLM model in GGUF format (recommendation: [Meta-Llama-3-8B-Instruct-Q5_K_M.gguf](https://huggingface.co/lmstudio-community/Meta-Llama-3-8B-Instruct-GGUF/tree/main)), move the file to somewhere in your godot project
3. Now you should be able to add the `GdLlama` node, set the variables (you must point the `Model Path` to the GGUF file) and call function `generate_text(prompt: String) -> String` to generate some texts

# Demo
See [godot-llm-template](https://github.com/Adriankhl/godot-llm-template)

# Features
* `GdLlama` node
  - function `generate_text(prompt: String) -> String`: generate text from prommpt, note that this function takes a long time to run so it is best to use this with `Thread`
  - function `stop_generate_text()`: stop text generation from the above function
  - signal `generate_text_updated(new_text: String)`: instead of waiting the full generated text from `generate_text(prompt: String) -> String`, this signal is emited whenever a new token (part of the text sequence) is generated, which forms a stream of strings
  - Inspector variables
    * `Model Path`: location of your gguf model
    * `Context Size`: number of tokens the model can process at a time
    * `N Batch`: maximum number of tokens per iteration during continuous batching
    * `N Ubatch`: maximum batch size for computation
    * `N GPU Layer`: number of layer offloaded to GPU
    * `Escape`: process escape character
    * `N Predict`: number of new tokens to generate
    * `N Thread`: number of cpu threads to use

# Already working
* Windows and Linux
* Basic [llama.cpp](https://github.com/ggerganov/llama.cpp) text generation

# TODO
* Android
* More [llama.cpp](https://github.com/ggerganov/llama.cpp) features
* [mlc-llm](https://github.com/mlc-ai/mlc-llm) integration

# Compile from source
Install build tools and Vulkan SDK for your operating system, then clone this repository
```
git clone https://github.com/Adriankhl/godot-llm.git
cd godot-llm
git submodule update --init --recursive
mkdir build
cd build
```

Run`cmake`.

On Windows:
```
cmake .. -GNinja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DLLAMA_VULKAN=1 -DCMAKE_BUILD_TYPE=Release
```

On Linux:
```
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release -DLLAMA_VULKAN=1
```

Then compile and install by `ninja`:
```
ninja -j4
ninja install
```

The folder `../install/addons/godot_llm` can be copy directly to the `addons` folder of your godot project.
