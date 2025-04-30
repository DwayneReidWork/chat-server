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

// void *run_accept_thread(void *arg)
// {
//     accept_connections((server_t *)arg);
//     return NULL;
// }
//
// static void test_accept_connections(void **state)
// {
//     pthread_t t;
//     should_quit = false;
//     pthread_create(&t, NULL, run_accept_thread, &server);
//     sleep(1);
//     should_quit = true;
//     pthread_join(t, NULL);
//     assert_true(true); // make sure its not hanfing
// }
//
// static void test_accept_connections_invalid_server(void **state)
// {
//     int result = accept_connections(&server);
//     assert_int_equal(result, 0);
//     should_quit = true;
// }

int main(void)
{
    const struct CMUnitTest unit_tests[] = {

        // { } REQUIRED Edit the test list here
        cmocka_unit_test(test_setup_server),
        cmocka_unit_test(test_setup_server_invalid_server),
        cmocka_unit_test(test_handle_client),
        cmocka_unit_test(test_handle_client_invalid_arg),
        // cmocka_unit_test(test_accept_connections),
        // cmocka_unit_test(test_accept_connections_invalid_server),
    };
    return cmocka_run_group_tests(unit_tests, NULL, NULL);
}