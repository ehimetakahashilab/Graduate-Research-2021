import collections
from graphviz import Digraph

## APIコール列の読み込み ##
with open(r"C:\Users\Nagao\Desktop\Feature_Dataset\trojan.win32.poweliks\18.txt", 'r') as f:
    apicalls = f.read()

apicall_list = apicalls.split(" ")

##　連想配列の生成　##
apicall_graph = collections.OrderedDict()
for i in range(len(apicall_list)-1):
    api_arrow = apicall_list[i] + "->" + apicall_list[i+1]
    if api_arrow in apicall_graph.keys():
        apicall_graph[api_arrow] += 1
    else:
        apicall_graph[api_arrow] = 1

## グラフの作成 ##
graphviz = Digraph(format = "png")

if len(apicall_graph) > 1:

    for edge, num_edge in apicall_graph.items():
        call_1, call_2 = edge.split("->")
        graphviz.edge(call_1, call_2, label=str(num_edge))



graphviz.render(r"C:\Users\Nagao\Desktop\graph\poweliks_18_graph")