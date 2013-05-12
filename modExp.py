import random
import time

random.seed(0)
nbits = 1024
timer = 0   #

def genBigRam(bits):
    ret = 0
    mul = 1
    for i in range(bits):
        ran = random.randint(0, 1)
        #ran = 1
        ret = ret + ran * mul
        mul = mul * 2
    
    return ret


def expF(m, e, n): # binary method
    ret = 0
    e_str = str(bin(e))
    e_str = e_str[2:]
    len_e_str = len(e_str)
    
    if(e_str[0] == '1'):
        ret = m
    else:
        ret = 1
    
    for i in range(1, len_e_str):
        timer2 = time.clock()
        ret = ret * ret % n

        if(e_str[i] == '1'):
            ret = ret * m % n
            if(ret >= n):
                ret = ret % n
        
    return ret


def findRLen(n):
    r = 1
    i = 0
    while(r <= n):
        r = r * 2
        i = i + 1
    return i
    
# extende algorithm
def extGCD(a, b):
    #timer = time.clock()
    q_arr = []
    r_arr = []
    gcd = 1
    # reverse a and b if necessary
    fReversed = False
    if(a < b):
            fReversed = True
            temp = a
            a = b
            b = temp
    oa = a
    ob = b

    # get GCD
    while(a != 1):
            q = a / b
            q_arr.append(q)
            r = a % b
            r_arr.append(r)
            #print "& " + str(a) + " = " + str(b) + " * " + str(q) + " + " + str(r) + " & \\\\"

            if(r == 0):
                    gcd = b
                    break
            else:
                    a = b
                    b = r
    # get x and y
    x_arr = []
    y_arr = []
    x_arr.append(0)
    y_arr.append(1)
    x_arr.append(1)
    y_arr.append(-q_arr[0])
    x = 1
    y = -q_arr[0]
    #print "& " + str(r_arr[0]) + " = " +  str(oa) + " * " + str(x) + " + " + str(ob) + " * " + str(y) + " & \\\\"
    for i in range(1, len(q_arr)-1):
            x = (x_arr[i] * -q_arr[i] + x_arr[i - 1])
            y = (y_arr[i] * -q_arr[i] + y_arr[i - 1])
            x_arr.append(x)
            y_arr.append(y)
            #print "& " + str(r_arr[i]) + " = " +  str(oa) + " * " + str(x) + " + " + str(ob) + " * " + str(y) + " & \\\\"

    if(fReversed):
            temp = x
            x = y
            y = temp
    
    #print("extGCD time:")
    #print(time.clock() - timer)
    
    return [x, y, gcd]

def expMont(m, e, n): # montgomery method
    timer = 0
    ret = 0;
    r_len = findRLen(n)
    r = 1
    for i in range(r_len):
        r = r * 2
    
    [n_prime, r_prime, gcd] = extGCD(n, r)
    #print(extGCD(n, r))
    #print(str(n_prime * n + r_prime * r))
    
    n_prime = -n_prime  #required in expMontgomery
#    print("n_prime")
#    print(n_prime)
    
    m_bar = (m << r_len) % n
    x_bar = r % n
#    print("m_bar:")
#    print(m_bar)
#    print("x_bar:")
#    print(x_bar)
    
    def monPro2(a_bar, b_bar):
        a_bar_str = str(bin(a_bar))
        a_bar_str = a_bar_str[2:]
        len_a_bar_str = len(a_bar_str)   

        res = 0     
        for i in reversed(range(len_a_bar_str)):
            if(a_bar_str[i] == "1"):
                res = res + b_bar
            if(res % 2 == 1):
                res = res + n
            res = res >> 1   
            
        for i in range(r_len - len_a_bar_str):
            if(res % 2 == 1):
                res = res + n
            res = res >> 1
        
        while(res >= n):
            res = res - n
            
        return res
    
    def monPro(a_bar, b_bar):
#        print("In monPro:")
        t = a_bar * b_bar
        m1 = t * n_prime
        m2 = ((m1 >> r_len) << r_len) ^ m1 # mod r
        m3 = m2 * n
        m4 = m3 + t
        #m = m % r
        u = m4 >> r_len

        while(u >= n):
            u = u - n    
                   
        return u
    
    e_str = str(bin(e))
    e_str = e_str[2:]
    len_e_str = len(e_str)
    for i in range(len_e_str):
        timer2 = time.clock()
        x_bar = monPro2(x_bar, x_bar)
            
        if(e_str[i] == "1"):
            x_bar = monPro2(m_bar, x_bar)  
                         
        timer2 = time.clock() - timer2
        timer = timer + timer2
    ret = monPro2(x_bar, 1)
    return ret

m = 0x3a879a4f3badb68666602a2f56d655844e370f7816baf14b9f2e7fad1766d21db2d5462b8ac7eb47bd438a17f47d3b9ab548ca5730b0c8847d4baadaad73ebe1e42ec70a6cd50f30a1afdd7ab8fd937e77901a0d9e321c0e645a1f667a64eee4428cf81fcd7e39be98e97f43c1d2025509b443afe04f61c7fa0beef572456a1d
#genBigRam(nbits)
e = 0x2896db94a6862033f83f365ed39c641e80f3f5cacc368598b29d2ef7b1e7059ac3cef68c88ddb42e8ceb2b572edb2dbe7084642ef70b3275247d2e352c71c573dc1d7fee53cb64395489445d1c88320b9580da6313c8a48b581cd41d588766cd8bc84f3a2d3aa16976caae8234a878e9fa1293e690a86adc1b76b3071e0eb2ab
#genBigRam(nbits)
n = 0xa97ee8c01a5032dbcca71a30d6b91cba673f919864fcb1158821b29a2d16e04651525c0ddca48b4c43fb879224026b3ed846030c10b750322800fc1bd091ffd40160e0962ce6384a857873e1ade3ff9a8c2542313c50571b4bc528dd419c6e8020c3f2a8f36cca5cc591a9833ea0d61948ff36e914f86b929b423168e4942bb3#genBigRam(nbits)
if(n % 2 == 0):
    n = n + 1

print("m:\t")
print(hex(m))
print("e:\t")
print(hex(e))
print("n:\t")
print(hex(n))
print("m^e mod n:\t")
repeats = 1
total_time = time.clock()
for i in range(repeats):
    m_2_e_mod_n = expMont(m, e, n)
print("Montgomery:")
print(hex(m_2_e_mod_n))

total_time = time.clock() - total_time
print(total_time)
    
#total_time = time.clock()
#for i in range(repeats):
#    m_2_e_mod_n = expF(m, e, n)
#print("Normal Exp:")
#print(str(m_2_e_mod_n))
#total_time = time.clock() - total_time
#print(total_time)
