import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d100"}
limits = [3, 6, 10, 30, 100, 1000]

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
    for file in listdir("../results/TreeWorld/expansionTests/Nancy/" + dir):
        with open("../results/TreeWorld/expansionTests/Nancy/" + dir + "/" + file) as json_data:
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
    for file in listdir("../results/TreeWorld/expansionTests/Nancy/" + dir):
        with open("../results/TreeWorld/expansionTests/Nancy/" + dir + "/" + file) as json_data:
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

algorithmsExpC = ["A*", "F-Hat", "Risk"]
algorithmsDiffExpC = ["A*", "F-Hat", "Risk"]

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
    for file in listdir("../results/TreeWorld/expansionTests/Nancy/" + dir):
        with open("../results/TreeWorld/expansionTests/Nancy/" + dir + "/" + file) as json_data:
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
    sns.set(rc={'figure.figsize': (11, 8)})

    ax = sns.pointplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpA, data=instanceDataExpA, join=False, dodge=0.4, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpA, data=instanceDataExpA, palette="Set2")    

    plt.title("Aligning Expansion Strategy with Decision Strategy with Tree Instance: " + instance)
    plt.ylabel("Solution Cost")
    plt.savefig("../plots/ExpansionExpA" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffExpA = dfDiffExpA.loc[dfDiffExpA["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})
    sns.pointplot(x="Node Expansion Limit", y="Algorithm Cost - A* Cost", hue="Algorithm", order=limits, hue_order=algorithmsDiffExpA, data=instanceDataDiffExpA, ci=95, join=False, dodge=0.35, palette="Set2")
    plt.title("Aligning Expansion Strategy with Decision Strategy with Tree Instance: " + instance)

    plt.savefig("../plots/ExpansionDifferenceExpA" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()

    instanceDataExpB = dfExpB.loc[dfExpB["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})

    ax = sns.pointplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpB, data=instanceDataExpB, join=False, dodge=0.53, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpB, data=instanceDataExpB, palette="Set2")    

    plt.title("Flexible vs Uniform Lookahead with Tree Instance: " + instance)
    plt.ylabel("Solution Cost")
    plt.savefig("../plots/ExpansionExpB" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffExpB = dfDiffExpB.loc[dfDiffExpB["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})
    sns.pointplot(x="Node Expansion Limit", y="Algorithm Cost - A* Cost", hue="Algorithm", order=limits, hue_order=algorithmsDiffExpB, data=instanceDataDiffExpB, ci=95, join=False, dodge=0.35, palette="Set2")
    plt.title("Flexible vs Uniform Lookahead with Tree Instance: " + instance)

    plt.savefig("../plots/ExpansionDifferenceExpB" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()

    instanceDataExpC = dfExpC.loc[dfExpC["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})

    ax = sns.pointplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpC, data=instanceDataExpC, join=False, dodge=0.53, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=limits, hue_order=algorithmsExpC, data=instanceDataExpC, palette="Set2")    

    plt.title("Risk vs F-Hat Lookahead with Tree Instance: " + instance)
    plt.ylabel("Solution Cost")
    plt.savefig("../plots/ExpansionExpC" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffExpC = dfDiffExpC.loc[dfDiffExpC["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8)})
    sns.pointplot(x="Node Expansion Limit", y="Algorithm Cost - A* Cost", hue="Algorithm", order=limits, hue_order=algorithmsDiffExpC, data=instanceDataDiffExpC, ci=95, join=False, dodge=0.35, palette="Set2")
    plt.title("Risk vs F-Hat Lookahead with Tree Instance: " + instance)

    plt.savefig("../plots/ExpansionDifferenceExpC" + instance + ".png")
    
    plt.close()
    plt.clf()
    plt.cla()
