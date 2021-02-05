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
	Atom m_env;
	explicit Environment(const Atom& parent, bool b):
		m_env(parent) {}

  public:
	Environment(const Atom& parent, const std::vector<std::pair<std::string, Atom::builtin_t>>& pair):
		m_env(Atom(parent, nil)) {
		for(auto& s: pair) {
			set(Atom(s.first), Atom(s.second));
		}
	}
	explicit Environment(const Atom& parent):
		m_env(Atom(parent, nil)) {}

	Atom& atom() { return m_env; }

	/**
	 * Uses the atom structure as a map
	 * @param symbol
	 * @return
	 */
	Atom get(const Atom& symbol) {
		Atom parent = m_env.car();
		Atom bs     = m_env.cdr();

		// go through the Atom structure
		while(!bs.isNil()) {
			Atom b = bs.car();
			// find a symbol
			if(*b.car().symbol() == *symbol.symbol()) {
				return b.cdr();
			}
			bs = bs.cdr();
		}
		if(parent.isNil()) {
			throw EnvError(*this, parent, format("Unexpected identifier {}, have you defined {}?", *symbol.symbol(), *symbol.symbol()));
		}
		//		m_env = parent;
		auto parent_env = Environment(parent, true);
		return parent_env.get(symbol);
	}

	void set(const Atom& symbol, const Atom& value) {
		Atom bs = m_env.cdr();
		Atom b  = nil;

		// try to find symbol
		while(!bs.isNil()) {
			b = bs.car();
			if(b.car().symbol() == symbol.symbol()) {
				b.cdr() = value;
				return;
			}
			bs = bs.cdr();
		}
		// if it does not exist, prepend symbol value set
		b           = Atom(symbol, value);
		m_env.cdr() = Atom(b, m_env.cdr());
	}
};

#endif //LISP_ENVIRONMENT_H
