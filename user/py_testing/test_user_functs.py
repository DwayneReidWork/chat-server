import pytest
from unittest.mock import Mock
from user.__main__ import send_msg, receive_forever, create_client, shutdown_event
from user.netcomm import recv_expected, send_all

def test_shutdown_event(monkeypatch):
    s = create_client()
    sample_inputs = iter(['hello', 'test', 'exit'])
    monkeypatch.setattr('builtins.input', lambda _: next(sample_inputs))

    send_msg(s)
    assert shutdown_event.is_set()
    s.close()

def test_shutdown_event_fail(monkeypatch):
    s = create_client()
    shutdown_event.clear()
    #following commands shouldn't set shutdown event
    sample_inputs = iter(['hello', 'test'])
    monkeypatch.setattr('builtins.input', lambda _: next(sample_inputs))
    send_msg(s)
    assert not shutdown_event.is_set()

    #set shutdown event to continue testing
    sample_inputs = iter(['exit'])
    send_msg(s)
    monkeypatch.setattr('builtins.input', lambda _: next(sample_inputs))
    assert shutdown_event.is_set()
    s.close()

def test_send_all_valid():
    s = create_client()
    data = b'test'  
    assert send_all(s, data) == True

def test_send_all_bad_socket():
    data = b'test'
    #bad socket
    assert send_all(None, data) == False

def test_send_all_bad_data():
    s = create_client()
    #bad data
    empty_string_fail = " "
    assert send_all(s, empty_string_fail) == False
    s.close()

def test_send_msg_bad_socket():
    assert send_msg(None) == None

def test_recv_forever_bad_socket():
    assert receive_forever(None) == None

@pytest.fixture
def mock_socket_class():
    mock_socket = Mock()
    mock_socket.recv.return_value = b'mocked_data'
    return mock_socket

def test_recv_expected_valid(mock_socket_class):
    result = recv_expected(mock_socket_class, 10)
    assert isinstance(result, bytes)
    assert result == b'mocked_data'


def test_recv_expected_bad_socket():
    # Bad Socket
    assert recv_expected(None, 10) == None

def test_recv_expected_bad_length(mock_socket_class):
    # Invalid expected_len
    assert recv_expected(mock_socket_class, None) == None