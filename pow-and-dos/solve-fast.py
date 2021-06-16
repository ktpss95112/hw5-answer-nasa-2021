import socket
import hashlib
import time
from tqdm import tqdm
import multiprocessing as mp

HOST = 'linux7.csie.ntu.edu.tw'
PORT = 13087


def task1(s):
    POW(s)
    s.sendall(b'1\n')

    # qsort
    a = list(range(7000))
    b = [ 0 for _ in range(7000) ]
    for i in range(7000):
        n = len(a) // 2
        pivot_index = a[n]
        b[pivot_index] = 7000 - 1 - i
        a = a[:n] + a[n+1:]
    qsort_ans = ' '.join(map(str, b))

    s.recv(1000)
    s.sendall((qsort_ans + '\n').encode())
    print(s.recv(1000).decode())


def task2(s):
    POW(s)
    s.sendall(b'2\n')

    regex_ans1 = 'Dear Sophia, ' + '柴魚'*25 + '. Best wishes, aaa!'
    # regex_ans2 = 'Dear Sophia, ' + '柴魚'*10 + '. Best wishes, ' + 'a'*30 + '!'
    s.recv(1000)
    s.sendall((regex_ans1 + '\n').encode())
    print(s.recv(1000).decode())


table = {}
def task3(s):
    gen_table()
    POW(s)
    s.sendall(b'3\n')

    for _ in range(10):
        line = s.recv(1000).decode()
        s.sendall((f'{table[line[38:46]]}\n').encode())

    print(s.recv(1000).decode())


def worker(i):
    return hashlib.md5(f'{i}'.encode()).hexdigest()[0:8]


def gen_table():
    global table

    with mp.Pool() as pool:
        l = pool.map(worker, range(2**24), chunksize=2**15)

        for i, h in tqdm(enumerate(l), desc='generate table', total=2**24):
            table[h] = i


def POW(s):
    line = s.recv(1000).decode()
    for i in tqdm(range(2**24)):
        if hashlib.md5(f'{i}'.encode()).hexdigest()[0:8] == line[38:46]:
            s.sendall(f'{i}\n'.encode())
            break
    s.recv(1000)


if __name__ == '__main__':
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        task1(s)
        time.sleep(1.5)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        task2(s)
        time.sleep(1.5)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        task3(s)
        time.sleep(1.5)
