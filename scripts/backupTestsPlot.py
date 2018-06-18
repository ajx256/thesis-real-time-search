import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d100"}

algorithms = ["Minimin", "Bellman", "Nancy", "K-Best 3", "K-Best 10", "K-Best 30", "Cserna Pemberton Belief", "Cserna"]
algorithmsDiff = ["Minimin", "Bellman", "Nancy", "K-Best 3", "K-Best 10", "K-Best 30", "Cserna Pemberton Belief"]

depthsDFS = [3, 7, 10]

instanceDFS = []
lookAheadValsDFS = []
algorithmDFS = []
solutionCostDFS = []

instanceDFSDiff = []
lookAheadValsDFSDiff = []
algorithmDFSDiff = []
differenceCostDFS = []

depthsAS = [10, 100, 1000]

instanceAS = []
lookAheadValsAS = []
algorithmAS = []
solutionCostAS = []

instanceASDiff = []
lookAheadValsASDiff = []
algorithmASDiff = []
differenceCostAS = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../results/backupTests/DFS/" + dir):
        with open("../results/backupTests/DFS/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instanceDFS.append(str(dir))
                lookAheadValsDFS.append(resultData["Lookahead"])
                algorithmDFS.append(algo)
                solutionCostDFS.append(resultData[algo])
                if algo != "Cserna":
                    differenceCostDFS.append(resultData[algo] - resultData["Cserna"])
                    instanceDFSDiff.append(str(dir))
                    lookAheadValsDFSDiff.append(resultData["Lookahead"])
                    algorithmDFSDiff.append(algo)

dfDFS = pd.DataFrame({
    "instance":instanceDFS,
    "Depth Limit":lookAheadValsDFS,
    "Solution Cost":solutionCostDFS,
    "Algorithm":algorithmDFS
})

dfDiffDFS = pd.DataFrame({
    "instance":instanceDFSDiff,
    "Depth Limit":lookAheadValsDFSDiff,
    "Algorithm Cost - Cserna Cost":differenceCostDFS,
    "Algorithm":algorithmDFSDiff
})

for dir in resultDirs:
    for file in listdir("../results/backupTests/AS/" + dir):
        with open("../results/backupTests/AS/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instanceAS.append(str(dir))
                lookAheadValsAS.append(resultData["Lookahead"])
                algorithmAS.append(algo)
                solutionCostAS.append(resultData[algo])
                if algo != "Cserna":
                    differenceCostAS.append(resultData[algo] - resultData["Cserna"])
                    instanceASDiff.append(str(dir))
                    lookAheadValsASDiff.append(resultData["Lookahead"])
                    algorithmASDiff.append(algo)
dfAS = pd.DataFrame({
    "instance":instanceAS,
    "Node Expansion Limit":lookAheadValsAS,
    "Solution Cost":solutionCostAS,
    "Algorithm":algorithmAS
})

dfDiffAS = pd.DataFrame({
    "instance":instanceASDiff,
    "Node Expansion Limit":lookAheadValsASDiff,
    "Algorithm Cost - Cserna Cost":differenceCostAS,
    "Algorithm":algorithmASDiff
})

print("building plots...")

for instance in resultDirs:
    instanceDataDFS = dfDFS.loc[dfDFS["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (22, 16)})

    ax = sns.pointplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depthsDFS, hue_order=algorithms, data=instanceDataDFS, join=False, dodge=0.839, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depthsDFS, hue_order=algorithms, data=instanceDataDFS, palette="Set3")    

    plt.title("Tree Instance: " + instance)
    plt.ylabel("Solution Cost")
    plt.savefig("../plots/BackupStrategyComparisonDFS" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffDFS = dfDiffDFS.loc[dfDiffDFS["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (22, 16)})
    sns.pointplot(x="Depth Limit", y="Algorithm Cost - Cserna Cost", hue="Algorithm", order=depthsDFS, hue_order=algorithmsDiff, data=instanceDataDiffDFS, ci=95, join=False, dodge=0.3, palette="Set3")
    plt.title("Tree Instance: " + instance)

    plt.savefig("../plots/BackupStrategyDifferenceDFS" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()

    instanceDataAS = dfAS.loc[dfAS["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (22, 16)})

    ax = sns.pointplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=depthsAS, hue_order=algorithms, data=instanceDataAS, join=False, dodge=0.839, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=depthsAS, hue_order=algorithms, data=instanceDataAS, palette="Set3")    

    plt.title("Tree Instance: " + instance)
    plt.ylabel("Solution Cost")
    plt.savefig("../plots/BackupStrategyComparisonAS" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffAS = dfDiffAS.loc[dfDiffAS["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (22, 16)})
    sns.pointplot(x="Node Expansion Limit", y="Algorithm Cost - Cserna Cost", hue="Algorithm", order=depthsAS, hue_order=algorithmsDiff, data=instanceDataDiffAS, ci=95, join=False, dodge=0.3, palette="Set3")
    plt.title("Tree Instance: " + instance)

    plt.savefig("../plots/BackupStrategyDifferenceAS" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()