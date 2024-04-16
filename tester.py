import socket
from time import sleep

def send_string_to_ip( ip, port):
    # Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Connect to the server
        s.connect((ip, port))
        
        # Send the string
        # s.sendall("PUT /sanoj1 HTTP/1.1\r\nHost:ohcko.com\r\nContent-Length: 20\r\n\r\n".encode())
        # s.sendall("123456789123456789".encode())

        # sleep(3)
        # s.sendall("popa".encode())

        
        # # Receive response from the server
        # response = s.recv(1024)

        # s.sendall("PUT /sanoj2 HTTP/1.1\r\nHost:ohcko.com\r\nContent-Length: 20\r\n\r\n".encode())
        # s.sendall("123456789123456789".encode())
        # s.sendall("popa".encode())
        s.sendall("GET / HTTP/1.1\r\nHost:ohcko.com\r\n\r\n".encode())
        response = s.recv(1024)  # Adjust buffer size as needed
        print('Response from server:', response.decode())
        s.sendall("GET / HTTP/1.1\r\nHost:ohcko.com\r\n\r\n".encode())
        print('Request2 sent')
        response = s.recv(1024)  # Adjust buffer size as needed
        # Decode and print the response
        print('Response from server:', response.decode())
        s.close()

# Example usage:
string_to_send = "PUT /sanoj HTTP/1.1\r\nHost:ohcko.com\r\nContent-Length: 10\r\n\r\n"
server_ip = "127.0.0.1"  # Replace with the server's IP address
server_port = 8080  # Replace with the server's port

send_string_to_ip( server_ip, server_port)
