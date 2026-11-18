import os
import threading
from . import netcomm as n

BUFFER = 1024
MAX_LENGTH = 1024

shutdown_event = threading.Event()

def create_client():
    return n.server_setup()

def send_msg(client_socket):
    if client_socket is None:
        return None
    
    while not shutdown_event.is_set():
        try:
            message = input("You: ")
            if message.lower() == 'exit':
                shutdown_event.set()
                break
            message += "\0"
            success = n.send_all(client_socket, message.encode())
            if not success:
                print("Failed to send message.")
        except Exception as e:
            print(f"Send Failed: {e}")
            break

def receive_forever(client_socket):
    if client_socket is None:
        return None
    
    while not shutdown_event.is_set():
        buffer = b''
        try:
            while len(buffer) < MAX_LENGTH:
                byte = client_socket.recv(1)
                if not byte:
                    print("\nNo data recieved.")
                    shutdown_event.set()
                    break
                buffer += byte
            
                if byte == b'\0':
                    break

            if len(buffer) == 0:
                print("\nError: empty buffer.")
                break
            
            else: 
                print(f'\n{buffer.decode()}\nYou: ', end='', flush=True)

        except ConnectionError as e:
            print(f"\nConnection Lost: {e}")
            shutdown_event.set()
        except KeyboardInterrupt as e:
            print(f'Keyboard Interruption!')
            shutdown_event.set()
            os._exit(0)
        finally:
            if shutdown_event.is_set() and not client_socket._closed:
                client_socket.close()
        break

def run_chat_client():
    client_socket = create_client()
    threading.Thread(target=receive_forever, args=(client_socket,), daemon=True).start()
    threading.Thread(target=send_msg, args=(client_socket,)).start()

if __name__ == "__main__":
    run_chat_client()