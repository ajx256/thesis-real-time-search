import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d10"}

colors=["#e6194b", "#3cb44b", "#ffe119", "#0082c8", "#f58231", "#911eb4",
        "#46f0f0", "#f032e6", "#808000", "#fabebe", "#008080", "#e6beff", 
        "#aa6e28", "#800000"]

algorithms = ["Minimin", "Bellman", "Nancy", "K-Best 1 Pemberton Belief", "K-Best 3 Pemberton Belief", "K-Best 5 Pemberton Belief", 
              "K-Best 7 Pemberton Belief", "Cserna Pemberton Belief", "K-Best 1", "K-Best 3", "K-Best 5", 
              "K-Best 7", "Cserna",]
algorithmsDiff = ["Minimin", "Bellman", "Nancy", "K-Best 1 Pemberton Belief", "K-Best 3 Pemberton Belief", "K-Best 5 Pemberton Belief", 
              "K-Best 7 Pemberton Belief", "Cserna Pemberton Belief", "K-Best 1", "K-Best 3", "K-Best 5", 
              "K-Best 7", "Cserna"]

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
    for file in listdir("../results/TreeWorld/lastIncrementalDecision/" + dir):
        with open("../results/TreeWorld/lastIncrementalDecision/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instance.append(str(dir))
                lookAheadVals.append(resultData["Lookahead"])
                algorithm.append(algo)
                solutionCost.append(resultData[algo])
                differenceCost.append(resultData[algo] - resultData["Cserna"])
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
    "Algorithm Cost - Cserna Cost":differenceCost,
    "Algorithm":algorithmDiff
})

print("building plots...")

for instance in resultDirs:
    instanceData = df.loc[df["instance"] == instance]

    depths = []

    if instanceData["Depth Limit"].iloc[0] == 14:
        depths.append(14)
    elif instanceData["Depth Limit"].iloc[0] == 9:
        depths.append(9)
        
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})

    ax = sns.pointplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depths, hue_order=algorithms, data=instanceData, join=False, dodge=0.741, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    ax.tick_params(colors='black', labelsize=12)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depths, hue_order=algorithms, data=instanceData, palette=sns.color_palette(colors))    

    plt.ylabel("Solution Cost", color='black', fontsize=18)
    plt.xlabel("Depth Limit", color='black', fontsize=18)
    plt.savefig("../plots/BackupComparisonExpA" + instance + ".pdf")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiff = dfDiff.loc[dfDiff["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x="Depth Limit", y="Algorithm Cost - Cserna Cost", hue="Algorithm", order=depths, hue_order=algorithmsDiff, data=instanceDataDiff, ci=95, join=False, dodge=0.3, palette=sns.color_palette(colors))
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel("Algorithm Cost - Cserna Cost", color='black', fontsize=18)
    plt.xlabel("Depth Limit", color='black', fontsize=18)
    plt.savefig("../plots/BackupDifferenceExpA" + instance + ".pdf")
    
    plt.close()
    plt.clf()
    plt.cla()
