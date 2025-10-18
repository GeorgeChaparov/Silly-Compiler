#include <string>
#include "Utils.h"

const std::string OPERATORS[] = { 
	"::::", //	+
	";;;;", //	-
	"::;;", //	*
	";;::",	//	/
	":;",	//	>
	":::",	//	==
	"::"	//	=
};

const char SEPARATORS[] = { ' ', '\n', '\r', '\t' };
const std::string KEYWORDS[] = {
	 ":",	//	if / while
	 ";;"	//	else-if / else
	 "(;)",	//	cout
	 "(:)"	//	cin
};
const char PUNCTUATION[] = {'(', ')'};

const int SEPARATORS_SET_LENGTH = GetArraySize(SEPARATORS);
const int OPERATORS_SET_LENGTH = GetArraySize(OPERATORS);
const int KEYWORDS_SET_LENGTH = GetArraySize(KEYWORDS);
const int PUNCTUATION_SET_LENGTH = GetArraySize(PUNCTUATION);

const int DIGITS_MIN_CODE = 48;
const int DIGITS_MAX_CODE = 57;

const int UPPER_CASE_LETTERS_MIN_CODE = 97;
const int UPPER_CASE_LETTERS_MAX_CODE = 122;
const int LOWER_CASE_LETTERS_MIN_CODE = 65;
const int LOWER_CASE_LETTERS_MAX_CODE = 90;