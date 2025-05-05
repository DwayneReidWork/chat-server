#include "server.h"
#include "test_server.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <unistd.h>

#include <cmocka.h>

server_t server = {0};
_Atomic bool should_quit = false;

static void test_setup_server(void **state)
{
    int result = setup_server(&server);
    assert_int_equal(result, 0);
}

static void test_setup_server_invalid_server(void **state)
{
    int result = setup_server(NULL);
    assert_int_equal(result, -1);
}

static void test_handle_client(void **state)
{
    int *socket_fd = malloc(sizeof(int));
    assert_non_null(socket_fd); // check that malloc succeded
    *socket_fd = 1;

    void *result = handle_client(socket_fd);
    assert_null(result);
}

static void test_handle_client_invalid_arg(void **state)
{
    void *result = handle_client(NULL);
    assert_null(result);
}

int main(void)
{
    const struct CMUnitTest unit_tests[] = {

        // { } REQUIRED Edit the test list here
        cmocka_unit_test(test_setup_server),
        cmocka_unit_test(test_setup_server_invalid_server),
        cmocka_unit_test(test_handle_client),
        cmocka_unit_test(test_handle_client_invalid_arg),
    };
    return cmocka_run_group_tests(unit_tests, NULL, NULL);
}