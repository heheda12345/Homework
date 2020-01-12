import matplotlib.pyplot as plt

f = open('time/bf.txt')
time_bf = []
for st in f.readlines():
    s = st.strip().split("\t")
    time_bf.append([float(x) for x in s])

f = open('time/add.txt')
time_add = []
for st in f.readlines():
    s = st.strip().split("\t")
    time_add.append([float(x) for x in s])

f = open('time/fft.txt')
time_fft = []
for st in f.readlines():
    s = st.strip().split("\t")
    time_fft.append([float(x) for x in s])

f = open('time/save.txt')
time_save = []
for st in f.readlines():
    s = st.strip().split("\t")
    time_save.append([float(x) for x in s])
    
n = [[x for x in range(100, 10100, 100)], [x for x in range(1000, 101000, 1000)], [x for x in range(10000, 1010000, 10000)]]
print(n)

x = n[0][:10] + n[1][:10] + n[2][:10]
plt.loglog(x, time_bf[0][:10] + time_bf[1][:10] + time_bf[2][:10], '-o', label = "bruteforce")
plt.loglog(x, time_fft[0][:10] + time_fft[1][:10] + time_fft[2][:10], '-o', label = "fft")
plt.loglog(x, time_add[0][:10] + time_add[1][:10] + time_add[2][:10], '-o', label = "overlap add")
plt.loglog(x, time_save[0][:10] + time_save[1][:10] + time_save[2][:10], '-o', label = "overlap save")
plt.legend()
plt.xlabel("length")
plt.ylabel("time(s)")
plt.show()

for i in range(3):
    plt.plot(n[i], time_fft[i], label = "fft")
    plt.plot(n[i], time_add[i], label = "overlap add")
    plt.plot(n[i], time_save[i], label = "overlap save")
    plt.legend()
plt.xlabel("length")
plt.ylabel("time(s)")
plt.show()