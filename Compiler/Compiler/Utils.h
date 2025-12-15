#pragma once
#include <charconv>
#include <string>

template <typename T, size_t N>
constexpr size_t GetArraySize(const T(&)[N]) noexcept {
	return N;
}

inline bool IsInteger(std::string _string) {
	int value;
	auto [ptr, ec] = std::from_chars(_string.data(), _string.data() + _string.size(), value);

	return ec == std::errc{} && ptr == _string.data() + _string.size();
}

inline int ConvertToInt(std::string _string) {
	int value;
	auto [ptr, ec] = std::from_chars(_string.data(), _string.data() + _string.size(), value);

	return value;
}