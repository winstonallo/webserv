import socket

def send_string_to_ip(string, ip, port):
    # Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Connect to the server
        s.connect((ip, port))
        
        # Send the string
        s.sendall(string.encode())
        s.sendall("body12345678".encode())
        
        # Receive response from the server
        response = s.recv(1024)  # Adjust buffer size as needed
        
        # Decode and print the response
        print('Response from server:', response.decode())

# Example usage:
string_to_send = "PUT /sanoj HTTP/1.1\r\nHost:ohcko.com\r\nContent-Length: 10\r\n\r\n"
server_ip = "127.0.0.1"  # Replace with the server's IP address
server_port = 8080  # Replace with the server's port

send_string_to_ip(string_to_send, server_ip, server_port)
