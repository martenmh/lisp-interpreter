#include "atom.h"
//#include "debug.h"
#include "environment.h"

Atom::Atom(const Atom& car, const Atom& cdr):
	value(std::make_shared<Pair>(Pair(car, cdr))), type(Type::Pair) {}

#define OPTIONAL(name, atomTypeValue, staticType)  \
	std::optional<staticType> Atom::name() const { \
		if(type != Type::atomTypeValue)            \
			return std::nullopt;                   \
		return std::get<staticType>(value);        \
	}

OPTIONAL(symbol, Symbol, std::string)
OPTIONAL(integer, Integer, long)
OPTIONAL(rational, Rational, double)

#undef OPTIONAL

std::optional<Pair*> Atom::pairPtr() {
	if(type != Type::Pair)
		return std::nullopt;
	return std::get<std::shared_ptr<Pair>>(value).get(); //std::get<Pair>(value);
}

std::optional<Pair> Atom::pair() const {
	if(type != Type::Pair)
		return std::nullopt;
	return *std::get<std::shared_ptr<Pair>>(value); //std::get<Pair>(value);
}

std::optional<Atom::builtin_t> Atom::builtin() const {
	if(type != Type::Builtin)
		return std::nullopt;
	return *std::get<builtin_t>(value);
}

Atom& Atom::cdr() {
	return std::get<std::shared_ptr<Pair>>(value)->cdr;
}
Atom& Atom::car() {
	return std::get<std::shared_ptr<Pair>>(value)->car;
}
Atom& Atom::cdr() const {
	return std::get<std::shared_ptr<Pair>>(value)->cdr;
}
Atom& Atom::car() const {
	return std::get<std::shared_ptr<Pair>>(value)->car;
}

Atom::Atom(std::string symbol):
	value(symbol), type(Type::Symbol) {
	symbolTable[symbol];
}

Atom::Atom(Environment& env, Atom& params, Atom& body):
	type(Type::Closure) {

	if(params.isNil() || body.isNil()) {
        throw EvalError(params, "Closure parameters and body are nil");
	}

	// all parameter names should be symbols
    Atom p;
	p = params;
	while(!p.isNil()) {
		if(p.car().type != Type::Symbol) {
			throw TypeError(p, "Type error:");
		}
		p = p.cdr();
	}
	*this = Atom(env.atom(), Atom(params, body));
	this->type = Type::Closure;
}

bool Atom::isProperList() {
	Atom expr = *this;
	// go through the list
	while(!expr.isNil()) {
		if(expr.type != Type::Pair)
			return false;
		expr = expr.cdr();
	}
	return true;
}

//iterator Atom::begin() {
//	return iterator(this);
//}
//iterator Atom::end() {
//    return iterator(&nil);
//}
