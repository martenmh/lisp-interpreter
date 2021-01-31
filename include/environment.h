#ifndef LISP_ENVIRONMENT_H
#define LISP_ENVIRONMENT_H

#include "atom.h"
#include "debug.h"

#include <vector>

/**
 * The environment contains all definitions
 * It uses the atom structure as a map
 */
class Environment {

	Atom env;
  public:
	Environment(const Atom& parent, const std::vector<std::pair<std::string, Atom::builtin_t>>& pair): env(Atom(parent, nil)){
		for(auto& s : pair){
			set(Atom(s.first), Atom(s.second));
		}
	}
	explicit Environment(const Atom& parent) : env(Atom(parent, nil)){}
	Atom& atom(){ return env; }

	/**
	 * Uses the atom structure as a map
	 * @param symbol
	 * @return
	 */
	Atom get(const Atom& symbol) {
        std::cout << env << std::endl;
		Atom parent = env.car();
        Atom bs = env.cdr();
		// go through the Atom structure
		while(!bs.isNil()){
            Atom b = bs.car();
            // find a symbol
			if(*b.car().symbol() == *symbol.symbol()){
				return b.cdr();
			}
			bs = bs.cdr();
		}
		if(parent.isNil()){
			throw EnvError(*this, parent, format("Unexpected identifier {}, have you defined {}?", *symbol.symbol(), *symbol.symbol()) );
		}
		env = parent;
		return get(symbol);
	}

    void set(const Atom& symbol, const Atom& value) {

		Atom bs = env.cdr();
		Atom b = nil;

		while(!bs.isNil()){
			b = bs.car();
			if(b.car().symbol() == symbol.symbol()){
				b.cdr() = value;
			}
			bs = bs.cdr();
		}
		b = Atom(symbol, value);
		env.cdr() = Atom(b, env.cdr()); // prepend symbol value set
    }
};

#endif //LISP_ENVIRONMENT_H
