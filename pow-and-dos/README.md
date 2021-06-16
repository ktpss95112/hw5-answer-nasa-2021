# Problem 2: Proof of Work & DoS

## Environment setup

The server:
* use `make` to compile `server.cpp` and run `./server 0.0.0.0 13087` on linux7, linux8, linux9
* `server.cpp` receives TCP connections from client. One client can have only one connection simultaneously.
* The server launches `main.py` for every connection and forwards the connection to `main.py`, which means every client interacts directly with `main.py`.
* The `test_slow` function in `main.py` adjusts the timeout according to the current workload of the host machine. That is, if the workload of the workload of the server is heavy, the timeout is longer.
* flags are stored in `secret.py`

The released files of the homework are `example.py` and `main.py`.

## Writeup

https://hackmd.io/@uqzWTXyyTk6IYTBwcPwnoA/SJL-sCIsd#2-Proove-of-Work-amp-DoS
