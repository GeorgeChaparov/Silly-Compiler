#include <string>
#include "Utils.h"

const char SPECIAL_IDENTIFIER_CHARS[] = { '_' };
const std::string OPERATORS[] = { "+", "-", "*", "/", "=", "%", "<", ">", "==", "<=", ">=", "!=", "++", "--", "&&", "||" };
const char START_OPERATOR_SYMBOL[] = { '|', '&', '!' };
const char SEPARATIORS[] = { ' ', '\n', '\r', '\t' };
const std::string KEYWORDS[] = {
	// Control flow
	"if", "else",
	"for", "while", "do", "break", "continue", "goto",

	// Functions and return
	"return",

	// Type specifiers
	 "bool", "char",
	 "int", "float",

	 // Misc
	 "true", "false", "null",

	 "cout"
};
const char PUNCTUATION[] = { '{', '}', '(', ')' , ';', '.' , ',' };

const int SEPARATIORS_SET_LENGTH = GetArraySize(SEPARATIORS);
const int OPERATORS_SET_LENGTH = GetArraySize(OPERATORS);
const int SPECIAL_IDENTIFIER_SET_LENGTH = GetArraySize(SPECIAL_IDENTIFIER_CHARS);
const int KEYWORDS_SET_LENGTH = GetArraySize(KEYWORDS);
const int PUNCTUATION_SET_LENGTH = GetArraySize(PUNCTUATION);
const int START_OPERATOR_SYMBOL_SET_LENGTH = GetArraySize(START_OPERATOR_SYMBOL);

const int DIGITS_MIN_CODE = 48;
const int DIGITS_MAX_CODE = 57;

const int UPPER_CASE_LETTERS_MIN_CODE = 97;
const int UPPER_CASE_LETTERS_MAX_CODE = 122;
const int LOWER_CASE_LETTERS_MIN_CODE = 65;
const int LOWER_CASE_LETTERS_MAX_CODE = 90;




const std::string POSSIBLE_KEYWORDS_TO_ADD[] = {
	// Control flow
	"switch", "case", "default",

	// Namespaces
	"namespace", "using",

	// Type specifiers
	"void", "wchar_t", "char8_t", "char16_t", "char32_t",
	"short", "long", "signed", "unsigned", "double",

	// Exception handling
	"try", "catch", "throw",

	// Storage class specifiers
	"auto", "register", "static", "extern", "mutable", "thread_local",

	// Type modifiers
	"const", "volatile", "restrict", // (restrict is C99, not standard C++)
	"constexpr", "consteval", "constinit",

	// Classes / struct / enum
	"class", "struct", "union", "enum",

	// Templates
	"template", "typename", "concept", "requires",

	// Access specifiers
	"public", "private", "protected",

	// Inheritance / polymorphism
	"virtual", "override", "final",

	// Operators and casting
	"operator", "explicit", "friend",
	"new", "delete",
	"sizeof", "alignof", "decltype", "typeid",
	"static_cast", "dynamic_cast", "const_cast", "reinterpret_cast",

	// Misc
	"this"
};