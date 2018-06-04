import matplotlib.pyplot as plt
import pandas as pd
import json
import seaborn as sns
from os import listdir

# Hard coded result directories
resultDirs = {"b2d10", "b2d100", "b2d1000"}

instance = []
kbestWins = []
miniminWins = []

print("reading in data...")

wins = { "minimin":0, "k-best":0 }
for file in listdir("../results/lastIncrementalDecision/b2d10"):
	with open("../results/lastIncrementalDecision/b2d10/" + file) as json_data:
		resultData = json.load(json_data)
		if (resultData["winner"] != "tie"):
			wins[resultData["winner"]] += 1
instance.append(10)
kbestWins.append((wins["k-best"] / 1000) * 100)
miniminWins.append((wins["minimin"] / 1000) * 100)

wins = { "minimin":0, "k-best":0 }
for file in listdir("../results/lastIncrementalDecision/b2d100"):
	with open("../results/lastIncrementalDecision/b2d100/" + file) as json_data:
		resultData = json.load(json_data)
		if (resultData["winner"] != "tie"):
			wins[resultData["winner"]] += 1
instance.append(100)
kbestWins.append((wins["k-best"] / 1000) * 100)
miniminWins.append((wins["minimin"] / 1000) * 100)

wins = { "minimin":0, "k-best":0 }
for file in listdir("../results/lastIncrementalDecision/b2d1000"):
	with open("../results/lastIncrementalDecision/b2d1000/" + file) as json_data:
		resultData = json.load(json_data)
		if (resultData["winner"] != "tie"):
			wins[resultData["winner"]] += 1
instance.append(1000)
kbestWins.append((wins["k-best"] / 1000) * 100)
miniminWins.append((wins["minimin"] / 1000) * 100)

df = pd.DataFrame({
    "K-Best 7":instance,
    "Minimin":instance,
    "K-Best Wins":kbestWins,
    "Minimin Wins":miniminWins
})

print("building plots...")


plt.plot(df["K-Best 7"], df["K-Best Wins"])
plt.plot(df["Minimin"], df["Minimin Wins"])
plt.title("Minimin v K-Best Percent Wins")
plt.legend()
plt.ylabel("Percent Wins")
plt.xlabel("Tree Depth")
plt.savefig("../plots/LastIncrementalDecision.png")
plt.close()
