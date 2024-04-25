import socket
from time import sleep

def send_string_to_ip( ip, port):
    # Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Connect to the server
        s.connect((ip, port))
        #s.sendall("DELETE /../hui HTTP/1.1\r\nHost:www.domain1.com\r\n\r\n".encode())

        # Send a put request to the server
        s.sendall("PUT /test227 HTTP/1.1\r\nHost:www.domain1.com\r\nContent-Length:6\r\n\r\nPENISPENIS\n\n".encode())
        sleep(1)

        response = s.recv(1024)  # Adjust buffer size as needed
        print(response.decode())
        s.close()

# Example usage:
server_ip = "127.0.0.1"  # Replace with the server's IP address
server_port = 8080  # Replace with the server's port

send_string_to_ip( server_ip, server_port)
