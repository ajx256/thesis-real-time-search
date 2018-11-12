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

def makeDifferencePlot(width, height, xAxis, yAxis, dataframe, dodge, hue, orderList, hueOrderList, xLabel, yLabel, outputName):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, ci=95, join=False, dodge=dodge, palette="Set2")
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def makeOptimalityGapPlot(width, height, xAxis, yAxis, dataframe, dodge, hue, orderList, hueOrderList, xLabel, yLabel, outputName):
    sns.set(rc={'figure.figsize': (width, height), 'font.size': 26, 'text.color': 'black'})
    ax = sns.pointplot(x=xAxis, y=yAxis, hue=hue, order=orderList, hue_order=hueOrderList, data=dataframe, ci=95, join=False, dodge=dodge, palette="Set2")
    ax.tick_params(colors='black', labelsize=12)
    plt.ylabel(yLabel, color='black', fontsize=18)
    plt.xlabel(xLabel, color='black', fontsize=18)
    plt.savefig(outputName, bbox_inches="tight", pad_inches=0)
    
    plt.close()
    plt.clf()
    plt.cla()
    return

korfCosts = {}

with open("../utility/KorfTrueCosts.json") as costs:
	korfCosts = json.load(costs)

# Hard coded result directories
resultDirs = {"4x4"}
limits = [3, 10, 30, 100, 300, 1000]
revisedLimits = [30, 100, 300, 1000]

algorithms = ["A*", "F-Hat", "BFS", "Risk", "LSS-LRTA*", "Confidence"]

instance = []
lookAheadVals = []
algorithm = []
solutionCost = []
differenceCost = []

gapSums = {}

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir):
        with open("../../results/SlidingTilePuzzle/expansionTests/Nancy/" + dir + "/" + file) as json_data:
            resultData = json.load(json_data)

            if resultData["Lookahead"] not in gapSums:
                gapSums[resultData["Lookahead"]] = {"A*" : 0, "Risk" : 0, "F-Hat" : 0, "BFS" : 0, "LSS-LRTA*" : 0, "Confidence" : 0}

            # Find the optimal cost
            instanceKorf = file.split('-')[1].split('.')[0]

            for algo in algorithms:
                instance.append(str(dir))
                lookAheadVals.append(resultData["Lookahead"])
                algorithm.append(algo)
                solutionCost.append(resultData[algo])
                differenceCost.append(resultData[algo] - resultData["A*"])
                gapSums[resultData["Lookahead"]][algo] =  gapSums[resultData["Lookahead"]][algo] + (resultData[algo] - korfCosts[instanceKorf])

dfExp = pd.DataFrame({
    "instance":instance,
    "Node Expansion Limit":lookAheadVals,
    "Solution Cost":solutionCost,
    "Algorithm":algorithm
})

dfDiffExp = pd.DataFrame({
    "instance":instance,
    "Node Expansion Limit":lookAheadVals,
    "Algorithm Cost - A* Cost":differenceCost,
    "Algorithm":algorithm
})

algorithmsExpA = ["A*", "F-Hat"]

algorithmsExpB = ["A*", "F-Hat", "BFS"]

algorithmsExpC = ["A*", "F-Hat", "BFS", "Risk", "LSS-LRTA*"]

algorithmsOpt = ["A*", "F-Hat", "Risk", "LSS-LRTA*"]

instanceDiffExpCOpt = []
lookAheadValsDiffExpCOpt = []
optimalGap = []
algorithmDiffExpCOpt = []

for la in gapSums:
    for algo in gapSums[la]:
        instanceDiffExpCOpt.append("4x4")
        lookAheadValsDiffExpCOpt.append(la)
        optimalGap.append((gapSums[la][algo] / 100) / (gapSums[la]["A*"] / 100))
        algorithmDiffExpCOpt.append(algo)

dfOpt = pd.DataFrame({
    "instance":instanceDiffExpCOpt,
    "Node Expansion Limit":lookAheadValsDiffExpCOpt,
    "Average Optimality Gap":optimalGap,
    "Algorithm":algorithmDiffExpCOpt
})

print("building plots...")

for instance in resultDirs:
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    instanceDataExp = dfExp.loc[dfExp["instance"] == instance]
    instanceDataDiffExp = dfDiffExp.loc[dfDiffExp["instance"] == instance]
    instanceDataOpt = dfOpt.loc[dfOpt["instance"] == instance]

    makeViolinPlot(11, 8, "Node Expansion Limit", "Solution Cost", instanceDataExp, 0.4, "Algorithm", limits, algorithmsExpA, "Node Expansion Limit", "Solution Cost", "../../plots/Experiment2AViolin" + instance + ".pdf")

    makeDifferencePlot(11, 8, "Node Expansion Limit", "Algorithm Cost - A* Cost", instanceDataDiffExp, 0.35, "Algorithm", limits, algorithmsExpA, "Node Expansion Limit", "Algorithm Cost - A* Cost", "../../plots/Experiment2ADifference" + instance + ".pdf")

    makeViolinPlot(11, 8, "Node Expansion Limit", "Solution Cost", instanceDataExp, 0.53, "Algorithm", limits, algorithmsExpB, "Node Expansion Limit", "Solution Cost", "../../plots/Experiment2BViolin" + instance + ".pdf")

    makeDifferencePlot(11, 8, "Node Expansion Limit", "Algorithm Cost - A* Cost", instanceDataDiffExp, 0.35, "Algorithm", limits, algorithmsExpB, "Node Expansion Limit", "Algorithm Cost - A* Cost", "../../plots/Experiment2BDifference" + instance + ".pdf")

    makeViolinPlot(11, 12, "Node Expansion Limit", "Solution Cost", instanceDataExp, 0.65, "Algorithm", limits, algorithmsExpC, "Node Expansion Limit", "Solution Cost", "../../plots/Experiment2CViolin" + instance + ".pdf")

    makeDifferencePlot(11, 8, "Node Expansion Limit", "Algorithm Cost - A* Cost", instanceDataDiffExp, 0.35, "Algorithm", limits, algorithmsExpC, "Node Expansion Limit", "Algorithm Cost - A* Cost", "../../plots/Experiment2CDifference" + instance + ".pdf")

    makeDifferencePlot(11, 8, "Node Expansion Limit", "Algorithm Cost - A* Cost", instanceDataDiffExp, 0.35, "Algorithm", limits, algorithmsExpC, "Node Expansion Limit", "Algorithm Cost - A* Cost", "../../plots/Experiment2CDifference" + instance + "AAAI19Slides.pdf")

    makeOptimalityGapPlot(11, 8, "Node Expansion Limit", "Average Optimality Gap", instanceDataOpt, 0.35, "Algorithm", revisedLimits, algorithmsOpt, "Node Expansion Limit", "Average Optimality Gap (% of A*)", "../../plots/Experiment2COptimalityGap" + instance + ".pdf")