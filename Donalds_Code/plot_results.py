import numpy as np
import matplotlib.pyplot as plt
n="/home/donald/Desktop/HPC/simplestMPI/CPP-version/results1/quadMPI1_out1_tasks<blank>"
def nums(s):
    a=min(i for i,j in enumerate(s) if j.isdigit())
    b=max(i for i,j in enumerate(s) if j.isdigit())
    if "," not in s[a:b+1]:
        return s[a:b+1]
    return s[a:b+1].split(",")[0]
kk=[]

def extend(kk,i):
    with open(n.replace("<blank>",str(i))) as f:
        s=f.readlines()
    l=len(s)
    k=[s[i:i+11] for i in range(0,l,11)]
    kk.extend(k)
    return kk
for i in [1,4,12,"36_l"]:
    extend(kk,i)
    
def rearrange(kk):

    kk=list(zip(*kk))

    kk=list(map(lambda x:np.array(list(map(lambda y:float(nums(y)),x))),kk))
    return kk
kk=rearrange(kk)
def mpl(d,x,y,c,s):
    for i,j in d.items():
        w=list((p,q,r ) for p,q,r in zip(x,y,c) if r==i)
        if len(w)==0:
            continue
        plt.plot(list(map(lambda x:x[0], w)),list(map(lambda x:x[1], w)),s+j,label=str(i))
    
    plt.legend()
    plt.show()
#{1:"-k",4:"-r",12:"-g",36:"-b"}
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Nodes used")
plt.ylabel("Time per quad (seconds)")
mpl({10000:"k",33333:"r",100000:"g",333333:"b"},kk[1],kk[10],kk[3],"-x")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Nodes used")
plt.ylabel("Time per quad (seconds)")
plt.plot(kk[1],kk[10],"kx")
plt.show()
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Resolution used")
plt.ylabel("Time (seconds)")
mpl({1:"-k",4:"-r",12:"-g",36:"-b"},kk[3],kk[9],kk[1],"x")
plt.show()
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Resolution used")
plt.ylabel("Error")

plt.plot(kk[3][-3:],kk[8][-3:],"-x")
plt.show()
plt.plot(kk[1],kk[10]*kk[1],"x")
plt.show()

k2=rearrange(extend([],"36_l"))
plt.plot(k2[3],k2[8],"-xk")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Resolution used")
plt.ylabel("Error")
plt.show()
