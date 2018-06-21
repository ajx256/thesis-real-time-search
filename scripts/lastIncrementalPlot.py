import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d10"}

algorithms = ["K 1", "K 10", "K 100", "K 1000", "All Frontier Nodes"]
algorithmsDiff = ["K 1", "K 10", "K 100", "K 1000"]

depths = [9]

instance = []
lookAheadVals = []
algorithm = []
solutionCost = []

instanceDiff = []
lookAheadValsDiff = []
algorithmDiff = []
differenceCost = []

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
                if algo != "All Frontier Nodes":
                    differenceCost.append(resultData[algo] - resultData["All Frontier Nodes"])
                    instanceDiff.append(str(dir))
                    lookAheadValsDiff.append(resultData["Lookahead"])
                    algorithmDiff.append(algo)

df = pd.DataFrame({
    "instance":instance,
    "Depth Limit":lookAheadVals,
    "Solution Cost":solutionCost,
    "Algorithm":algorithm
})

dfDiff = pd.DataFrame({
    "instance":instanceDiff,
    "Depth Limit":lookAheadValsDiff,
    "Algorithm Cost - K=All Cost":differenceCost,
    "Algorithm":algorithmDiff
})

print("building plots...")

for instance in resultDirs:
    instanceData = df.loc[df["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})

    ax = sns.pointplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depths, hue_order=algorithms, data=instanceData, join=False, dodge=0.639, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depths, hue_order=algorithms, data=instanceData, palette="Set2")    

    plt.title("Tree Instance: " + instance)
    plt.ylabel("Solution Cost")
    plt.savefig("../plots/KBestCompareKPlot" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiff = dfDiff.loc[dfDiff["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})
    sns.pointplot(x="Depth Limit", y="Algorithm Cost - K=All Cost", hue="Algorithm", order=depths, hue_order=algorithmsDiff, data=instanceDataDiff, ci=95, join=False, dodge=0.3, palette="Set2")
    plt.title("Tree Instance: " + instance)

    plt.savefig("../plots/KBestCompareKPlotDifference" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
