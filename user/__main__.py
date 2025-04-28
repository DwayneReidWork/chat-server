import os
import threading
from . import netcomm as n
#import netcomm as n # for debugging

BUFFER = 1024
MAX_LENGTH = 1024

client_socket = n.server_setup()
shutdown_event = threading.Event()

def send_msg():
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

def receive_forever():
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
                    shutdown_event.set()
                    break

            if len(buffer) == 0:
                print("\nError: empty buffer.")
                break
            
            else: 
                print(f'\n{buffer.decode()}\nYou: ', end='', flush=True)


        except ConnectionError as e:
            print(f"\nConnection Lost: {e}")
            shutdown_event.set()
            break
        except KeyboardInterrupt as e:
            print(f'Keyboard Interrupttion: {e}.')
            shutdown_event.set()
            break
        finally:
            if not client_socket._closed:
                client_socket.close()
            os._exit(0)

# Start receiving in a background thread
recv_thread = threading.Thread(target=receive_forever, daemon=True)
send_thread = threading.Thread(target=send_msg)

send_thread.start()
recv_thread.start()

send_thread.join()
recv_thread.join()

shutdown_event.set()
#Run input loop in main thread
#send_msg()
