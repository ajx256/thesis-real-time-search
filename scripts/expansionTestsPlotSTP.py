import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"4x4"}
limits = [3, 10, 30, 100, 300, 1000]

algorithmsExpA = ["A*", "F-Hat"]
algorithmsDiffExpA = ["A*", "F-Hat"]

instanceExpA = []
lookAheadValsExpA = []
algorithmExpA = []
solutionCostExpA = []

instanceDiffExpA = []
lookAheadValsDiffExpA = []
algorithmDiffExpA = []
differenceCostExpA = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir):
        with open("../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithmsExpA:
                instanceExpA.append(str(dir))
                lookAheadValsExpA.append(resultData["Lookahead"])
                algorithmExpA.append(algo)
                solutionCostExpA.append(resultData[algo])
                differenceCostExpA.append(resultData[algo] - resultData["A*"])
                instanceDiffExpA.append(str(dir))
                lookAheadValsDiffExpA.append(resultData["Lookahead"])
                algorithmDiffExpA.append(algo)

dfExpA = pd.DataFrame({
    "instance":instanceExpA,
    "Node Expansion Limit":lookAheadValsExpA,
    "Solution Cost":solutionCostExpA,
    "Algorithm":algorithmExpA
})

dfDiffExpA = pd.DataFrame({
    "instance":instanceDiffExpA,
    "Node Expansion Limit":lookAheadValsDiffExpA,
    "Algorithm Cost - A* Cost":differenceCostExpA,
    "Algorithm":algorithmDiffExpA
})

algorithmsExpB = ["A*", "F-Hat", "BFS"]
algorithmsDiffExpB = ["A*", "F-Hat", "BFS"]

instanceExpB = []
lookAheadValsExpB = []
algorithmExpB = []
solutionCostExpB = []

instanceDiffExpB = []
lookAheadValsDiffExpB = []
algorithmDiffExpB = []
differenceCostExpB = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir):
        with open("../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithmsExpB:
                instanceExpB.append(str(dir))
                lookAheadValsExpB.append(resultData["Lookahead"])
                algorithmExpB.append(algo)
                solutionCostExpB.append(resultData[algo])
                differenceCostExpB.append(resultData[algo] - resultData["A*"])
                instanceDiffExpB.append(str(dir))
                lookAheadValsDiffExpB.append(resultData["Lookahead"])
                algorithmDiffExpB.append(algo)

dfExpB = pd.DataFrame({
    "instance":instanceExpB,
    "Node Expansion Limit":lookAheadValsExpB,
    "Solution Cost":solutionCostExpB,
    "Algorithm":algorithmExpB
})

dfDiffExpB = pd.DataFrame({
    "instance":instanceDiffExpB,
    "Node Expansion Limit":lookAheadValsDiffExpB,
    "Algorithm Cost - A* Cost":differenceCostExpB,
    "Algorithm":algorithmDiffExpB
})

algorithmsExpC = ["A*", "F-Hat", "BFS", "Risk"]
algorithmsDiffExpC = ["A*", "F-Hat", "BFS", "Risk"]

instanceExpC = []
lookAheadValsExpC = []
algorithmExpC = []
solutionCostExpC = []

instanceDiffExpC = []
lookAheadValsDiffExpC = []
algorithmDiffExpC = []
differenceCostExpC = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir):
        with open("../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithmsExpC:
                instanceExpC.append(str(dir))
                lookAheadValsExpC.append(resultData["Lookahead"])
                algorithmExpC.append(algo)
                solutionCostExpC.append(resultData[algo])
                differenceCostExpC.append(resultData[algo] - resultData["A*"])
                instanceDiffExpC.append(str(dir))
                lookAheadValsDiffExpC.append(resultData["Lookahead"])
                algorithmDiffExpC.append(algo)

dfExpC = pd.DataFrame({
    "instance":instanceExpC,
    "Node Expansion Limit":lookAheadValsExpC,
    "Solution Cost":solutionCostExpC,
    "Algorithm":algorithmExpC
})

dfDiffExpC = pd.DataFrame({
    "instance":instanceDiffExpC,
    "Node Expansion Limit":lookAheadValsDiffExpC,
    "Algorithm Cost - A* Cost":differenceCostExpC,
    "Algorithm":algorithmDiffExpC
})

print("building plots...")

for instance in resultDirs:
    instanceDataExpA = dfExpA.loc[dfExpA["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})

    ax = sns.pointplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpA, data=instanceDataExpA, join=False, dodge=0.4, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    ax.tick_params(colors='black', labelsize=12)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpA, data=instanceDataExpA, palette="Set2")    

    plt.ylabel("Solution Cost", color='black', fontsize=18)
    plt.xlabel("Node Expansion Limit", color='black', fontsize=18)
    plt.tight_layout()
    plt.savefig("../plots/Experiment2AViolin" + instance + ".pdf")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffExpA = dfDiffExpA.loc[dfDiffExpA["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x="Node Expansion Limit", y="Algorithm Cost - A* Cost", hue="Algorithm", order=limits, hue_order=algorithmsDiffExpA, data=instanceDataDiffExpA, ci=95, join=False, dodge=0.35, palette="Set2")
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel("Algorithm Cost - A* Cost", color='black', fontsize=18)
    plt.xlabel("Node Expansion Limit", color='black', fontsize=18)
    plt.tight_layout()
    plt.savefig("../plots/Experiment2ADifference" + instance + ".pdf")
    
    plt.close()
    plt.clf()
    plt.cla()

    instanceDataExpB = dfExpB.loc[dfExpB["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})

    ax = sns.pointplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpB, data=instanceDataExpB, join=False, dodge=0.53, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    ax.tick_params(colors='black', labelsize=12)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpB, data=instanceDataExpB, palette="Set2")    

    plt.ylabel("Solution Cost", color='black', fontsize=18)
    plt.xlabel("Node Expansion Limit", color='black', fontsize=18)
    plt.tight_layout()
    plt.savefig("../plots/Experiment2BViolin" + instance + ".pdf")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffExpB = dfDiffExpB.loc[dfDiffExpB["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x="Node Expansion Limit", y="Algorithm Cost - A* Cost", hue="Algorithm", order=limits, hue_order=algorithmsDiffExpB, data=instanceDataDiffExpB, ci=95, join=False, dodge=0.35, palette="Set2")
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel("Algorithm Cost - A* Cost", color='black', fontsize=18)
    plt.xlabel("Node Expansion Limit", color='black', fontsize=18)
    plt.tight_layout()
    plt.savefig("../plots/Experiment2BDifference" + instance + ".pdf")
    
    plt.close()
    plt.clf()
    plt.cla()

    instanceDataExpC = dfExpC.loc[dfExpC["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})

    ax = sns.pointplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpC, data=instanceDataExpC, join=False, dodge=0.53, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    ax.tick_params(colors='black', labelsize=12)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpC, data=instanceDataExpC, palette="Set2")    

    plt.ylabel("Solution Cost", color='black', fontsize=18)
    plt.xlabel("Node Expansion Limit", color='black', fontsize=18)
    plt.tight_layout()
    plt.savefig("../plots/Experiment2CViolin" + instance + ".pdf")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffExpC = dfDiffExpC.loc[dfDiffExpC["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x="Node Expansion Limit", y="Algorithm Cost - A* Cost", hue="Algorithm", order=limits, hue_order=algorithmsDiffExpC, data=instanceDataDiffExpC, ci=95, join=False, dodge=0.35, palette="Set2")
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel("Algorithm Cost - A* Cost", color='black', fontsize=18)
    plt.xlabel("Node Expansion Limit", color='black', fontsize=18)
    plt.tight_layout()
    plt.savefig("../plots/Experiment2CDifference" + instance + ".pdf")
    
    plt.close()
    plt.clf()
    plt.cla()

