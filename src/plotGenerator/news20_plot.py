import matplotlib.pyplot as plt
import numpy as np

samples1 = np.fromfile("../output/news20_ms.txt",sep="\n")
samples3 = np.fromfile("../output/news20_mt.txt",sep="\n")
samples2 = np.fromfile("../output/news20_poly.txt",sep="\n")
samples4 = np.fromfile("../output/news20_mur.txt",sep="\n")
samples5 = np.fromfile("../output/news20_p20.txt",sep="\n")

def mse(l,m):
    return np.round(np.mean(map(lambda x : (x-m)**2, l)),4)

f, (ax1, ax2, ax3, ax4, ax5) = plt.subplots(1,5,sharex=True,sharey=True)
ax1.hist(samples1, 55, range=[0,2], alpha=0.5)
ax1.set_title("Multiply-shift",fontsize=12)
ax1.plot([1.0,1.0],[0,350000], lw=2, color='r')
ax1.annotate('MSE='+str(mse(samples1,1.0)), xy=(1,320000), fontsize=7)
ax2.hist(samples2, 55, range=[0,2], alpha=0.5)
ax3.set_title("Mixed Tabulation",fontsize=12)
ax2.plot([1.0,1.0],[0,350000], lw=2, color='r')
ax2.annotate('MSE='+str(mse(samples2,1.0)), xy=(1,320000), fontsize=7)
ax3.hist(samples3, 55, range=[0,2], alpha=0.5)
ax2.set_title("PolyHash 2-wise",fontsize=12)
ax3.plot([1.0,1.0],[0,350000], lw=2, color='r')
ax3.annotate('MSE='+str(mse(samples3,1.0)), xy=(1,320000), fontsize=7)
ax4.hist(samples4, 55, range=[0,2], alpha=0.5)
ax4.set_title("MurmurHash3",fontsize=12)
ax4.plot([1.0,1.0],[0,350000], lw=2, color='r')
ax4.annotate('MSE='+str(mse(samples4,1.0)), xy=(1,320000), fontsize=7)
ax5.hist(samples5, 55, range=[0,2], alpha=0.5)
ax5.set_title("\"random\"",fontsize=12)
ax5.plot([1.0,1.0],[0,350000], lw=2, color='r')
ax5.annotate('MSE='+str(mse(samples5,1.0)), xy=(1,320000), fontsize=7)
ax1.tick_params(labelsize=7)
ax2.tick_params(labelsize=7)
ax3.tick_params(labelsize=7)
ax4.tick_params(labelsize=7)
ax5.tick_params(labelsize=7)
#f.suptitle("Feature Hashing on News20 with d' = 128", fontsize=18)
f.subplots_adjust(top=0.8)
f.set_size_inches(10,2)
f.savefig("output/news20hist.pdf", bbox_inches="tight")

def printStats(arr, name):
    print name + " & " + str(np.round(np.mean(arr),4)) + " & " + \
            str(np.round(np.var(arr),4)) + " & " + \
            str(np.round(np.percentile(arr,10),4)) + " & " + \
            str(np.round(np.percentile(arr,90),4)) + " \\\\"

printStats(samples1, "Multiply-Shift")
printStats(samples2, "Mixed tab")
printStats(samples3, "2-wise PolyHash")
printStats(samples4, "20-wise PolyHash")
