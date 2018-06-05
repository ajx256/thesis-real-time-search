import pandas as pd
import numpy as np
from scipy import stats, integrate
import matplotlib.pyplot as plt

import seaborn as sns

sns.set(color_codes=True)

node1 = pd.read_csv("../examples/PembertonNode1.csv")
node2 = pd.read_csv("../examples/PembertonNode2.csv")
cserna = pd.read_csv("../examples/PembertonCserna.csv")

plt.plot(cserna["Path Cost Cserna"], cserna["Probability Cserna"])
plt.plot(node1["Path Cost Node 1"], node1["Probability Node 1"])
plt.plot(node2["Path Cost Node 2"], node2["Probability Node 2"])
plt.title("Probability that Complete Path Cost is Less Than Given X")
plt.legend()
plt.ylabel("Probability")
plt.xlabel("Solution Cost")
plt.show()
