import json

def can_to_mat(cc: str):
    canon_code = list(map(int,cc.split(',')))
    canon_code = canon_code[1:]
    nv = 0
    for i in canon_code:
        if i == 0:
            nv += 1

    adj_matrix = []
    for i in range(nv):
        adj_matrix.append([])
        for j in range(nv):
            adj_matrix[i].append(0)

    adj_matrix[0][1] = 1
    adj_matrix[1][0] = 1

    index = 0
    for curr in range(nv):
        while canon_code[index] != 0:
            adj_matrix[curr][(canon_code[index] - 1) % nv] = 1
            adj_matrix[(canon_code[index] - 1) % nv][curr] = 1

            index += 1
        index += 1

    return adj_matrix

def is_covered(need: list[str], pos: list[list[str]], mp: dict[str, int])-> bool:
    mapped: list[str] = list()
    for elem in need:
        e1 = elem.split(":")[0]
        e2 = elem.split(":")[1]
        mapped.append(str(min(mp[e1], mp[e2])) + ":" + str(max(mp[e1], mp[e2])))
    mapped.sort()

    for p in pos:
        if (p == mapped):
            return True

    return False

def is_covering(needs:list[list[str]], pos: list[list[str]]) -> bool:
    mapping: dict[str, int] = dict()
    for i in range(4):
        mapping[str(i)] = i

    for _ in range(2):
        for _ in range(4):
            for i in range(4):
                mapping[str(i)] = (mapping[str(i)] + 1) % 4

            is_covering_all = True
            for need in needs:
                if (not is_covered(need, pos, mapping)):
                    is_covering_all = False
                    break

            if (is_covering_all):
                return True
            
        #flip
        mapping["1"] = 4 - mapping["1"]
        mapping["3"] = 4 - mapping["3"]
                
    return False

possible_n = [13, 12, 11, 10, 9, 6, 5]
for i in range(len(possible_n)):
    reduced_graphs_file = open(f"reduced_graphs/{possible_n[i]}.txt", "r")
    rg = ""
    for line in reduced_graphs_file:
        rg += line
    rg_json: dict[str, list[list[str]]] = json.loads(rg)

    with open(f"reduced_pairs/{possible_n[i]}.txt", "w") as f:
        for graph in rg_json:
            found_reducer = False
            available = rg_json[graph]

            for j in range(len(possible_n) - 1, i, -1):
                reductors_file = open(f"reduced_graphs/{possible_n[j]}.txt", "r")
                reductors = ""
                for line in reductors_file:
                    reductors += line
                reduc: dict[str, list[list[str]]] = json.loads(reductors)

                for rdc in reduc: 
                    needs_to_cover = reduc[rdc]
                    if is_covering(needs_to_cover, available):
                        adj_matrix1 = can_to_mat(graph[1:-1])
                        adj_matrix2 = can_to_mat(rdc[1:-1])
                        
                        nv = len(adj_matrix1)
                        f.write(graph + ": \n")
                        f.write(str(available) + "\n")
                        for k in range(nv):
                            f.write(" ".join(map(str, adj_matrix1[k])) + "\n")
                        nv = len(adj_matrix2)
                        f.write(rdc + ": \n")
                        f.write(str(needs_to_cover) + "\n")
                        for k in range(nv):
                            f.write(" ".join(map(str, adj_matrix2[k])) + "\n")

                        found_reducer = True
                        break
                if found_reducer: break