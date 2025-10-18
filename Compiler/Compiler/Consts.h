#include <string>
#include "Utils.h"

const std::string OPERATORS[] = { 
	"::::", //	+
	";;;;", //	-
	"::;;", //	*
	";;::",	//	/
};

// Saved as operators
const std::string COMPARESON[] = {
	":;",	//	>
	":::",	//	==
	"::"	//	=
};

const char SEPARATORS[] = { ' ', '\n', '\r', '\t', '\0'};

const std::string KEYWORDS[] = {
	 ":-",	//	cout
	 "-:"	//	cin
};

const char PUNCTUATION[] = {'(', ')'};

// Saved as keywords
const std::string FLOW_CONTROL[] = {
	";-",	//	continue
	"-;",	//	break
	":",	//	if / while
	";;"	//	else-if / else
};

const int SEPARATORS_SET_LENGTH = GetArraySize(SEPARATORS);
const int OPERATORS_SET_LENGTH = GetArraySize(OPERATORS);
const int KEYWORDS_SET_LENGTH = GetArraySize(KEYWORDS);
const int PUNCTUATION_SET_LENGTH = GetArraySize(PUNCTUATION);
const int FLOW_CONTROL_SET_LENGTH = GetArraySize(FLOW_CONTROL);
const int COMPARESON_SET_LENGTH = GetArraySize(COMPARESON);

const int DIGITS_MIN_CODE = 48;
const int DIGITS_MAX_CODE = 57;

const int UPPER_CASE_LETTERS_MIN_CODE = 97;
const int UPPER_CASE_LETTERS_MAX_CODE = 122;
const int LOWER_CASE_LETTERS_MIN_CODE = 65;
const int LOWER_CASE_LETTERS_MAX_CODE = 90;