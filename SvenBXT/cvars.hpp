#pragma once
#include <vector>
#include <string>
#include <cassert>
#include "hlsdk_mini.hpp"

class CVarWrapper
{
public:
	CVarWrapper() : m_Reference(true) {}
	CVarWrapper(const char* name, const char* string);
	~CVarWrapper();
	void MarkAsStale();
	void Refresh();
	void Assign(cvar_t* cvar);
	cvar_t* GetPointer() const;

	bool IsEmpty() const;
	bool GetBool() const;
	int GetInt() const;
	float GetFloat() const;
	std::string GetString() const;

	// Only use before registering!
	void Set(const char* string);

protected:
	cvar_t* m_CVar = nullptr;
	const char* m_String = nullptr;
	bool m_StaleString = false;
	bool m_Reference = false;
};

inline CVarWrapper::CVarWrapper(const char* name, const char* string)
{
	m_CVar = new cvar_t;
	m_CVar->name = const_cast<char*>(name);
	m_String = string;
	m_CVar->string = const_cast<char*>(m_String);
	m_CVar->flags = 0;
	m_CVar->value = static_cast<float>(std::atof(m_String));
	m_CVar->next = nullptr;
}

inline CVarWrapper::~CVarWrapper()
{
	if (!m_Reference)
		delete m_CVar;
}

inline void CVarWrapper::MarkAsStale()
{
	assert(!m_Reference);
	m_StaleString = true;
}

inline void CVarWrapper::Refresh()
{
	if (m_StaleString)
	{
		assert(!m_Reference);
		m_StaleString = false;
		m_CVar->string = const_cast<char*>(m_String);
		m_CVar->value = static_cast<float>(std::atof(m_String));
	}
	else if (m_Reference)
		m_CVar = nullptr;
}

inline void CVarWrapper::Assign(cvar_t* cvar)
{
	assert(m_Reference);
	m_CVar = cvar;
}

inline cvar_t* CVarWrapper::GetPointer() const
{
	return m_CVar;
}

inline bool CVarWrapper::IsEmpty() const
{
	return !m_CVar || !m_CVar->string[0];
}

inline bool CVarWrapper::GetBool() const
{
	return m_CVar && (m_CVar->value != 0.0f);
}

inline int CVarWrapper::GetInt() const
{
	return m_CVar ? atoi(m_CVar->string) : 0;
}

inline float CVarWrapper::GetFloat() const
{
	return m_CVar ? m_CVar->value : 0.0f;
}

inline std::string CVarWrapper::GetString() const
{
	if (!m_CVar)
		return std::string();
	return std::string(m_CVar->string);
}

inline void CVarWrapper::Set(const char* string)
{
	assert(!m_Reference);

	m_String = string;
	m_CVar->string = const_cast<char*>(m_String);
	m_CVar->value = static_cast<float>(std::atof(m_String));
}

namespace CVars
{
	// Engine CVars
	extern CVarWrapper bxt_hud;
	extern CVarWrapper bxt_hud_color;
	extern CVarWrapper bxt_hud_speedometer;
	extern CVarWrapper bxt_hud_speedometer_offset;
	extern CVarWrapper bxt_hud_speedometer_anchor;
	extern const std::vector<CVarWrapper*> allCVars;
}