#ifndef LISP_FORMAT_H
#define LISP_FORMAT_H

#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

inline void format(std::stringstream& str, const char* fmt) {
	str << fmt;
}

template<typename T, typename... Targs>
inline void format(std::stringstream& ss, const char* fmt, T value, Targs... args) {
	for(; *fmt != '\0'; fmt++) {
		if(*fmt == '{' && *(fmt + 1) == '}') {
			ss << value;
			format(ss, fmt + 2, args...);
			return;
		}
		ss << *fmt;
	}
}

template<typename T, typename... Targs>
inline std::string format(const char* fmt, T value, Targs... args) {
	std::stringstream ss;
	format<T, Targs...>(ss, fmt, value, args...);
	return ss.str();
}

// Note: these are the foreground colors, for the background colors add 10 to each,
// however, set_background_color already implements the addition
enum FormatColor {
	black    = 30,
	red      = 31,
	green    = 32,
	yellow   = 33,
	blue     = 34,
	magenta  = 35,
	cyan     = 36,
	white    = 37,
	standard = 39,
	// bright variants
	bright_black   = 90,
	bright_red     = 91,
	bright_green   = 92,
	bright_yellow  = 93,
	bright_blue    = 94,
	bright_magenta = 95,
	bright_cyan    = 96,
	bright_white   = 97
};

enum FormatDecoration {
	Normal      = 0,
	bold        = 1,
	faint       = 2,
	italic      = 3,
	underline   = 4,
	slow_blink  = 5,
	rapid_blink = 6,
	crossed_out = 9,
	riverced    = 26,
};

enum align {
	left,
	right
};

enum dec_width {
	absolute,
	relative
};

// Structure for a specific repeatable format
struct Format {
	Format(int width, align alignment) {
		this->width     = width;
		this->alignment = alignment;
	}
	Format(const Format& format) {
		this->precision  = format.precision;
		this->decoration = format.decoration;
		this->fill       = format.fill;
		this->c_color    = format.c_color;
		this->width      = format.width;
	}
	Format(FormatColor color):
		c_color(color) {}
	FormatDecoration decoration       = FormatDecoration::Normal;
	align            alignment        = left;
	short            precision        = 1;
	char             fill             = ' ';
	FormatColor      c_color          = standard;
	unsigned int     width            = 30;
	dec_width        decoration_width = absolute;
};

// Formatted Output stream
class fout: private Format {
	// Maybe todo?, add support for 8 bit colors (full rgb)
	std::string output;

  public:
	// todo check if this can be done automatically instead of assigning each field
	fout(std::string str, Format fmt):
		output(std::move(str)), Format(fmt) {
	}

	fout(std::string str, unsigned int width = 0, align alignment = left):
		output(std::move(str)), Format(width, alignment) {
	}

	// Set colors:
	inline fout& set_fcolor(FormatColor col) {
		this->c_color = col;
		return *this;
	}
	inline fout& set_bcolor(FormatColor col) {
		this->c_color = static_cast<enum FormatColor>(col + 10);
		return *this;
	}

	inline fout& set_precision(short precision) {
		this->precision = precision;
		return *this;
	}
	inline fout& set_fill(char fill) {
		this->fill = fill;
		return *this;
	}
	inline fout& set_decoration(FormatDecoration decoration) {
		this->decoration = decoration;
		return *this;
	}
	inline fout& set_alignment(align alignment) {
		this->alignment = alignment;
		return *this;
	}
	inline fout& set_width(unsigned int width) {
		this->width = width;
		return *this;
	}
	static void color(FormatColor col) {
		std::cout << "\033[;" << col << "m";
	}
	static void reset() {
		std::cout << "\033[0m";
	}
	friend std::ostream& operator<<(std::ostream& os, const fout& out) {
		std::stringstream decoration;
		decoration << "\033[" << out.decoration << ";" << out.c_color << "m";
		if(out.decoration_width == absolute)
			os << decoration.str();
		os << (out.alignment == align::left ? std::left : std::right) // alignment
		   << std::setfill(out.fill)                                  // set fill if set
		   << std::setw(out.width);                                   // m_width
		if(out.decoration_width == relative)
			os << decoration.str();
		os << out.output // actual output
		   << "\033[0m"; // reset (for future input)
		return os;
	}
};

//void m_width(std::string str, unsigned int m_width, justify justify = left);

inline void print(const char* format) {
	std::cout << format << std::endl;
}
// Python / Rust like print using {} as placeholders
template<typename T, typename... Targs>
inline void print(const char* format, T value, Targs... Fargs) {
	for(; *format != '\0'; format++) {
		if(*format == '{' && *(format + 1) == '}') {
			std::cout << value;
			print(format + 2, Fargs...);
			return;
		}
		std::cout << value;
	}
}

inline void strfmt(std::stringstream& str, const char* format) {
	str << format;
}

template<typename T, typename... Targs>
inline void strfmt(std::stringstream& ss, const char* format, T value, Targs... args) {
	for(; *format != '\0'; format++) {
		if(*format == '{' && *(format + 1) == '}') {
			ss << value;
			strfmt(ss, format + 2, args...);
			return;
		}
		ss << *format;
	}
}

template<typename T, typename... Targs>
inline std::string strfmt(const char* format, T value, Targs... args) {
	std::stringstream ss;
	strfmt<T, Targs...>(ss, format, value, args...);
	return ss.str();
}

// get the max used m_width of all used
// TODO: concept: template<Iterable T>
template<typename T>
constexpr unsigned int get_max_width(T strings) {
	int width = 0;
	for(auto it = strings.begin(); it != strings.end(); it++) {
		int len = std::to_string(*it).length();
		width   = len > width ? len : width;
	}
	return width;
}

//// Format string into multiple lines (set a line break after a certain m_width has been met)
//inline std::string apply_max_width(std::string description, int m_width){
//    auto pos = description.begin();
//    std::stringstream newDescription;
//    while(pos < description.end()) {
//        bool b = (pos + m_width < description.end());
//        (pos == description.begin() ? newDescription : (newDescription << std::left << std::setw(getW()) << " " << std::string(optional_pre_description.length(), ' '))) << std::string(pos,  b ? pos + m_width : description.end());
//        // Continue word
//        pos += m_width;
//        while(pos < description.end() && !std::isspace(*pos)){
//            newDescription << *pos;
//            pos++;
//        }
//        if(pos < description.end())
//            newDescription << std::endl;
//        pos++;
//    }
//    return newDescription.str();
//}

#endif //LISP_FORMAT_H