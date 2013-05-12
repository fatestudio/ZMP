def factorial(n, stop=0):
    o = 1
    while n > stop:
        o *= n
        n -= 1
    return o

def choose(n, k):
    return factorial(n, stop=k) / factorial(n - k)

if __name__ == '__main__':
    print choose(50000, 50)
