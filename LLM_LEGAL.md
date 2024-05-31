# Information on legal issues of large language model (LLM) in games

* [Government regulation](#government-regulation)
* [Steam AI policy](#steam-ai-policy)
* [Best practices](#best-practices)
* [Articles](#articles)
* [Communities](#communities)

## Government regulation

* [Large language models and copyright Wikipedia](https://en.wikipedia.org/wiki/Wikipedia:Large_language_models_and_copyright)
    - Copyright law is slightly different in every country, AI generated contents may or may not be copyrightable
    - AI generated contents can violate copyright law if it largely duplicates copyrighted materials
* [European Artificial Intelligence Act](https://artificialintelligenceact.eu/) ([Wiki](https://en.wikipedia.org/wiki/Artificial_Intelligence_Act)):
    - [High-level summary](https://artificialintelligenceact.eu/high-level-summary/): AI in game belongs to the `Minimal risk` category, it `is unregulated...at least in 2021; this is changing with generative AI` (doesn't mean you can violate copyright though)
    - Section 105 of the [Document](https://www.europarl.europa.eu/doceo/document/TA-9-2024-0138_EN.pdf): `Text and data mining techniques may be used extensively in this context for the retrieval and analysis of such content, which may be protected by copyright and related rights. Any use of copyright protected content requires the authorisation of the rightsholder concerned unless relevant copyright exceptions and limitations apply...Under these rules, rightsholders may choose to reserve their rights over their works or other subject matter to prevent text and data mining...`
    - Exceptions in [Directive on Copyright in the Digital Single Market](https://eur-lex.europa.eu/eli/dir/2019/790/oj): `In the fields of research, innovation, education and preservation of cultural heritage...`
* California [Generative AI Copyright Disclosure Act](https://en.wikipedia.org/wiki/Generative_AI_Copyright_Disclosure_Act)
    - Status: introduced on April 9, 2024
    - `The legislation requires the submission of a notice regarding the identity and the uniform resource locator (URL) address of the copyrighted works used in the training data to the Register of Copyrights at least 30 days before the public release of the new or updated version of the AI model`

## Steam AI policy
Check this [Steam Article](https://store.steampowered.com/news/group/4145017/view/3862463747997849618). TLDR: you can publish a game with AI generated contents on Steam, but you should report to Steam the measure you took to prevent  illegal contents, for both pre-generated text and live-generated text. This [reddit post](https://www.reddit.com/r/gamedev/comments/1adgxt3/is_the_use_of_openais_llm_allowed_by_steam/) summarizes it as
```
you can have your game generate LMM content at runtime. There are two caveats:

    * You cannot have runtime-generated "adult-only sexual content"

    * You must tell Steam what guards/checks you have against "generating illegal content"

    * (Whatever you tell Steam will probably appear on your store page.)

    * (This implies you must write some sanity-checking code. A simple keyword blacklist on user input would probably suffice.)

```

## Best practices

* Even without all these legal issues, just relying on LLM-generated contents won't make your game stand out. Like many other video game technologies, LLM can be useful, but making a successful game is much more than being able to utilize a specific technology.
* If you use pre-generated contents, double check your contents to make sure they are legal, e.g., search them on Google to see if it is too similar to other copyrighted materials
* Live-generated contents are in general harder to control, but you can still pick an appropriate model, contrain the input prompt, or apply filters to generated texts to safeguard the generated contents

### Selecting LLM models

#### Fully open model

Different LLM models have different degrees of openness. Many "open" LLM models are open in the sense that the trained models are permissively licensed, e.g., Phi-3 is under [MIT License](https://huggingface.co/microsoft/Phi-3-mini-4k-instruct/blob/main/LICENSE). The training data and the training data themselves remain closed. In contrast, models like [olmo](https://huggingface.co/allenai/OLMo-7B) and [MobileVLM](https://github.com/Meituan-AutoML/MobileVLM) open up their training data and code. Fully open model doesn't necessarily mean they are immune from copyright infringement, but it allows people to check whether there are copyrighted data being used that doesn't fall into the [fair use](https://en.wikipedia.org/wiki/Fair_use) case.

#### Models that emphasize on being ethical

If a company put sufficient efforts into filtering, cleaning, and synthesizing the data, and the model is carefully tested, one may trust the ethicality of the output from the model.

* [Microsoft Responsible AI Standard](https://www.microsoft.com/en-us/ai/principles-and-approach/) and see "Safety-first model design" in [Introducing Phi-3: Redefining what’s possible with SLMs](https://azure.microsoft.com/en-us/blog/introducing-phi-3-redefining-whats-possible-with-slms/)
* [Our responsible approach to Meta AI and Meta Llama 3](https://ai.meta.com/blog/meta-llama-3-meta-ai-responsibility/)

#### Consider using a safeguard

There are many ways to safeguard LLM-genrated contents, such as constraining the format and setting up text filter. Meta even published a [Llama Guard 2](https://llama.meta.com/docs/model-cards-and-prompt-formats/meta-llama-guard-2/) model to identify issues (e.g., privacy, intellectual property) in user input and generated output, and yes it has a [GGUF version](https://huggingface.co/neopolita/meta-llama-guard-2-8b-gguf/tree/main) that can be used in inference.

## Articles

* [Large Language Models and Games: A Survey and Roadmap](https://arxiv.org/html/2402.18659v1)
* [How ChatGPT and our language models are developed](https://help.openai.com/en/articles/7842364-how-chatgpt-and-our-language-models-are-developed) from OpenAI

## Communities

* [aiwars reddit](https://www.reddit.com/r/aiwars/): `Following news and developments on ALL sides of the AI art debate (and more)`