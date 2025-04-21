import sys
import threading
from . import netcomm as n
#import netcomm as n # used for debugging

BUFFER = 1024
MAX_LENGTH = 1024

client_socket = n.server_setup()

def send_msg():
    message = input("Please enter message to send to the chat room or type 'exit' to quit: ")
    if message.lower() == 'exit':
        sys.exit()
        
    try:
        message += "\0"
        success = n.send_all(client_socket, message.encode())
        if success:
            print("Message sent successfully!")
        else:
            print("Failed to send message.") 
    except Exception as e:
        print(f"Send Failed: {e}")

def receive_forever():
    while True:
        buffer = b''
        try:
            while len(buffer) < MAX_LENGTH:
                byte = client_socket.recv(1)
                if not byte:
                    print("Connection closed by peer.")
                    break
                buffer += byte
                if byte == b'\0':
                    break

            if len(buffer) == 0:
                print("Error: empty buffer.")
                break

            print(f'Buffer contents: {buffer}')

        except ConnectionError as e:
            print(f"Connection Lost: {e}")
            break
        except OSError as e:
            print(f"OS Error: {e}")
            break
        except Exception as e:
            print(f"Unexpected error: {e}")
            break

    client_socket.close()
    sys.exit()


# new threads for receiving messages
threading.Thread(target=receive_forever, daemon=True).start()

while True:
    send_msg()