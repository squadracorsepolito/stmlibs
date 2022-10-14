# "THE BEER-WARE LICENSE" (Revision 69):
# Squadra Corse firmware team wrote this file. As long as you retain this notice
# you can do whatever you want with this stuff. If we meet some day, and you
# think this stuff is worth it, you can buy us a beer in return.
#
# Authors
# - Filippo Rossi <filippo.rossi.sc@gmail.com>

from glob import glob
from io import TextIOWrapper
from pathlib import Path

import click
import jinja2 as j2
import networkx as nx

dir = Path(__file__).parent
source = j2.Template((dir / "fsm.c.j2").read_text())
header = j2.Template((dir / "fsm.h.j2").read_text())


@click.command()
@click.argument("name", type=str)
@click.argument("dot", type=click.File("r"))
def main(name: str, dot: TextIOWrapper):
    global source, header

    G = nx.drawing.nx_pydot.read_dot(dot)
    assert nx.is_directed(G), "Graph must be directed"
    assert nx.is_weakly_connected(G), "Graph must be weakly connected"
    G = nx.MultiDiGraph(G)  # Cast to MultiDiGraph to allow parallel edges

    states = [state for state in G.nodes]
    out_edges = {state: G.out_edges(state) for state in states}

    args = {
        "name": name,
        "states": states,
        "out_edges": out_edges,
    }

    cwd = Path.cwd()

    (cwd / f"{name}.c").write_text(source.render(**args))
    (cwd / f"{name}.h").write_text(header.render(**args))


if __name__ == "__main__":
    main()
