import socket


class Sock:

   def __init__(self, hostname):
      self.hostname = hostname

   def connect(self):
      s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
      s.connect((self.hostname))
      self.socket = s
      return self

   def send(self, content):
      self.socket.sendall(bytes(content + "\n", "utf-8"))

   def receive(self):
      received = str(self.socket.recv(1024), "utf-8")
      return received