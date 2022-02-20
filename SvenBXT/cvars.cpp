#include "cvars.hpp"

namespace CVars
{
	// Clientside CVars
	CVarWrapper bxt_hud("bxt_hud", "1");

	const std::vector<CVarWrapper*> allCVars =
	{
		&bxt_hud
	};
}