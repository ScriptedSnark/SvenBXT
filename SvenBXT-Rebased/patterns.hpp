/**
 * Copyright - xWhitey ~ ScriptedSnark, 2022.
 * patterns.hpp - Our pattern-system
 */

#ifdef PATTERNS_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in patterns.hpp
#else //PATTERNS_HPP_RECURSE_GUARD

#define PATTERNS_HPP_RECURSE_GUARD

#ifndef PATTERNS_HPP_GUARD
#define PATTERNS_HPP_GUARD
#pragma once

#include "MemUtils.hpp"

#define PATTERNS(name, ...) const auto name = new CIntelligentPattern(__VA_ARGS__);

typedef unsigned char uint8_t;

const static std::vector<std::string> split(const std::string& s, char seperator) {
	std::vector<std::string> output;

	int prev_pos = 0, pos = 0;

	while ((pos = s.find(seperator, pos)) != std::string::npos) {
		std::string substring(s.substr(prev_pos, pos - prev_pos));

		output.push_back(substring);

		prev_pos = ++pos;
	}

	output.push_back(s.substr(prev_pos, pos - prev_pos));

	return output;
}

constexpr const uint8_t hex(char c) {
	return (c >= '0' && c <= '9') ? static_cast<uint8_t>(c - '0')
		: (c >= 'a' && c <= 'f') ? static_cast<uint8_t>(c - 'a' + 10)
		: (c >= 'A' && c <= 'F') ? static_cast<uint8_t>(c - 'A' + 10)
		: throw std::domain_error("not a hex digit");
}

typedef struct CIntelligentPattern {
	explicit CIntelligentPattern(std::string peidPattern, std::string version) {
		std::vector<uint8_t> pattern = {};
		std::string mask = "";
		std::map<std::vector<uint8_t>, std::string> _pattern;
		auto splitten = split(peidPattern, ' ');
		for (unsigned int idx = 0; idx < splitten.size(); idx++) {
			if (splitten[idx] == "??") {
				mask.append("?");
				pattern.push_back(0xCC);
			}
			else {
				mask.append("x");
				pattern.push_back(hex(splitten[idx][0]) * 16 + hex(splitten[idx][1]));
			}
		}
		_pattern.insert(std::make_pair(pattern, mask));
		std::map<decltype(_pattern), std::string> final_pattern;
		final_pattern.insert(std::make_pair(_pattern, version));
		patterns.push_back(final_pattern);
	}
	explicit CIntelligentPattern(std::vector<std::string> peidPatterns, std::vector<std::string> versions) {
		for (unsigned int idx = 0; idx < peidPatterns.size(); idx++) {
			auto peidPattern = peidPatterns.at(idx);
			auto version = versions.at(idx);
			std::vector<uint8_t> pattern = {};
			std::string mask = "";
			std::map<std::vector<uint8_t>, std::string> _pattern;
			auto splitten = split(peidPattern, ' ');
			for (unsigned int idx = 0; idx < splitten.size(); idx++) {
				if (splitten[idx] == "??") {
					mask.append("?");
					pattern.push_back(0xCC);
				}
				else {
					mask.append("x");
					pattern.push_back(hex(splitten[idx][0]) * 16 + hex(splitten[idx][1]));
				}
			}
			_pattern.insert(std::make_pair(pattern, mask));
			std::map<decltype(_pattern), std::string> final_pattern;
			final_pattern.insert(std::make_pair(_pattern, version));
			patterns.push_back(final_pattern);
		}
	}
	std::vector<std::map<std::map<std::vector<uint8_t>, std::string>, std::string>> patterns;
	//vector<map<map<pattern, mask>, version>
} CIntelligentPattern;

/**
 * @author: xWhitey
 * @comment: _Module can't be nullptr!
 */
_Success_(return == true) template<class ResultType> bool FindAsync(_In_ void* _Module, _In_ std::vector<uint8_t> _Pattern, _In_ const char* _Mask, _Out_ ResultType& _Result) {
	if (!_Module) return false;
	if (!_Mask[0] || !_Mask) return false;

	auto module_size = MemUtils::GetModuleSize(_Module);

	unsigned int pattern_length = strlen(_Mask);
	unsigned char* based = reinterpret_cast<unsigned char*>(_Module);

	for (unsigned int idx = 0; idx < module_size - pattern_length; ++idx) {
		bool found = true;
		for (unsigned int i = 0; i < pattern_length; i++) {
			if (_Mask[i] != '?' && based[idx + i] != _Pattern[i]) {
				found = false;
				break;
			}
		}

		if (found) {
			_Result = ((ResultType)&based[idx]);

			return true;
		}
	}

	return false;
}

template<class ResultType> void FindAsync(_In_ void* _Module, _In_ CIntelligentPattern* _Pattern, _Out_ ResultType& _Result, _In_ std::function<void(std::string)> _ToDoUponFound, _In_ std::function<void()> _ToDoIfCouldntFind) {
	bool found = false;
	for (auto p : _Pattern->patterns) {
		for (auto val : p) {
			auto pattern = val.first;
			auto version = val.second;
			for (auto m : pattern) {
				auto bytes = m.first;
				auto mask = m.second;
				if (found = FindAsync(_Module, bytes, mask.c_str(), _Result)) {
					_ToDoUponFound(version);
					return;
				}
			}
		}
	}
	if (!found) _ToDoIfCouldntFind();
}

template<class ResultType> bool FindAsync(_In_ void* _Module, _In_ CIntelligentPattern* _Pattern, _Out_ ResultType& _Result) {
	for (auto p : _Pattern->patterns) {
		for (auto val : p) {
			auto pattern = val.first;
			auto version = val.second;
			for (auto m : pattern) {
				auto bytes = m.first;
				auto mask = m.second;
				return FindAsync(_Module, bytes, mask.c_str(), _Result);
			}
		}
	}
}

