import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d100"}

algorithms = ["Minimin", "Bellman", "Nancy", "K-Best 3", "K-Best 10", "K-Best 30", "CsernaPembertonBelief"]
depths = [3, 7, 10]

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
                solutionCost.append(resultData[algo] - resultData["Cserna"])

df = pd.DataFrame({
    "instance":instance,
    "Depth Limit":lookAheadVals,
    "Algorithm Cost - Cserna Cost":solutionCost,
    "Algorithm":algorithm
})

print("building plots...")

for instance in resultDirs:
    instanceData = df.loc[df["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})
    sns.pointplot(x="Depth Limit", y="Algorithm Cost - Cserna Cost", hue="Algorithm", order=depths, hue_order=algorithms, data=instanceData, ci=95, join=False, dodge=0.3, palette="Set3")
    plt.title("Tree Instance: " + instance)

    plt.savefig("../plots/BackupStrategyDifference" + instance + ".png")
    plt.close()

