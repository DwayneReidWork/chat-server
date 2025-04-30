#pragma once

static void test_setup_server(void **state);
static void test_setup_server_invalid_server(void **state);
static void test_handle_client(void **state);
static void test_handle_client_invalid_arg(void **state);
void *run_accept_thread(void *arg);
// static void test_accept_connections(void **state);
// static void test_accept_connections_invalid_server(void **state);