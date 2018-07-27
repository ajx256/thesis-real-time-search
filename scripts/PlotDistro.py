import pandas as pd
import numpy as np
from scipy import stats, integrate
import matplotlib.pyplot as plt

import seaborn as sns

sns.set(color_codes=True)

node1 = pd.read_csv("../examples/PreSquish.csv")
node2 = pd.read_csv("../examples/PostSquish.csv")
#cserna = pd.read_csv("../examples/ConvUniformCserna.csv")

#plt.plot(cserna["Path Cost Cserna"], cserna["CDF Cserna"])
plt.plot(node1["Before Squish Cost"], node1["Before Squish Probability"])
plt.plot(node2["Squish Cost"], node2["Squish Probability"])
plt.title("Probability that Complete Path Cost is Less Than Given X")
plt.legend()
plt.ylabel("Probability")
plt.xlabel("Solution Cost")
plt.show()
