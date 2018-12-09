import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

def makeViolinPlot(width, height, xAxis, yAxis, dataframe, dodge, hue, orderList, hueOrderList, xLabel, yLabel, outputName):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, join=False, dodge=dodge, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    ax.tick_params(colors='black', labelsize=12)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, palette="Set2")    

    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()
    return

def makeDifferencePlot(width, height, xAxis, yAxis, dataframe, dodge, hue, orderList, hueOrderList, xLabel, yLabel, outputName, markerList):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, ci=95, errwidth=3, join=False, dodge=dodge, palette="Set2", markers=markerList)
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

markers=["o", "v", "s", "<", "p", "h", "^", "D", "X", ">", "o", "v", "s", "<", "p", "h", "^", "D", "X", ">"]

korfCosts = {}

with open("../utility/KorfTrueCosts.json") as costs:
	korfCosts = json.load(costs)

# Hard coded result directories
resultDirs = {"4x4"}

algorithms = ["Minimin", "Bellman", "Nancy", "K-Best 3", "K-Best 10", "K-Best 30", "Cserna"]
algorithmsAAAI19Slide = ["Minimin", "Bellman", "Nancy", "Cserna"]

'''
depthsDFS = [3, 7, 10]

instanceDFS = []
lookAheadValsDFS = []
algorithmDFS = []
solutionCostDFS = []

instanceDFSDiff = []
lookAheadValsDFSDiff = []
algorithmDFSDiff = []
differenceCostDFS = []
'''

depthsAS = [10, 100, 1000]

instanceAS = []
lookAheadValsAS = []
algorithmAS = []
solutionCostAS = []
differenceCostAS = []

print("reading in data...")

'''
for dir in resultDirs:
    for file in listdir("../../results/SlidingTilePuzzle/backupTests/DFS/" + dir):
        with open("../../results/SlidingTilePuzzle/backupTests/DFS/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instanceDFS.append(str(dir))
                lookAheadValsDFS.append(resultData["Lookahead"])
                algorithmDFS.append(algo)
                solutionCostDFS.append(resultData[algo])
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
'''

for dir in resultDirs:
    for file in listdir("../../results/SlidingTilePuzzle/backupTests/AS/" + dir):
        with open("../../results/SlidingTilePuzzle/backupTests/AS/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instanceAS.append(str(dir))
                lookAheadValsAS.append(resultData["Lookahead"])
                algorithmAS.append(algo)
                solutionCostAS.append(resultData[algo])
                differenceCostAS.append(resultData[algo] - resultData["Cserna"])

dfAS = pd.DataFrame({
    "instance":instanceAS,
    "Node Expansion Limit":lookAheadValsAS,
    "Solution Cost":solutionCostAS,
    "Algorithm":algorithmAS
})

dfDiffAS = pd.DataFrame({
    "instance":instanceAS,
    "Node Expansion Limit":lookAheadValsAS,
    "Algorithm Cost - Cserna Cost":differenceCostAS,
    "Algorithm":algorithmAS
})

print("building plots...")

for instance in resultDirs:
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    '''
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
    plt.ylabel("Algorithm Cost - Cserna Cost", color='black', fontsize=18)
    plt.xlabel("Depth Limit", color='black', fontsize=18)
    plt.savefig("../../plots/Experiment1BDifference" + instance + ".pdf", bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()
    '''
    instanceDataAS = dfAS.loc[dfAS["instance"] == instance]
    instanceDataDiffAS = dfDiffAS.loc[dfDiffAS["instance"] == instance]

    makeViolinPlot(13, 10, "Node Expansion Limit", "Solution Cost", instanceDataAS, 0.685, "Algorithm", depthsAS, algorithms, "Node Expansion Limit", "Solution Cost", "../../plots/Experiment1CViolin" + instance + ".pdf")
    
    makeDifferencePlot(13, 10, "Node Expansion Limit", "Algorithm Cost - Cserna Cost", instanceDataDiffAS, 0.35, "Algorithm", depthsAS, algorithms, "Node Expansion Limit", "Algorithm Cost - Cserna Cost", "../../plots/Experiment1CDifference" + instance + ".pdf", markers)

    makeDifferencePlot(13, 10, "Node Expansion Limit", "Algorithm Cost - Cserna Cost", instanceDataDiffAS, 0.35, "Algorithm", depthsAS, algorithmsAAAI19Slide, "Node Expansion Limit", "Algorithm Cost - Cserna Cost", "../../plots/Experiment1CDifference" + instance + "AAAI19Slides.pdf", markers)