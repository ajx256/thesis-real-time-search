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
    
    sns.violinplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, palette=sns.color_palette(colors))    

    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()
    return

def makeDifferencePlot(width, height, xAxis, yAxis, dataframe, dodge, hue, orderList, hueOrderList, xLabel, yLabel, outputName, colorPalette):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, ci=95, join=False, dodge=dodge, palette=sns.color_palette(colorPalette))
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

# Hard coded result directories
resultDirs = {"b2d10"}

colors=["#e6194b", "#3cb44b", "#ffe119", "#0082c8", "#f58231", "#911eb4",
        "#46f0f0", "#f032e6", "#808000", "#fabebe", "#008080", "#e6beff", 
        "#aa6e28", "#800000", "#229954", "#D35400"]

algorithms = ["Minimin", "Bellman", "Nancy", "K-Best 1 Correct Belief", "K-Best 2 Correct Belief", "K-Best 3 Correct Belief", "K-Best 5 Correct Belief", 
              "K-Best 7 Correct Belief", "Cserna Correct Belief", "K-Best 1", "K-Best 2", "K-Best 3", "K-Best 5", 
              "K-Best 7", "Cserna"]

instance = []
lookAheadVals = []
algorithm = []
solutionCost = []
differenceCost = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../../results/TreeWorld/lastIncrementalDecision/" + dir):
        with open("../../results/TreeWorld/lastIncrementalDecision/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)
            for algo in algorithms:
                instance.append(str(dir))
                lookAheadVals.append(resultData["Lookahead"])
                algorithm.append(algo)
                solutionCost.append(resultData[algo.replace("Correct", "Pemberton")])
                differenceCost.append(resultData[algo.replace("Correct", "Pemberton")] - resultData["Cserna Pemberton Belief"])

df = pd.DataFrame({
    "instance":instance,
    "Depth Limit":lookAheadVals,
    "Solution Cost":solutionCost,
    "Algorithm":algorithm
})

dfDiff = pd.DataFrame({
    "instance":instance,
    "Depth Limit":lookAheadVals,
    "Algorithm Cost - Cserna Cost":differenceCost,
    "Algorithm":algorithm
})

print("building plots...")

for instance in resultDirs:
    instanceData = df.loc[df["instance"] == instance]

    depths = []

    if instanceData["Depth Limit"].iloc[0] == 14:
        depths.append(14)
    elif instanceData["Depth Limit"].iloc[0] == 9:
        depths.append(9)
    
    makeViolinPlot(11, 8, "Depth Limit", "Solution Cost", instanceData, 0.75, "Algorithm", depths, algorithms, "Depth Limit", "Solution Cost", "../../plots/Experiment1AViolin" + instance + ".pdf")
	    
    instanceDataDiff = dfDiff.loc[dfDiff["instance"] == instance]

    makeDifferencePlot(11, 8, "Depth Limit", "Algorithm Cost - Cserna Cost", instanceDataDiff, 0.3, "Algorithm", depths, algorithms, "Depth Limit", "Algorithm Cost - Cserna Correct Belief Cost", "../../plots/Experiment1ADifference" + instance + ".pdf", colors)