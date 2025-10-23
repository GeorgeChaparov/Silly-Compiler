#include <string>
#include "Utils.h"
const char SEPARATORS[] = { ' ', '\r', '\t'};

const char PUNCTUATION[] = {'(', ')'};

const int SEPARATORS_SET_LENGTH = GetArraySize(SEPARATORS);
const int PUNCTUATION_SET_LENGTH = GetArraySize(PUNCTUATION);

const int DIGITS_MIN_ASCII_CODE = 48;
const int DIGITS_MAX_ASCII_CODE = 57;

const int UPPER_CASE_LETTERS_MIN_ASCII_CODE = 97;
const int UPPER_CASE_LETTERS_MAX_ASCII_CODE = 122;
const int LOWER_CASE_LETTERS_MIN_ASCII_CODE = 65;
const int LOWER_CASE_LETTERS_MAX_ASCII_CODE = 90;

const int SYMBOL_TABLE_SIZE = 10000;

const int UNEXPECTED_LEXICAL_ERROR_CODE = SYMBOL_TABLE_SIZE + 2;
const int END_OF_FILE_CODE = SYMBOL_TABLE_SIZE + 1;