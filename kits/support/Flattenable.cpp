#include "Flattenable.h"

BFlattenable::~BFlattenable()
{
}

bool BFlattenable::AllowsTypeCode(type_code code) const
{
	return TypeCode() == code;
}
