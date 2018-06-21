import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d10"}

algorithms = ["K 1", "K 10", "K 100", "K 1000", "All Frontier Nodes"]

depths = [9]

instance = []
lookAheadVals = []
algorithm = []
solutionCost = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../results/lastIncrementalDecision/" + dir):
        with open("../results/lastIncrementalDecision/" + dir + "/" + file) as json_data:
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
    sns.set(rc={'figure.figsize': (22, 16)})

    ax = sns.pointplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depths, hue_order=algorithms, data=instanceData, join=False, dodge=0.7, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depths, hue_order=algorithms, data=instanceData, palette="Set3")    

    plt.title("Tree Instance: " + instance)
    plt.ylabel("Solution Cost")
    plt.savefig("../plots/KBestCompareKPlot" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
