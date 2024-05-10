# Godot LLM
Isn't it cool to utilize large language model (LLM) to generate contents for your game? LLM has great potential in NPC models, game mechanics and design assisting. Thanks for technology like [llama.cpp](https://github.com/ggerganov/llama.cpp), "small" LLM, such as [llama-3-8B](https://huggingface.co/meta-llama/Meta-Llama-3-8B), run reasonably well locally on lower-end machine without a good GPU.
I want to experiment LLM in Godot but I couldn't find any good library, so I decided to create one here.

# Table of Contents
1. [Quick start](#quick-start)
    - [Install](#install)
    - [Text Generation: GDLlama node](#text-generation-gdllama-node)
    - [Text Embedding: GDEmbedding node](#text-embedding-gdembedding-node)
    - [Multimodal Text Generation: GDLlava node](#multimodal-text-generation-gdllava-node)
    - [Template/Demo](#templatedemo)
2. [Features](#features)
3. [Documentation](#documentation)
    - [Inspector Properties](#inspector-properties)
    - [GDLlama Functions and Signals](#gdllama-functions-and-signals)
    - [GDEmbedding Functions and Signals](#gdembedding-functions-and-signals)
    - [GDLlava Functions and Signals](#gdllava-functions-and-signals)
4. [Compile from Source](#compile-from-source)

# Quick Start

## Install
1. Get `Godot LLM` directly from the asset library, or download the zip file from the [release page](https://github.com/Adriankhl/godot-llm/releases), and unzip it to place it in the `addons` folder in your godot project
2. Now you should be able to see `GdLlama`, `GdEmbedding`, and `GDLlava` nodes in your godot editor.

## Text Generation: GDLlama node
1. Download a [supported](https://github.com/ggerganov/llama.cpp?tab=readme-ov-file#description) LLM model in GGUF format (recommendation: [Meta-Llama-3-8B-Instruct-Q5_K_M.gguf](https://huggingface.co/lmstudio-community/Meta-Llama-3-8B-Instruct-GGUF/tree/main)), move the file to somewhere in your godot project
2. Setup your model with GDScript, point `model_path` to your GGUF file. The default `n_predict = -1` generates an infinite sequence, we want it to be shorter here
```
func _ready():
    var gdllama = GDLlama.new()
    gdllama.model_path = "./models/Meta-Llama-3-8B-Instruct.Q5_K_M.gguf" ##Your model path
    gdllama.n_predict = 20
```
3. Generate text starting from "Hello"
```
    var generated_text = gdllama.generate_text_simple("Hello")
    print(hello)
```
4. Text generation is slow, you may want to call `gdllama.run_generate_text("Hello", "", "")` to run the generation in background, then handle the `generate_text_updated` or `generate_text_finished` signals

```
    gdllama.generate_text_updated.connect(_on_gdllama_updated)
    gdllama.run_generate_text("Hello", "", "")

func _on_gdllama_updated(new_text: String):
    print(new_text)
```

## Text Embedding: GDEmbedding node
1. Download a [supported](https://github.com/ggerganov/llama.cpp?tab=readme-ov-file#description) embedding model in GGUF format (recommendation: [mxbai-embed-large-v1.Q5_K_M.gguf](https://huggingface.co/ChristianAzinn/mxbai-embed-large-v1-gguf/tree/main)), move the file to somewhere in your godot project
2. Setup your model with GDScript, point `model_path` to your GGUF file
```
func _ready():
    var gdembedding= GDEmbedding.new()
    gdembedding.model_path = "./models/mxbai-embed-large-v1.Q5_K_M.gguf"
```
3. Compute the embedded vector of "Hello world" in PackedFloat32Array
```
    var array: PackedFloat32Array = gdembedding.compute_embedding("Hello world")
    print(array)
```

4. Compute the similarity between "Hello" and "World"
```
    var similarity: float = gdembedding.similarity_cos_string("Hello", "World")
    print(similarity)
```

5. Embedding computation can be slow, you may want to call `gdembedding.run_compute_embedding("Hello world")` or `gdembedding.run_similarity_cos_string("Hello", "Worlld")` to run the computation in background, then handle the `compute_embedding_finished` and `similarity_cos_string_finished` signals

```
    gdembedding.compute_embedding_finished.connect(_on_embedding_finished)
    gdembedding.run_compute_embedding("Hello world")

func _on_embedding_finished(embedding: PackedFloat32Array):
    print(embedding)
```

```
    gdembedding.similarity_cos_string_finished.connect(_on_embedding_finished)
    gdembedding.run_similarity_cos_string("Hello", "Worlld")

func _on_similarity_finished(similarity: float):
    print(similarity)
```

Note that the current implementation only allows one thread running per node, avoid calling 2 `run_*` methods consecutively:
```
    ## Don't do this, this will hang your UI
    gdembedding.run_compute_embedding("Hello world")
    gdembedding.run_similarity_cos_string("Hello", "Worlld")
```

Instead, always wait for the finished signal or check `gdembedding.is_running()` before calling a `run_*` function.


## Multimodal Text Generation: GDLlava node
1. Download a [supported](https://github.com/ggerganov/llama.cpp?tab=readme-ov-file#description) multimodal model in GGUF format (recommendation: [llava-phi-3-mini-int4.gguf](https://huggingface.co/xtuner/llava-phi-3-mini-gguf/tree/main)), be aware that there are two files needed - a `gguf` language model and a mmproj model (typical name `*mmproj*.gguf`), move the files to somewhere in your godot project
2. Setup your model with GDScript, point `model_path` and `mmproj_path` to your corresponding GGUF files
```
func _ready():
    var gdllava = GDLlava.new()
    gdllava.model_path = "./models/llava-phi-3-mini-int4.gguf"
    gdllava.mmproj_path = "./models/llava-phi-3-mini-mmproj-f16.gguf"
```
3. Load an image (`svg`, `png`, or `jpg`, other format may also works as long as it is supported by Godot), or use your game screen (viewport) as a image
```
    var image = Image.new()
    image.load("icon.svg")

    ## Or load the game screen instead
    #var image = get_viewport().get_texture().get_image()
```
4. Generate text to provide "Provide a full description" for the image
```
    var generated_text = gdllava.generate_text_image("Provide a full description", image)
    print(generated_text)
```
5. Text generation is slow, you may want to call `gdllama.run_generate_text("Hello", "", "")` to run the generation in background, then handle the `generate_text_updated` or `generate_text_finished` signals
```
    gdllava.generate_text_updated.connect(_on_gdllava_updated)
    gdllava.run_generate_text_image("Provide a full description", image)

func _on_gdllava_updated(new_text: String):
    print(new_text)
```

## Template/Demo
The [godot-llm-template](https://github.com/Adriankhl/godot-llm-template) provides a rather complete demonstration on different functionalities of this plugin


# Features
## Already working
* Windows, Linux, Android
* [llama.cpp](https://github.com/ggerganov/llama.cpp)
    - Text generation
    - Embedding
    - Multimodal

## TODO
* Add in-editor documentation, waiting for proper support in Godot 4.3
* Vulken backend, need to resolve this [issue](https://github.com/ggerganov/llama.cpp/issues/7130)
* Mac and ios
* Automatically generate json schema from data classes in GDSCript
* More [llama.cpp](https://github.com/ggerganov/llama.cpp) features
* [mlc-llm](https://github.com/mlc-ai/mlc-llm) integration
* RAG framework, waiting for [sqlite-vec](https://github.com/asg017/sqlite-vec) (seems like this is the only one with proper mobile support in mind)
* Any suggestion?


# Documentation

## Inspector Properties
There are 3 nodes added by this plugin: `GdLlama`, `GdEmbedding`, and `GdLlava`.
Each type of node owns a set of properties which affect the computational performance and the generated output. Some of the properties belong to more than one node, and they generally have similar meaning for all types of node.

* `Model Path`: location of your GGUF model
* `Mmproj Path` location of your `mmproj` GGUF file, for `GdLlava` only
* `Instruct`: question and answer interactive mode
* `Interactive`: custom interactive mode, you should set your `reverse_prompt`, `input_prefix`, and `input_suffix` to set up a smooth interaction
* `Reverse Prompt`: AI stops to wait for user input after seeing this prompt being generated, a good example is "User:"
* `Input Prefix`: append before every user input
* `Input Suffix`: append after every user input
* `Should Output prompt`: whether the input prompt should be included in the output
* `Should Output Bos`: whether the special bos (beginning of sequence) token should be included in the output
* `Should Output Eos`: whether the special eos (ending of sequence) token should be included in the output
* `Context Size`: number of tokens the model can process at a time
* `N Predict`: number of new tokens to generate, generate infinite sequence if -1
* `N Keep`: when the model run out of `context size`, it starts to forget about earlier context, set this variable to force the model to keep a number of the earliest tokens to keep the conversation relevant
* `Temperature`: the higher the temperature, the more random the generated text
* `Penalty Repeat`: penalize repeated sequence, diabled if -1
* `Penalty Last N`: the number of latest token to consider when penalizing repeated sequence, disabled if 0, `Context Size` if -1
* `Penalilze Nl`: penallize newline token
* `Top K`: only sample from this amount of tokens with the highest probabilities, disabled if 0
* `Top P`: only sample from tokens within this cumulative probability, disabledd if 1.0
* `Min P`: only sample from tokens with at least this probability, disabledd if 0.0
* `N Thread`: number of cpu threads to use
* `N GPU Layer`: number of layer offloaded to GPU
* `Escape`: process escape character in input prompt
* `N Batch`: maximum number of tokens per iteration during continuous batching
* `N Ubatch`: maximum batch size for computation

## GdLlama functions and signals

### Functions
* `generate_text_simple(prompt: String) -> String`: generate text from prompt
* `generate_text_json(prompt: String, json: String) -> String`: generate text in a format enforced by a [json schema](https://json-schema.org/), see [the following section](#text-generation-with-json-schema)
* `generate_text_grammar(prompt: String, grammar: String) -> String`: generate text in a format enforced by [GBNF grammar](https://github.com/ggerganov/llama.cpp/blob/master/grammars/README.md)
* `generate_text(prompt: String, grammar: String, json: String) -> String`: a wrapper function, run `generate_text_gramma` if `grammar` is non-empty, runs `generate_text_json` if `json` is non-empty, run `generate_text_simple` otherwise
* `run_generate_text(prompt: String, grammar: String, json: String) -> Error`: run `generate_text` in background, rely on signals to recieve generated text, note that only one background thread is allowd for a GDLlama node, calling this function when the background thread is still running will freeze the logic until the background thread is done
* `input_text(input: String)`: input text to interactively generate text (with either `Instruct` or `Interactive` enabled) with the model, only works if the model is waiting for intput, inputing an empty string means the model should continue to generate what it has been generating
* `stop_generate_text()`: stop text generation, clean up the model and the background thread
* `is_running() -> bool`: whether the background thread is running
* `is_waiting_input() -> bool`: whether the model is waiting for input text (with either `Instruct` or `Interactive` enabled)

### Signals
* `generate_text_finished(text: String) `: emitted with the full generated text when a text generation is completed. When either `Instruct` or `Interactive` enabled, this signal is emitted after the whole interaction is finished
* `generate_text_updated(new_text: String)`: instead of waiting the full generated text, this signal is emited whenever a new token (part of the text sequence) is generated, which forms a stream of strings
* `input_wait_started()`: the model is now starting to wait for user input (with either `Instruct` or `Interactive` enabled)

## GDEmbedding functions and signals
### Functions
* `compute_embedding(prompt: String) -> PackedFloat32Array`: compute the embedding vector of a prompt
* `similarity_cos_array(array1: PackedFloat32Array, array2: PackedFloat32Array) -> float`: compute the cosine similarity between two embedding vectors, this is a fast function, no model is loaded
* `similarity_cos_string(s1: String, s2: String) -> float`: compute the cosine similarity between two strings
* `run_compute_embedding(prompt: String) -> Error`: run `compute_embedding(prompt: String)` in background, rely on the `compute_embedding_finished` signal to recieve the embedding vector, note that only one background thread is allowd for a GDEmbedding node, calling this function when the background thread is still running will freeze the logic until the background thread is done
* `run_similarity_cos_string(s1: String, s2: String) -> Error`: run `similarity_cos_string` in background, rely on the `compute_similairty_finished` signal to recieve the cosine similairty, note that only one background thread is allowd for a GDEmbedding node, calling this function when the background thread is still running will freeze the logic until the background thread is done
* `is_running() -> bool`: whether the background thread is running

### Signals
* `compute_embedding_finished(embedding: PackedFloat32Array)`: emitted when `run_compute_embedding` is completed
* `similarity_cos_string_finished(similarity: float)`: emitted when `run_similarity_cos_string` is completed

## GDLlava functions and signals
### functions
* `generate_text_base64(prompt: String, image_base64: String) -> String`: generate text based on a prompt and a base64 string which encodes a `jpg` or `png` image
* `generate_text_image(prompt: String, image: Image) -> String`: generate text based on a prompt and an `Image` object in Godot
* `run_generate_text_base64(prompt: String, image_base64: String) -> Error`: run `generate_text_base64` in background, rely on signals to recieve generated text, note that only one background thread is allowd for a GDLlava node, calling this function when the background thread is still running will freeze the logic until the background thread is done
* `run_generate_text_base64(prompt: String, image: Image) -> Error`: run `generate_text_base64` in background, rely on signals to recieve generated text, note that only one background thread is allowd for a GDLlava node, calling this function when the background thread is still running will freeze the logic until the background thread is done
* `stop_generate_text()`: stop text generation, clean up the model and the background thread
* `is_running() -> bool`: whether the background thread is running

### Signals
* `generate_text_finished(text: String) `: emitted with the full generated text when a text generation is completed
* `generate_text_updated(new_text: String)`: instead of waiting the full generated text, this signal is emited whenever a new token (part of the text sequence) is generated, which forms a stream of strings


## Text generation with Json schema

Suppose you want to generate a character with:
  * `name`: a string from 3 character to 20 character
  * `birthday`: a string with a specific date format
  * `weapon`: either "sword", "bow", or "wand
  * `description`: a text with minimum 10 character

You should first create a GDLlama node, and turn `Should Output prompt`, `Should Output Bos`, and `Should Output Eos` off either by inspector or by script:
```
should_output_prompt = false
should_output_bos = false
should_output_eos = false
```


Construct the following `_person_schema` dictionary in GDScript:
```
var _person_schema = {
	"type": "object",
	"properties": {
		"name": {
			"type": "string",
			"minLength": 3,
			"maxLength": 20,
		},
		"birthday": {
			"type": "string",
			"format": "date"
		},
		"weapon": {
			 "enum": ["sword", "bow", "wand"],
		},
		"description": {
			"type": "string",
			"minLength": 10,
		},
	},
	"required": ["name", "birthday", "weapon", "description"]
}
```

Then convert it to a json string
```
var person_schema: String = JSON.stringify(_person_schema)
```

Supposed you are interested in a "Main character in a magic world", you can generate the character using the `generate_text_json(prompt, json_scheme)` of the `GDLlama` node:
```
var json_string: String = generate_text_json(prompt, json_scheme)
```

Note that text generation is slow, you may want to use `run_generate_text(prompt, "", json_scheme)` to run the generation in background, then handle `generate_text_finished` to receive the generated text.

`json_string` should look like this:
```
{"birthday": "2000-05-12", "description": "A young wizard with a pure heart and a mischievous grin. He has a wild imagination and a love for adventure. He is always up for a challenge and is not afraid to take risks.", "name": "Eryndor Thorne", "weapon": "wand"}
```

Now, the generated data is ready, you can parse back to a dictionary or other object to use the data.
```
var dict: Dictionary = {}
var json = JSON.new()
var error = json.parse(json_string)
if (error == OK):
		dict = json.data

print(dict["name"]) ##Eryndor Thorne
```

# Compile from source
Install build tools and Vulkan SDK for your operating system, then clone this repository
```
git clone https://github.com/Adriankhl/godot-llm.git
cd godot-llm
git submodule update --init --recursive
mkdir build
cd build
```

Currently, vulkan backend is not working for embedding, don't add the `-DLLAMA_VULKAN=1` flag

Run `cmake`.

On Windows:
```
cmake .. -GNinja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DLLAMA_NATIVE=OFF  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release
```

On Linux:
```
cmake .. -GNinja -DLLAMA_NATIVE=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release
```

For Android, set `$NDK_PATH` to your android ndk directory, then:
```
cmake .. -GNinja -DCMAKE_TOOLCHAIN_FILE=$NDK_PATH\cmake\android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-23 -DCMAKE_C_FLAGS="-mcpu=generic" -DCMAKE_CXX_FLAGS="-mcpu=generic" -DCMAKE_BUILD_TYPE=Release
```

You may want to adjust the compile flags for Android to suit different types of CPU.

Then compile and install by `ninja`:
```
ninja -j4
ninja install
```

The folder `../install/addons/godot_llm` can be copy directly to the `addons` folder of your godot project.
