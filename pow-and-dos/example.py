#!/usr/bin/env python3

import socket
import random
import hashlib
import time
from tqdm import tqdm

HOST = f'linux{random.randint(7, 9)}.csie.ntu.edu.tw'
PORT = 13087


def interactive(s):
    while True:
        try:
            buf = input()
        except KeyboardInterrupt:
            break
        try:
            sendLine(s, buf)
            print(recvAll(s).decode(), end='')
        except BrokenPipeError:
            break


def recvUntil(s, suffix):
    suffix = suffix.encode() if isinstance(suffix, str) else suffix
    ret = b''
    while True:
        c = s.recv(1)
        ret += c
        if ret.endswith(suffix):
            break
    return ret


def recvLine(s):
    return recvUntil(s, '\n')


def recvAll(s):
    return s.recv(100000)


def sendLine(s, buf):
    buf = buf.encode() if isinstance(buf, str) else buf
    return s.sendall(buf + b'\n')


def POW(s):
    line = recvUntil(s, ' : ').decode()
    for i in tqdm(range(2**24)):
        if hashlib.md5(f'{i}'.encode()).hexdigest()[0:8] == line[38:46]:
            sendLine(s, str(i))
            break


if __name__ == '__main__':
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        POW(s)
        print(recvUntil(s, ': ').decode(), end='')
        interactive(s)

