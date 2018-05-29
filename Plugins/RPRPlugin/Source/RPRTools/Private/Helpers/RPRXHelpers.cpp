#include "RPRXHelpers.h"

namespace RPRX
{
	void DeleteContext(FContext context)
	{
		rprxDeleteContext(context);
	}

}