/**
 * Warning! You must follow this example in case of creating custom pattern: pattern1, pattern2, mask1, mask2, version1, version2
 * Otherwise you'll break the whole pattern-system!
 */
namespace patterns {
	namespace engine {
		PATTERNS(Cbuf_AddText,
			std::vector<std::string>{ "56 8B 74 24 ?? 56 E8 ?? ?? ?? ?? 03 05 ?? ?? ?? ?? 83 C4 04 3B 05 ?? ?? ?? ?? 7C ?? 5E C7 44 24 04 68 34",
			"55 8B EC 8B 45 08 68 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 08 5D C3 90 90 90 90 90 90 90 90 90 90 55 8B EC 56 57 8B 7D 08 57 E8",
			"55 8B EC 56 8B 75 08 56 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 83 C4 04 03 C8 A1 ?? ?? ?? ?? 3B C8 7C 10 68",
			"56 8B 74 24 08 56 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 83 C4 04 03 C8 A1 ?? ?? ?? ?? 3B C8 7C",
			"56 8B 74 24 08 68 ?? ?? ?? ?? 56 E8 ?? ?? ?? ?? 83 C4 08 85 C0 75 07 E8 ?? ?? ?? ?? 5E C3 68 ?? ?? ?? ?? 56 E8" }, { "Sven-5.25", "HL-SteamPipe-8183", "HL-SteamPipe", "HL-NGHL", "HL-WON"});
		PATTERNS(Cbuf_InsertText, std::vector<std::string>{ "53 8B 5C 24 ?? 56 8B 35 ?? ?? ?? ?? 53 E8 ?? ?? ?? ?? 03 C6",
			"55 8B EC 8B 45 08 6A 00 68 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 0C 5D C3",
			"55 8B EC 53 8B 5D 08 56 8B 35 ?? ?? ?? ?? 57 53 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 03 C6 83 C4 04 3B C1 7C 12 68 ?? ?? ?? ?? E8",
			"53 8B 5C 24 08 56 8B 35 ?? ?? ?? ?? 57 53 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 03 C6 83 C4 04 3B C1 7C"}, {"Sven-5.25", "HL-SteamPipe-8183", "HL-SteamPipe", "HL-NGHL"});
		PATTERNS(SPR_Set, std::vector<std::string>{ "8B 44 24 ?? 83 C0 FF 78 ?? 3B 05 ?? ?? ?? ?? 7D ?? 8D 0C ?? A1 ?? ?? ?? ?? 8D 04 ?? 85 C0 74 ?? 8B 00 85 C0 74 ?? 8B 80 ?? ?? ?? ?? A3 ?? ?? ?? ??",
			"55 8B EC 83 EC 08 8D 45 ?? 8D 4D ?? 50 8D 55 ?? 51 8D 45 ?? 52 50",
			"83 EC 08 8D 44 24 ?? 8D 4C 24 ?? 8D 54 24 ?? 50 51 8D 44 24",
			"8B 44 24 ?? 83 EC 08 50"}, {"Sven-5.25", "HL-SteamPipe", "HL-4554", "HL-WON-1712"});
		PATTERNS(Draw_FillRGBA, std::vector<std::string>{ "56 68 E1 0D 00 00 FF 15 ?? ?? ?? ?? 68 E2 0B 00 00 FF 15 ?? ?? ?? ?? D9 05 ?? ?? ?? ?? 51 D9 1C 24 68 00 22 00 00 68 00 23 00 00 FF 15 ?? ?? ?? ?? 6A 01",
			"55 8B EC 83 EC 08 8D 45 ?? 8D 4D ?? 50 8D 55 ?? 51 8D 45 ?? 52 8D 4D ?? 50 8D 55 ?? 51 8D 45 ?? 52 8D 4D ?? 50 51 FF 15 ?? ?? ?? ?? 83 C4 20 68 E1 0D 00 00 FF 15 ?? ?? ?? ?? 68 E2 0B 00 00 FF 15 ?? ?? ?? ?? 68 00 00 04 46 68 00 22 00 00 68 00 23 00 00 FF 15 ?? ?? ?? ?? 6A 01",
			"83 EC 08 8D 44 24 ?? 8D 4C 24 ?? 50 8D 54 24 ?? 51 8D 44 24 ?? 52 8D 4C 24 ?? 50 8D 54 24 ?? 51 8D 44 24 ?? 52 8D 4C 24 ?? 50 51 FF 15 ?? ?? ?? ?? 83 C4 20 68 E1 0D 00 00 FF 15 ?? ?? ?? ?? 68 E2 0B 00 00 FF 15 ?? ?? ?? ?? 68 00 00 04 46 68 00 22 00 00 68 00 23 00 00 FF 15 ?? ?? ?? ?? 6A 01",
			"83 EC 08 53 56 57 68 E1 0D 00 00 FF 15"}, {"Sven-5.25", "HL-SteamPipe", "HL-4554", "HL-WON-1712"});
	}
	namespace client {
		PATTERNS(ScaleColors, std::vector<std::string>{ "66 0F 6E 4C 24 ?? 8B 4C 24", "DB 44 24 ?? 56 8B 74 24 ??" }, { "Sven-5.25", "Sven-8458" });
		PATTERNS(SMR_StudioRenderModel, "83 EC 44 A1 ?? ?? ?? ?? 33 C4 89 44 24 ?? 53 55", "Sven-5.25");
	}
}

#endif //PATTERNS_HPP_GUARD

#undef PATTERNS_HPP_RECURSE_GUARD
#endif //PATTERNS_HPP_RECURSE_GUARD
