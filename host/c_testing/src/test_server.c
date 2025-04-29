#include "server.h"
#include "test_server.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

int result;
server_t server = {0};
_Atomic bool should_quit = false;

static void test_setup_server(void **state)
{
    result = setup_server(&server);
    assert_int_equal(result, 0);
}

static void test_setup_server_invalid_server(void **state)
{
    result = setup_server(&server);
    assert_int_equal(result, -1);
}

int main(void)
{
    const struct CMUnitTest unit_tests[] = {

        // { } REQUIRED Edit the test list here
        cmocka_unit_test(test_setup_server),
        cmocka_unit_test(test_setup_server_invalid_server),
    };

    return cmocka_run_group_tests(unit_tests, NULL, NULL);
}