import pandas as pd
import numpy as np
from scipy import stats, integrate
import matplotlib.pyplot as plt

import seaborn as sns

sns.set(color_codes=True)

node1 = pd.read_csv("../../distributions/NormalBeliefNode1.csv")
node2 = pd.read_csv("../../distributions/NormalBeliefNode2.csv")
cserna = pd.read_csv("../../distributions/NormalBeliefCserna.csv")

plt.rcParams['figure.figsize'] = (11, 8) 
plt.rcParams['font.size'] = 26
plt.rcParams['text.color'] = 'black'
plt.tick_params(labelsize=12)


plt.plot(cserna["Path Cost Cserna"], cserna["Probability Cserna"], color="blue")
#plt.bar(cserna["Path Cost Cserna"], cserna["CDF Cserna"], 0.03, color="blue")
plt.plot(node1["Path Cost Node 1"], node1["Probability Node 1"], color="green")
#plt.bar(node1["Path Cost Node 1"], node1["Probability Node 1"], 0.03, color="green")
plt.plot(node2["Path Cost Node 2"], node2["Probability Node 2"], color="red")
#plt.bar(node2["Path Cost Node 2"], node2["Probability Node 2"], 0.03, color="red")
plt.title("Probability that Complete Path Cost is x", color='black', fontsize=18)
plt.legend()
plt.ylabel("Density", color='black', fontsize=18)
plt.xlabel("Path Cost", color='black', fontsize=18)
plt.savefig("../../plots/outputName.pdf", bbox_inches="tight", pad_inches=0)
