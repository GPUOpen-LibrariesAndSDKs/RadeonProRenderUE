#include "RPRXHelpers.h"
#include "RprSupport.h"

namespace RPRX
{
	void DeleteContext(FContext context)
	{
		rprxDeleteContext(context);
	}

}