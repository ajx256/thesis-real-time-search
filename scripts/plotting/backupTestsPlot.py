import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d100"}
#"K-Best 3 One Level Belief", "K-Best 10 One Level Belief", "K-Best 30 One Level Belief", 
algorithms = ["Minimin", "Bellman", "Nancy", "K-Best 3", "K-Best 10", "K-Best 30", "Cserna", "Cserna One Level Belief"]
algorithmsDiff = ["Minimin", "Bellman", "Nancy", "K-Best 3", "K-Best 10", "K-Best 30", "Cserna", "Cserna One Level Belief"]

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
    for file in listdir("../../results/TreeWorld/backupTests/DFS/" + dir):
        with open("../../results/TreeWorld/backupTests/DFS/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instanceDFS.append(str(dir))
                lookAheadValsDFS.append(resultData["Lookahead"])
                algorithmDFS.append(algo)
                solutionCostDFS.append(resultData[algo.replace("One Level", "Pemberton")])
                differenceCostDFS.append(resultData[algo.replace("One Level", "Pemberton")] - resultData["Cserna Pemberton Belief"])
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
    for file in listdir("../../results/TreeWorld/backupTests/AS/" + dir):
        with open("../../results/TreeWorld/backupTests/AS/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instanceAS.append(str(dir))
                lookAheadValsAS.append(resultData["Lookahead"])
                algorithmAS.append(algo)
                solutionCostAS.append(resultData[algo.replace("One Level", "Pemberton")])
                differenceCostAS.append(resultData[algo.replace("One Level", "Pemberton")] - resultData["Cserna Pemberton Belief"])
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
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})

    ax = sns.pointplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depthsDFS, hue_order=algorithms, data=instanceDataDFS, join=False, dodge=0.70, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    ax.tick_params(colors='black', labelsize=12)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Depth Limit", y="Solution Cost", hue="Algorithm", order=depthsDFS, hue_order=algorithms, data=instanceDataDFS, palette="Set2")    

    plt.ylabel("Solution Cost", color='black', fontsize=18)
    plt.xlabel("Depth Limit", color='black', fontsize=18)
    plt.savefig("../../plots/Experiment1BViolin" + instance + ".pdf", bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffDFS = dfDiffDFS.loc[dfDiffDFS["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x="Depth Limit", y="Algorithm Cost - Cserna Cost", hue="Algorithm", order=depthsDFS, hue_order=algorithmsDiff, data=instanceDataDiffDFS, ci=95, join=False, dodge=0.35, palette="Set2")
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel("Algorithm Cost - Cserna One Level Belief Cost", color='black', fontsize=18)
    plt.xlabel("Depth Limit", color='black', fontsize=18)
    plt.savefig("../../plots/Experiment1BDifference" + instance + ".pdf", bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()

    instanceDataAS = dfAS.loc[dfAS["instance"] == instance]
    
    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})

    ax = sns.pointplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=depthsAS, hue_order=algorithms, data=instanceDataAS, join=False, dodge=0.7, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    ax.tick_params(colors='black', labelsize=12)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x="Node Expansion Limit", y="Solution Cost", hue="Algorithm", order=depthsAS, hue_order=algorithms, data=instanceDataAS, palette="Set2")    
    plt.ylabel("Solution Cost", color='black', fontsize=18)
    plt.xlabel("Node Expansion Limit", color='black', fontsize=18)
    plt.savefig("../../plots/Experiment1CViolin" + instance + ".pdf", bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()
    
    instanceDataDiffAS = dfDiffAS.loc[dfDiffAS["instance"] == instance]

    sns.set_style("white")
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x="Node Expansion Limit", y="Algorithm Cost - Cserna Cost", hue="Algorithm", order=depthsAS, hue_order=algorithmsDiff, data=instanceDataDiffAS, ci=95, join=False, dodge=0.35, palette="Set2")
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel("Algorithm Cost - Cserna One Level Belief Cost", color='black', fontsize=18)
    plt.xlabel("Node Expansion Limit", color='black', fontsize=18)
    plt.savefig("../../plots/Experiment1CDifference" + instance + ".pdf", bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()