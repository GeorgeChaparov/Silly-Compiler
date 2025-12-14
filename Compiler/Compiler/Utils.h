#pragma once
#include <charconv>
#include <string>

template <typename T, size_t N>
constexpr size_t GetArraySize(const T(&)[N]) noexcept {
	return N;
}

inline bool IsInteger(std::string string) {
	int value;
	auto [ptr, ec] = std::from_chars(string.data(), string.data() + string.size(), value);

	return ec == std::errc{} && ptr == string.data() + string.size();
}

inline int ConvertToInt(std::string string) {
	int value;
	auto [ptr, ec] = std::from_chars(string.data(), string.data() + string.size(), value);

	return value;
}