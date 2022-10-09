#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#include <doctest/doctest.h>

int main(int argc, char** argv)
{
	return doctest::Context(argc, argv).run();
}
