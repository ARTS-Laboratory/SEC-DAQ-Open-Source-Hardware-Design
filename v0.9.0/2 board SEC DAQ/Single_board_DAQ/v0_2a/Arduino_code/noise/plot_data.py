# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""


#%% import modules

import IPython as IP
IP.get_ipython().magic('reset -sf')
import matplotlib.pyplot as plt
import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import os as os
import numpy as np
from scipy.interpolate import griddata
from matplotlib import cm
import time
import subprocess
import pickle
import scipy.io as sio
import sympy as sym
from matplotlib import cm

plt.close('all')

#%% build the test setup

D = np.loadtxt('data.txt')
tt = D[:,0]
tt = tt*1e-6
c1 = D[:,1]


# caluclte a strain offset
m = np.mean(c1)

X = []
S = []
for i in [10,20,30,40,50,60]:
    s = i*1e-6 # strain in microstrain
    S.append(s)
    C = 50*1e-12 # in pF
    GF = 1.98
    X.append(GF*C*s*1e12)

X = np.asarray(X)
S = np.asarray(S)    
      
plt.figure()
plt.plot(tt,c1)

for i in range(0,6):
    plt.axhline(m+X[i], color='r', linestyle='-')

plt.axhline(m, color='k', linestyle='-')

plt.ticklabel_format(useOffset=False)
plt.ylabel('capacitance (pF)')
plt.xlabel('time (s)')

plt.text(55,m+0.0001,''.join([str(0*1e6),' $\mu \epsilon$']))
for i in range(0,6):
    plt.text(55,m+X[i]+0.0001,''.join([str(S[i]*1e6),' $\mu \epsilon$']))


plt.savefig('FDC_1004_sensitivity',figsize=(4,4),dpi=300)








