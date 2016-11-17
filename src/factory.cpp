#include "factory.h"
#include "Log.h"

void *srcFactory::getImpl(const char *name, int *ret)
{
	try
	{
		return factory(name, ret);
	}
	catch(...)
	{
		Log::Fatal("Unable to get interface '%s'", name);
	}
	return nullptr;
}
