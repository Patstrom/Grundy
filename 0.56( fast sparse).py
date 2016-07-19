## .56 octal game

import math
import time
import periodcheck as pe

t = time.time()

#list of grundy values for n = 0,...,6
gvlist = [0, 1, 0, 2, 2, 4, 1]

##def mex2(lst):
##    j = 0
##    for i in range(0, len(lst):
##        if lst[i] == j:
##            j+=0
##        elif lst[i] > j:
##            return j
##    return j

def mex(lst):
    i = 0
    while i in lst:
        i += 1
    return i

def gv_next(lst):
    length = len(lst) #n+1 for G_n
    value_list = []
    for i in range(1, math.ceil(length/2)):
        value_list.append(lst[i]^lst[length-i-1])
    for i in range(0, math.ceil(length/2)):
        value_list.append(lst[i]^lst[length-i-2])
    return mex(value_list)

def gv_listing(n, lst):
    for i in range(0,n):
        lst.append(gv_next(lst))
    return lst

gvlong = gv_listing(1795-len(gvlist)+1, gvlist)#1795 first values(last rare value)

def sp_list(k):
    string_list = []
    for n in range(0, k):
        if bin(n & int("0b111111011011", 2)).count("1") % 2 == 0:
            string_list.append(n)
    return string_list


sparse = sp_list(100)
sparse_i2 = [i for i in range(1796) if gvlong[i] in sparse]
sparse_i1 = sparse_i2[1:]

common = [i for i in range(100) if i not in sparse]    

def common_v(lst):
    for i in common:
        if i not in lst:
            return i

def GV_NEXT(lst):
    length = len(lst)
    value_list = []
    for i in sparse_i1:
                value_list.append(lst[i]^lst[length-i-1])
    for i in sparse_i2:
        value_list.append(lst[i]^lst[length-i-2])
    return common_v(value_list)

##def GV_NEXT(lst, length):
##    value_list = [0]*(len(sparse_i)*2)
##    for idx, i in enumerate(sparse_i):
##        value_list[2*idx] = lst[i]^lst[length-i-2]
##        value_list[2*idx+1] = lst[i]^lst[length-i-3]
##    return common_v(value_list)


##def GV_LISTING(n, lst):
##    L = len(lst)
##    lst += [0]*n
##    for i in range(0, n):
##        lst[i+L] = GV_NEXT(lst, L+i)
##    return lst

def GV_LISTING(n, lst):
    for i in range(0, n):
        lst.append(GV_NEXT(lst))
    return lst



GV = GV_LISTING(100000-1796, gvlong)
print("tiden för att beräkna grundyvärdena är")
print(time.time()-t)

##t = time.time()
##pe.precheck(GV)
##print(time.time()-t)
