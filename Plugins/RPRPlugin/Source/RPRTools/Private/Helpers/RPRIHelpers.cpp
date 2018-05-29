#include "RPRIHelpers.h"
#include "RadeonProRenderInterchange.h"

void RPRI::DeleteContext(FContext context)
{
	rpriFreeContext(context);
}
