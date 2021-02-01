#include "atom.h"
#include "environment.h"
#include "eval.h"
#include "parser.h"
#include "ringbuffer.h"
#include "tokenizer.h"
#include "builtin.h"
#include <iostream>

void repl(const std::string& prompt, Environment& env) {
	std::string                 input;
	RingBuffer<std::string, 50> history;

	for(;;) {
		std::cout << prompt;
		std::getline(std::cin, input); // read
		history.push_back(input);
		if(input == ":history") {
			const size_t size = history.size();
			for(int i = 0; i < size; i++) {
				std::cout << history[i] << "\n";
			}
		}

		if(input == ":q")
			break;
		try {
			auto tokens = tokenizer(input);
			Atom root   = expression(tokens);
			std::cout << root << std::endl;
			Atom s = eval(root, env);
			std::cout << s << std::endl;
        } catch(EnvError& err) {
			std::cerr << err.what() << "\n";
		} catch(ProgramError& err) {
			std::cerr << err.what() << "\n";
			staticError(err);
		} catch(EvalError& err) {
			std::cerr << err.what() << "\n";
			dynamicError(err);
		}
	}
}

int main(int argv, char** argc) {
	Environment env(nil, {{"car", builtin::car},
						  {"cdr",  builtin::cdr},
						  {"cons", builtin::cons},
						  {"+",    builtin::add},
						  {"-",    builtin::sub},
						  {"*",    builtin::mul},
						  {"/",    builtin::div}});
	if(argv == 1){

	} else {

	}

	repl("> ", env);
}