#pragma once

#define HOOK_DECL(ret, call, name, ...) \
		static ret call HOOKED_##name(__VA_ARGS__); \
		ret HOOKED_##name##_Func(__VA_ARGS__); \
		typedef	ret(call *_##name) (__VA_ARGS__); \
		_##name ORIG_##name;

#define HOOK_DEF_2(ret, call, name, t1, n1, t2, n2) \
	ret call HOOKED_##name(t1 n1, t2 n2) { \
		return HOOKED_##name##_Func(n1, n2); \
	} \
	ret HOOKED_##name##_Func(t1 n1, t2 n2)