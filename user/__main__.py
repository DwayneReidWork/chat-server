import sys
import threading
from . import netcomm as n

#import netcomm as n # for debugging

BUFFER = 1024
MAX_LENGTH = 1024

client_socket = n.server_setup()

def send_msg():
    while True:
        try:
            message = input("You: ")
            if message.lower() == 'exit':
                client_socket.close()
                break
                #sys.exit()
            message += "\0"
            success = n.send_all(client_socket, message.encode())
            if not success:
                print("Failed to send message.")
        except Exception as e:
            print(f"Send Failed: {e}")
            break

def receive_forever():
    while True:
        buffer = b''
        try:
            while len(buffer) < MAX_LENGTH:
                byte = client_socket.recv(1)
                if not byte:
                    print("\nNo data recieved.")
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
            break
        except KeyboardInterrupt as e:
            print(f'Keyboard Interrupttion: {e}.')
            break


# Start receiving in a background thread
threading.Thread(target=receive_forever, daemon=True).start()

# Run input loop in main thread
send_msg()
