import socket

def server_setup():
    host = '127.0.0.1'
    port = 4444
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    print(f"Connected to {host}:{port}")
    return s

def send_len(sock: socket.socket, msg_size: int) -> bool:
    send_len = 0

    try:
        send_len += sock.send(str(msg_size).encode())
    except:
        return False
    return True

def send_all(sock: socket.socket, data: bytes) -> bool:
    send_b = 0
    exp_len = len(data)

    while send_b < exp_len:
        try:
            send_b += sock.send(data[send_b:])
        except:
            return False
    return True

def recv_expected(sock: socket.socket, expected_len: int) -> bytes:
    recv_b = b''
    while len(recv_b) < expected_len:
        chunk = sock.recv(expected_len - len(recv_b))
        if not chunk:
            raise ConnectionError("Socket closed before receiving expected data")       
        recv_b += chunk   
    return recv_b