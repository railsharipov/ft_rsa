#include <common.h>
#include <logger.h>
#include "test.h"

int	test_assert(bool condition, const char *expr)
{
	if (condition) {
#ifdef TEST_ENABLE_ASSERT_PASS_LOG
		TEST_LOG(INFO, TXT_GREEN("ASSERT PASS") " (%s)", expr);
#endif
		return (SSL_OK);
	} else {
		TEST_LOG(ERROR, TXT_RED("ASSERT FAIL") " (%s)", expr);

		if (errno) {
			TEST_LOG(ERROR, "%s", strerror(errno));
		}
		return (SSL_ERR);
	}
}
