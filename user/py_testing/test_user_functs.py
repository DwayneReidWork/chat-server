from user.__main__ import send_msg, receive_forever
from user.netcomm import server_setup

s = server_setup()


def test_send_message():
    assert send_msg() == True
    assert send_msg() == True

def test_receive_forever():
    assert receive_forever() == True
    assert receive_forever() == True