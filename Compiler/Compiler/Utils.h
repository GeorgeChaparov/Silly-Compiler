#pragma once

template <typename T, size_t N>
constexpr size_t GetArraySize(const T(&)[N]) noexcept {
	return N;
}