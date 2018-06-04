import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d10", "b2d100", "b2d1000"}

instance = []
lookAheadVals = []
algorithm = []
solutionCost = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../results/treeWorld/" + dir):
        with open("../results/treeWorld/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            if (resultData["Lookahead"] == 1000):
                continue
            if (resultData["Lookahead"] == 10000):
                continue
            instance.append(str(dir))
            lookAheadVals.append(resultData["Lookahead"])
            algorithm.append(resultData["Algorithm"])
            solutionCost.append(resultData["SolutionCost"])

df = pd.DataFrame({
    "instance":instance,
    "Expansion Limit":lookAheadVals,
    "Solution Cost":solutionCost,
    "Algorithm":algorithm
})

print("building plots...")

for instance in resultDirs:
    instanceData = df.loc[df["instance"] == instance]
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})
    ax = sns.boxplot(x="Expansion Limit", y="Solution Cost", hue="Algorithm", data=instanceData, showmeans=True)
    plt.title("Tree Instance: " + instance)

    plt.savefig("../plots/" + instance + ".png")
    plt.close()
