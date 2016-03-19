from itertools import *
import networkx as nx
import random

def powerset(iterable):
    s = list(iterable)
    return chain.from_iterable(combinations(s, r) for r in range(len(s) + 1))

def number_of_cuts(G):
    edge_list = G.edges()
    count = 0
    for e in powerset(range(len(edge_list))):
        H = nx.Graph()
        H.add_nodes_from(range(len(G.nodes())))
        #print H.nodes()
        for i in e:
            H.add_edge(*edge_list[i])
        #print H.edges()
        if not nx.is_connected(H):
            count += 1
            #print H.edges()
    return count

def number_of_rcuts(G, r1, r2):
    edge_list = G.edges()
    count = 0
    for e in powerset(range(len(edge_list))):
        H = nx.Graph()
        H.add_nodes_from(range(len(G.nodes())))
        #print H.nodes()
        for i in e:
            H.add_edge(*edge_list[i])
        #print H.edges()
        if not nx.has_path(H, r1, r2) and nx.number_connected_components(H) == 2:
            count += 1
            #print H.edges()
    return count

def number_of_rforest(G, r1, r2):
    edge_list = G.edges()
    count = 0
    for e in powerset(range(len(edge_list))):
        H = nx.Graph()
        H.add_nodes_from(range(len(G.nodes())))
        #print H.nodes()
        for i in e:
            H.add_edge(*edge_list[i])
        #print H.edges()
        if not nx.has_path(H, r1, r2) and nx.number_connected_components(H) == 2 and nx.is_forest(H):
            count += 1
            #print H.edges()
    return count

def number_of_pathmatching(G):
    edge_list = G.edges()
    count = 0
    for e in powerset(range(len(edge_list))):
        H = nx.Graph()
        H.add_nodes_from(range(len(G.nodes())))
        #print H.nodes()
        for i in e:
            H.add_edge(*edge_list[i])
        #print H.edges()
        is_pm = True
        for v in G.nodes():
            if nx.degree(H, v) > 2:
                is_pm = False
                break
        if is_pm:
            if nx.is_forest(H):
                count += 1
                #print H.edges()
    return count

def number_of_partition(G):
    count = 0
    for n in powerset(range(len(G.nodes()))):
        if len(n) >= 1 and len(n) < G.number_of_nodes():
            H1 = G.subgraph(n)
            if not nx.is_connected(H1):
                continue
            nbar = []
            for i in range(0, len(G.nodes())):
                if i not in n:
                    nbar.append(i)
            H2 = G.subgraph(nbar)
            if not nx.is_connected(H2):
                continue
            count += 1
    return count / 2

def number_of_3partition(G):
    edge_list = G.edges()
    count = 0
    for n in powerset(range(len(G.nodes()))):
        if len(n) == 0:
            continue
        H1 = G.subgraph(n)
        if not nx.is_connected(H1):
            continue
        nbar1 = []
        for i in range(0, len(G.nodes())):
            if i not in n:
                nbar1.append(i)
        for n2 in powerset(nbar1):
            if len(n2) == 0:
                continue
            H2 = G.subgraph(n2)
            if not nx.is_connected(H2):
                continue
            nbar = []
            for i in range(0, len(G.nodes())):
                if i not in n and i not in n2:
                    nbar.append(i)
            if len(nbar) == 0:
                continue
            H3 = G.subgraph(nbar)
            if not nx.is_connected(H3):
                continue
            count += 1
    return count / 6

def output_graph_as_edge_list(G, f):
    f.write(str(len(G.nodes())))
    f.write('\n')
    for e in G.edges():
        x = '{0:d} {1:d}\n'.format(e[0] + 1, e[1] + 1)
        f.write(x)

for i in range(0, 3):
    G = nx.gnp_random_graph(8, 0.2 + i * 0.2, i + 1)
    edge_list = G.edges()

    print edge_list
    print "is_connected = " + str(nx.is_connected(G)) + ", # of cuts = " + str(number_of_cuts(G))
    print "# of rcuts = " + str(number_of_rcuts(G, 0, len(G.nodes()) - 1))
    print "# of rforest = " + str(number_of_rforest(G, 0, len(G.nodes()) - 1))
    print "# of pathmatching = " + str(number_of_pathmatching(G))
    print "# of partition = " + str(number_of_partition(G))
    print "# of 3partition = " + str(number_of_3partition(G))

    s = 0
    t = 0
    while s == t:
        s = random.randrange(0, len(G.nodes()))
        t = random.randrange(0, len(G.nodes()))

    print "# of {0:d}-{1:d} paths = {2:d}".format(s + 1, t + 1, len(list(nx.all_simple_paths(G, source = s, target = t))))

    f = open("random_graph{0:d}.txt".format(i + 1), "w")
    output_graph_as_edge_list(G, f)
    f.close()
