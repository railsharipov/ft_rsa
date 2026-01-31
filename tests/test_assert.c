#include <common.h>
#include "test.h"

int	test_assert(int bool, const char *expr)
{
	if (bool) {
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
