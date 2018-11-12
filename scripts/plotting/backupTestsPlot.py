import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

def makeViolinPlot(width, height, xAxis, yAxis, dataframe, dodge, hue, orderList, hueOrderList, xLabel, yLabel, outputName, colorPalette):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, join=False, dodge=dodge, palette=sns.color_palette(["red"]), markers="_", errwidth=3, ci=95)
    ax.tick_params(colors='black', labelsize=12)
    plt.setp(ax.lines, zorder=100)
    plt.setp(ax.collections, zorder=100, label="")
    ax.legend_.remove()
    
    sns.violinplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, palette=colorPalette)    

    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()
    return

def makeDifferencePlot(width, height, xAxis, yAxis, dataframe, dodge, hue, orderList, hueOrderList, xLabel, yLabel, outputName, colorPalette):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, ci=95, join=False, dodge=dodge, palette=colorPalette)
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

# Hard coded result directories
resultDirs = {"b2d100"}
 
colors=["#e6194b", "#3cb44b", "#ffe119", "#0082c8", "#f58231", "#911eb4",
        "#46f0f0", "#f032e6", "#808000", "#fabebe", "#008080", "#e6beff", 
        "#aa6e28", "#800000", "#229954", "#D35400"]

algorithms = ["Minimin", "Bellman", "Nancy", "K-Best 3", "K-Best 10", "K-Best 30", "Cserna", "K-Best 3 One Level Belief", "K-Best 10 One Level Belief", "K-Best 30 One Level Belief", "Cserna One Level Belief"]

algorithmsAAAI19Slides = ["Minimin", "Bellman", "Nancy", "Cserna", "Cserna One Level Belief"]

depthsDFS = [3, 7, 10]

instanceDFS = []
lookAheadValsDFS = []
algorithmDFS = []
solutionCostDFS = []
differenceCostDFS = []

depthsAS = [10, 100, 1000]

instanceAS = []
lookAheadValsAS = []
algorithmAS = []
solutionCostAS = []
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

dfDFS = pd.DataFrame({
    "instance":instanceDFS,
    "Depth Limit":lookAheadValsDFS,
    "Solution Cost":solutionCostDFS,
    "Algorithm":algorithmDFS
})

dfDiffDFS = pd.DataFrame({
    "instance":instanceDFS,
    "Depth Limit":lookAheadValsDFS,
    "Algorithm Cost - Cserna Cost":differenceCostDFS,
    "Algorithm":algorithmDFS
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
    instanceDataDFS = dfDFS.loc[dfDFS["instance"] == instance]
    instanceDataDiffDFS = dfDiffDFS.loc[dfDiffDFS["instance"] == instance]
    instanceDataAS = dfAS.loc[dfAS["instance"] == instance]
    instanceDataDiffAS = dfDiffAS.loc[dfDiffAS["instance"] == instance]

    makeViolinPlot(11, 8, "Depth Limit", "Solution Cost", instanceDataDFS, 0.725, "Algorithm", depthsDFS, algorithms, "Depth Limit", "Solution Cost", "../../plots/Experiment1BViolin" + instance + ".pdf", colors)
    makeDifferencePlot(11, 8, "Depth Limit", "Algorithm Cost - Cserna Cost", instanceDataDiffDFS, 0.5, "Algorithm", depthsDFS, algorithms, "Depth Limit", "Algorithm Cost - Cserna One Level Belief Cost", "../../plots/Experiment1BDifference" + instance + ".pdf", colors)

    makeViolinPlot(11, 8, "Node Expansion Limit", "Solution Cost", instanceDataAS, 0.725, "Algorithm", depthsAS, algorithms, "Node Expansion Limit", "Solution Cost", "../../plots/Experiment1CViolin" + instance + ".pdf", colors)
    makeDifferencePlot(11, 8, "Node Expansion Limit", "Algorithm Cost - Cserna Cost", instanceDataDiffAS, 0.5, "Algorithm", depthsAS, algorithms, "Node Expansion Limit", "Algorithm Cost - Cserna One Level Belief Cost", "../../plots/Experiment1CDifference" + instance + ".pdf", colors)

    makeDifferencePlot(11, 8, "Node Expansion Limit", "Algorithm Cost - Cserna Cost", instanceDataDiffAS, 0.5, "Algorithm", depthsAS, algorithmsAAAI19Slides, "Node Expansion Limit", "Algorithm Cost - Cserna One Level Belief Cost", "../../plots/Experiment1CDifference" + instance + "AAAI19Slides.pdf", colors)