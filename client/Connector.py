
import socket
import select

class Connector():

    def __init__(self, self_ip='127.0.0.1', self_port=8887, ip='127.0.0.1', port=8888) -> None:
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.target_port = port
        self.target_ip = ip
        self.sock.bind((self_ip, self_port))
        self.sock.setblocking(0)

        
    def send(self, data) -> None:
        self.sock.sendto(data, (self.target_ip, self.target_port))

    def listen(self, timeout_sec=1):
        ready = select.select([self.sock], [], [], timeout_sec)
        data = None
        if ready[0]:
            data = self.sock.recv(1024)
        return data