import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d100"}

algorithms = {"Minimin", "Bellman", "Nancy", "Cserna", "CsernaPembertonBelief"}

instance = []
lookAheadVals = []
algorithm = []
solutionCost = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../results/backupTests/" + dir):
        with open("../results/backupTests/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instance.append(str(dir))
                lookAheadVals.append(resultData["Lookahead"])
                algorithm.append(algo)
                solutionCost.append(resultData[algo])

df = pd.DataFrame({
    "instance":instance,
    "Depth Limit":lookAheadVals,
    "Solution Cost":solutionCost,
    "Algorithm":algorithm
})

print("building plots...")

for instance in resultDirs:
    instanceData = df.loc[df["instance"] == instance]
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})
    ax = sns.boxplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", data=instanceData, showmeans=True)
    plt.title("Tree Instance: " + instance)

    plt.savefig("../plots/BackupStrategyComparison" + instance + ".png")
    plt.close()

