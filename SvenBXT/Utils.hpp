#include <array>
#include "MemUtils.h"
#include "patterns.hpp"
#include <functional>

typedef struct Utils {
	Utils(void* handle, void* base, size_t length) {
		this->m_Handle = handle;
		this->m_Base = base;
		this->m_Length = length;
	}

	template<typename Result, size_t N>
	inline auto FindAsync(
		Result& address,
		const std::array<patterns::PatternWrapper, N>& patterns)
	{
		return MemUtils::find_unique_sequence_async(
			reinterpret_cast<uintptr_t&>(address),
			m_Base,
			m_Length,
			patterns.cbegin(),
			patterns.cend());
	}

	template<typename Result, size_t N>
	inline auto FindAsync(
		Result& address,
		const std::array<patterns::PatternWrapper, N>& patterns,
		std::function<void(typename std::array<patterns::PatternWrapper, N>::const_iterator)> onFound)
	{
		return MemUtils::find_unique_sequence_async(
			reinterpret_cast<uintptr_t&>(address),
			m_Base,
			m_Length,
			patterns.cbegin(),
			patterns.cend(),
			onFound);
	}

	template<typename Result, size_t N>
	inline auto FindFunctionAsync(
		Result& address,
		const char* name,
		const std::array<patterns::PatternWrapper, N>& patterns)
	{
		return MemUtils::find_function_async(
			reinterpret_cast<uintptr_t&>(address),
			m_Handle,
			name,
			m_Base,
			m_Length,
			patterns.cbegin(),
			patterns.cend());
	}

	template<typename Result, size_t N>
	inline auto FindFunctionAsync(
		Result& address,
		const char* name,
		const std::array<patterns::PatternWrapper, N>& patterns,
		std::function<void(typename std::array<patterns::PatternWrapper, N>::const_iterator)> onFound)
	{
		return MemUtils::find_function_async(
			reinterpret_cast<uintptr_t&>(address),
			m_Handle,
			name,
			m_Base,
			m_Length,
			patterns.cbegin(),
			patterns.cend(),
			onFound);
	}

protected:
	void* m_Handle;
	void* m_Base;
	size_t m_Length;
} Utils;