import hashlib
from secret import secretkey

def check_evidence(s: str):
    r, data, h = s.split('||')
    return h == hashlib.sha256(f'{r}||{data}||{secretkey}'.encode()).hexdigest()

if __name__ == '__main__':
    if check_evidence(input('give your evidence: ')):
        print('success')
    else:
        print('fail')
