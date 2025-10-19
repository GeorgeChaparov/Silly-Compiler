#include <string>
#include "Utils.h"
const char SEPARATORS[] = { ' ', '\n', '\r', '\t'};

const char PUNCTUATION[] = {'(', ')'};

const int SEPARATORS_SET_LENGTH = GetArraySize(SEPARATORS);
const int PUNCTUATION_SET_LENGTH = GetArraySize(PUNCTUATION);

const int DIGITS_MIN_CODE = 48;
const int DIGITS_MAX_CODE = 57;

const int UPPER_CASE_LETTERS_MIN_CODE = 97;
const int UPPER_CASE_LETTERS_MAX_CODE = 122;
const int LOWER_CASE_LETTERS_MIN_CODE = 65;
const int LOWER_CASE_LETTERS_MAX_CODE = 90;