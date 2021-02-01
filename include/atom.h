#ifndef LISP_ATOM_H
#define LISP_ATOM_H

#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>

/**
 * Type system
 */
enum class Type {
	Nil,
    Integer,
    Rational,
    Symbol,
    Pair,
    // functions
	Builtin,
	Closure // user defined
};

struct Pair;
class Environment;
class iterator;

/**
 * Atom, the fundamental lisp type
 * Creates a tree-like structure for the program
 */
class Atom {
  public:

	typedef Atom (*builtin_t)(Atom args);
	using ValueType = std::variant<
		std::shared_ptr<Pair>, // pair
		std::string,           // symbol
		long,                  // integer
		double,                // rational
		builtin_t              // built-in functions
		>;
	Type      type;
	ValueType value;

//    iterator begin();
//	iterator end();
	[[nodiscard]] bool isNil() const { return type == Type::Nil; }
	/** Access **/
	[[nodiscard]] std::optional<std::string> symbol() const;
	[[nodiscard]] std::optional<long>        integer() const;
	[[nodiscard]] std::optional<double>      rational() const;
	[[nodiscard]] std::optional<Pair>        pair() const;
	[[nodiscard]] std::optional<builtin_t>   builtin() const;
	[[nodiscard]] std::optional<Pair*>       pairPtr();
	// Get first element of the list
	[[nodiscard]] Atom& car() const;
	[[nodiscard]] Atom& car();
	// Get the remaining elements of the list
	[[nodiscard]] Atom& cdr() const;
	[[nodiscard]] Atom& cdr();

    bool isProperList();

    Atom(const Atom& atom) = default;
	/** Constructors of all types **/
	// construct a Symbol atom
	explicit Atom(std::string symbol);
	// construct a Pair atom
	Atom(const Atom& car, const Atom& cdr);
    // construct a Builtin function atom
	explicit Atom(builtin_t fn):
		value(fn), type(Type::Builtin) {}
	// construct an Integer atom
	explicit Atom(long integer):
		value(integer), type(Type::Integer) {}
	// construct a Rational atom
	explicit Atom(double rational):
		value(rational), type(Type::Rational) {}
	// construct a Nil atom
//	explicit Atom(Type _type = Type::Nil):
//		value(0), type(_type) {}
    explicit Atom():
        value(0), type(Type::Nil) {}
	// construct a Closure atom
    Atom(Environment& env, Atom& params, Atom& body);
};

//class iterator {
//    Atom it;
//  public:
//    explicit iterator(Atom* atom): it(*atom){}
//    Atom operator++(int){
//        it = it.cdr();
//        return it;
//    }
//    Atom operator++(){
//        auto tmp = it;
//        it = it.cdr();
//        return it;
//    }
//    Atom operator*(){
//        return it.car();
//    }
//    bool operator==(iterator rhs) const{
//        return it.type == rhs.it.type && it.value == rhs.it.value;
//    }
//    bool operator!=(iterator rhs) const{
//        return it.type != rhs.it.type && it.value != rhs.it.value;
//    }
//
//};

struct Pair {
	Pair(const Atom& _car, const Atom& _cdr):
		car(_car), cdr(_cdr) {}
	Atom car, cdr;
};

static Atom nil = Atom();

static std::unordered_map<std::string, Atom> symbolTable;

inline std::ostream& operator<<(std::ostream& os, const Atom& atom) {
	switch(atom.type) {
	case Type::Nil:
		os << "NIL";
		break;
	case Type::Pair:
		os << "(";
		os << atom.car();
		os << " . ";
		os << atom.cdr();
		os << ")";
		break;
	case Type::Symbol:
		os << *atom.symbol();
		break;
	case Type::Integer:
		os << *atom.integer();
		break;
	case Type::Rational:
		os << *atom.rational();
		break;
	case Type::Builtin:
		os << "<BUILTIN%>" << *atom.builtin();
		break;
    case Type::Closure:
        os << "<CLOSURE%>" << *atom.builtin();
        break;
	}
	return os;
}

#endif //LISP_ATOM_H
