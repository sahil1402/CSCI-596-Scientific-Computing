#!/usr/bin/env python3
"""Create publication-style plots from a HarborTune design sweep."""
from __future__ import annotations
import argparse
from pathlib import Path
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

POSITIONS=["left","left-center","center","right-center","right"]
WIDTHS=["narrow","medium","wide"]
WAVES=["short","medium","long","pulse"]

def save(fig:plt.Figure,path:Path)->None:
    fig.tight_layout();fig.savefig(path,dpi=180,bbox_inches="tight");plt.close(fig)

def main()->None:
    p=argparse.ArgumentParser();p.add_argument("sweep_directory",type=Path);p.add_argument("output_directory",type=Path);a=p.parse_args();a.output_directory.mkdir(parents=True,exist_ok=True)
    plt.rcParams.update({"figure.dpi":140,"axes.grid":True,"grid.alpha":.22,"axes.spines.top":False,"axes.spines.right":False,"font.size":10})
    results=pd.read_csv(a.sweep_directory/"sweep_results.csv");rankings=pd.read_csv(a.sweep_directory/"design_rankings.csv").sort_values("rank");rankings["design"]=rankings.position+" / "+rankings.width
    fig,ax=plt.subplots(figsize=(8,5.5));ordered=rankings.iloc[::-1];ax.barh(ordered.design,ordered.robust_score,color=plt.cm.viridis_r(np.linspace(.15,.85,len(ordered))));ax.set_xlabel("Worst-case normalized composite score (lower is better)");ax.set_title("Robust Harbor Design Ranking");save(fig,a.output_directory/"robust_design_ranking.png")
    matrix=rankings.pivot(index="width",columns="position",values="robust_score").reindex(index=WIDTHS,columns=POSITIONS);fig,ax=plt.subplots(figsize=(8,3.5));im=ax.imshow(matrix.values,cmap="viridis_r",aspect="auto");ax.set_xticks(range(len(POSITIONS)),POSITIONS,rotation=25,ha="right");ax.set_yticks(range(len(WIDTHS)),WIDTHS);ax.set(xlabel="Entrance position",ylabel="Entrance width",title="Worst-Case Composite Score");
    for row in range(len(WIDTHS)):
        for col in range(len(POSITIONS)):ax.text(col,row,f"{matrix.iloc[row,col]:.2f}",ha="center",va="center")
    fig.colorbar(im,ax=ax,label="Normalized score");save(fig,a.output_directory/"design_score_heatmap.png")
    fig,ax=plt.subplots(figsize=(7,5));sc=ax.scatter(rankings.worst_normalized_peak,rankings.worst_normalized_speed,c=rankings.robust_score,cmap="viridis_r",s=75,edgecolor="black",linewidth=.4)
    for row in rankings.itertuples():ax.annotate(f"{row.position}\n{row.width}",(row.worst_normalized_peak,row.worst_normalized_speed),xytext=(4,4),textcoords="offset points",fontsize=6.7)
    ax.axvline(1,color="grey",ls="--",lw=1);ax.axhline(1,color="grey",ls="--",lw=1);ax.set(xlabel="Worst normalized peak elevation",ylabel="Worst normalized entrance speed",title="Elevation–Current Design Tradeoff");fig.colorbar(sc,ax=ax,label="Robust score");save(fig,a.output_directory/"elevation_current_tradeoff.png")
    results["design"]=results.position+" / "+results.width;case=results.pivot(index="design",columns="wave",values="score").reindex(columns=WAVES).loc[rankings.design];fig,ax=plt.subplots(figsize=(7,6));im=ax.imshow(case.values,cmap="magma_r",aspect="auto");ax.set_xticks(range(len(WAVES)),WAVES);ax.set_yticks(range(len(case)),case.index,fontsize=7.5);ax.set(xlabel="Wave scenario",title="Composite Score by Design and Wave Scenario");fig.colorbar(im,ax=ax,label="Normalized score");save(fig,a.output_directory/"scenario_score_heatmap.png")
    print(f"Wrote sweep figures to {a.output_directory.resolve()}")
if __name__=="__main__":main()
