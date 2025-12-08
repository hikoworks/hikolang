

for i in range(4):
    n = i
    i = i >> 1
    n = n & i
    n = n + i
    print(n)
