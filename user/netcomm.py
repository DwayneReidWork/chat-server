import socket

def server_setup():
    host = '127.0.0.1'
    port = 4444
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    print(f"Connected to {host}:{port}")
    return s

def send_all(sock: socket.socket, data: bytes) -> bool:
    send_b = 0
    exp_len = len(data)

    while send_b < exp_len:
        try:
            sent = sock.send(data[send_b:])
            if sent == 0:
                print("Connection closed by peer.")
                return False
            send_b += sent
        except Exception as e:
            print(f"Error sending data: {e}")
            return False
    return True


def recv_expected(sock: socket.socket, expected_len: int) -> bytes:
    recv_b = b''
    while len(recv_b) < expected_len:
        chunk = sock.recv(expected_len - len(recv_b))
        if chunk == b'':
            print("Connection closed by peer")
            break
        recv_b += chunk   
    return recv_b
