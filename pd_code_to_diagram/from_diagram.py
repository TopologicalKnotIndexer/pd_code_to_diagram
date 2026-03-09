# 由于扭结可能有定向冲突问题
# 因此需要编写一个从 diagram 到 pd_code 的检查来保证确实是同一个 pd_code

DX = [ 0, -1,  0, 1] # 东、北、西、南
DY = [ 1,  0, -1, 0]

def add_edge(pre:dict[int, int], nxt:dict[int, int], val1, val2):
    if val1 > val2:
        val1, val2 = val2, val1 # 保证 val1 <= val2
    
    if val2 - val1 == 1:
        if pre.get(val2) is not None and pre[val2] != val1:
            raise ValueError()
        if nxt.get(val1) is not None and nxt[val1] != val2:
            raise ValueError()
        pre[val2] = val1
        nxt[val1] = val2

    else:
        if pre.get(val1) is not None and pre[val1] != val2:
            raise ValueError()
        if nxt.get(val2) is not None and nxt[val2] != val1:
            raise ValueError()
        pre[val1] = val2
        nxt[val2] = val1

def get_pre_nxt_from_diagram(
        diagram:list[list[int]], 
        row_cnt, col_cnt) -> tuple[dict[int, int], dict[int, int]]:
    
    pre = dict()
    nxt = dict()
    for i in range(row_cnt):
        for j in range(col_cnt):
            if diagram[i][j] < 0:

                # -1, -2 不可以出现在矩阵边界
                if not (1 <= i <= row_cnt - 2) or not (1 <= j <= col_cnt - 2):
                    raise ValueError()
                
                for d in range(2):
                    now_x, now_y = i + DX[d + 0], j + DY[d + 0]
                    nxt_x, nxt_y = i + DX[d + 2], j + DY[d + 2]

                    add_edge(pre, nxt, diagram[now_x][now_y], diagram[nxt_x][nxt_y])

    # 特殊处理长度为 2 的连通分支
    for item in pre:
        if nxt.get(item) is None:
            nxt[item] = pre[item]
    for item in nxt:
        if pre.get(item) is None:
            pre[item] = nxt[item]

    if len(pre) != len(nxt):
        raise AssertionError()
    return pre, nxt

def value_arround(diagram:list[list[int]], i:int, j:int) -> list[int]:
    return [
        diagram[i + DX[d]][j + DY[d]]
        for d in range(4)]

# 包含了 set 中的每一个值
def contains(val_list:list[int], component_set:set[int]) -> bool:
    for val in component_set:
        if val not in val_list:
            return False
    return True

# xi, xj 是当前交叉点中心
# fi, fj 是当前交叉点旁边的一个点
def get_dir(xi:int, xj:int, fi:int, fj:int, in_out:str) -> int:
    if in_out not in ["in", "out"]:
        raise AssertionError()
    
    dir_now = -1

    # 从旁边的点前进到交叉点所需要的方向
    # 假设为 in，计算方向
    dx = xi - fi
    dy = xj - fj
    for d in range(4):
        if dx == DX[d] and dy == DY[d]:
            dir_now = d
    
    # 如果实际方向是 out，那么就反向一下
    if in_out == "out":
        dir_now = (dir_now + 2) % 4

    if not isinstance(dir_now, int):
        raise AssertionError()
    return dir_now

def process_len_2_components(
        diagram:list[list[int]], 
        row_cnt:int, col_cnt:int, 
        len_2_components:list[set[int]], 
        direction_matrix:list[list]):
    
    for component_set in len_2_components:
        
        # 寻找相关的两个 crossings
        crossings = []

        for i in range(row_cnt):
            for j in range(col_cnt):
                if diagram[i][j] < 0 and contains(value_arround(diagram, i, j), component_set):
                    crossings.append((i, j))

        # 只可能有 1 个或者 2 个 crossing 相关
        if len(crossings) == 0 or len(crossings) > 2:
            raise AssertionError()
        
        if len(crossings) == 1:

            # 只有一个 crossing 只能是一个 [a, b, b, a] 或者 [a, a, b, b]
            xi, xj = crossings[0][0], crossings[0][1]
            if set(value_arround(diagram, xi, xj)) != component_set:
                raise AssertionError()
            
            direction_matrix[xi + 0][xj + 1] = "in"
            direction_matrix[xi + 0][xj - 1] = "out"

            # 由于每个值都是进一次，出一次，所以只需要看另外两个元素的值
            val1 = diagram[xi + 0][xj + 1]
            val2 = diagram[xi + 0][xj - 1]
            for d in range(4):
                if direction_matrix[xi + DX[d]][xj + DY[d]] == "":
                    if diagram[xi + DX[d]][xj + DY[d]] == val1:
                        direction_matrix[xi + DX[d]][xj + DY[d]] = "out"
                    else:
                        if diagram[xi + DX[d]][xj + DY[d]] != val2:
                            raise AssertionError()
                        direction_matrix[xi + DX[d]][xj + DY[d]] = "in"

            # 把 in, out 改写成 up, down, left, right
            for d in range(4):
                direction_matrix[xi + DX[d]][xj + DY[d]] = get_dir(
                    xi, xj, xi + DX[d], xj + DY[d], direction_matrix[xi + DX[d]][xj + DY[d]]
                )
            return
        
        else:
            
            # 需要保证每个编号进一次出一次就行
            if len(crossings) != 2:
                raise AssertionError()
            
            # 给这个二元素集合定序
            val1 = list(component_set)[0] # 让 val1 对应 in
            val2 = list(component_set)[1] # val2 对应 out

            for crossing in crossings:
                xi, xj = crossing

                for d in range(4):
                    if diagram[xi + DX[d]][xj + DY[d]] == val1:
                        direction_matrix[xi + DX[d]][xj + DY[d]] = "in"

                    elif diagram[xi + DX[d]][xj + DY[d]] == val2:
                        direction_matrix[xi + DX[d]][xj + DY[d]] = "out"

                # 这里交换之后，下一个交叉点就能恰好实现相反方向
                val1, val2 = val2, val1

            # 把 in, out 转换成方向编码
            for crossing in crossings:
                xi, xj = crossing

                for d in range(4):
                    if direction_matrix[xi + DX[d]][xj + DY[d]] in ["in", "out"]:
                        direction_matrix[xi + DX[d]][xj + DY[d]] = get_dir(
                            xi, xj, xi + DX[d], xj + DY[d], direction_matrix[xi + DX[d]][xj + DY[d]]
                        )
                        if not isinstance(direction_matrix[xi + DX[d]][xj + DY[d]], int):
                            raise AssertionError()
            return

def get_pd_code_crossing(
        diagram:list[list[int]], 
        direction_matrix:list[list], 
        xi:int, xj:int) -> list[int]:
    
    dir = ["", "", "", ""]
    for d in range(4):
        diretion_now = direction_matrix[xi + DX[d]][xj + DY[d]]
        if diretion_now not in [d, (d + 2) % 4]:
            raise AssertionError()
        if diretion_now == d:
            dir[d] = "out"
        else:
            dir[d] = "in"
    
    if diagram[xi][xj] == -1:
        dir_set = [1, 3]
    
    else:
        if diagram[xi][xj] != -2:
            raise AssertionError()
        
        dir_set = [0, 2]

    # 找到下方进入的那一个方向
    down_dir = -1
    for d in dir_set:
        if dir[d] == "in":
            down_dir = d

    if down_dir == -1:
        raise AssertionError()
    
    return [
        diagram[xi + DX[(down_dir + i) % 4]][xj + DY[(down_dir + i) % 4]]
        for i in range(4)
    ]

# 注意由于 len2 分支有多种合法定向，所以结果可能不唯一
def diagram_to_pd_code(diagram:list[list[int]]) -> list[list[int]]:

    if not isinstance(diagram, list):
        raise TypeError()
    
    if len(diagram) == 0:
        raise ValueError()
    
    # 检查类型正确
    for raw in diagram:
        if not isinstance(raw, list):
            raise TypeError()
        if len(raw) == 0:
            raise ValueError()
    
    # 先检查矩阵的形状规则
    row_cnt = len(diagram)
    col_cnt = len(diagram[0])

    # 检查每一行元素个数相同
    # 由于我们只能对 diagram 做检查
    # 而 diagram 中所有编号都是数字，因此要求所有元素都是 int
    for i in range(len(diagram)):
        if len(diagram[i]) != col_cnt:
            raise ValueError()
        for j in range(col_cnt):
            if not isinstance(diagram[i][j], int):
                raise TypeError()
    
    # "" 表示该位置不需要定向或者还没有确定定向
    direction_matrix = [
        ["" for _ in range(col_cnt)]
        for _ in range(row_cnt)]

    # 确定前驱后继关系，并从中得到长度为 2 的连通分支
    pre, nxt = get_pre_nxt_from_diagram(diagram, row_cnt, col_cnt)
    len_2_components = []
    for item in pre:
        if pre[item] == nxt[item] and set([item, pre[item]]) not in len_2_components:
            len_2_components.append(set([item, pre[item]])) # 找到长度为 2 的连通分支
    
    # 处理所有长度为 2 的连通分支，处理后就不用处理了
    if len(len_2_components) != 0:
        process_len_2_components(diagram, row_cnt, col_cnt, len_2_components, direction_matrix)
    
    # 枚举交叉点
    for xi in range(row_cnt):
        for xj in range(col_cnt):
            if diagram[xi][xj] >= 0: # 跳过非交叉点
                continue
            for d in range(4):
                if direction_matrix[xi + DX[d]][xj + DY[d]] == "":
                    hr_val = diagram[xi + DX[d]][xj + DY[d]]
                    op_val = diagram[xi + DX[(d + 2) % 4]][xj + DY[(d + 2) % 4]]
                    if op_val == nxt[hr_val]:
                        direction_matrix[xi + DX[d]][xj + DY[d]] = "in"
                    else:
                        direction_matrix[xi + DX[d]][xj + DY[d]] = "out"

                    if direction_matrix[xi + DX[d]][xj + DY[d]] not in ["in", "out"]:
                        raise AssertionError()
            # 确定了这个交叉点的定向之后，再扫描成具体方向
            for d in range(4):
                if direction_matrix[xi + DX[d]][xj + DY[d]] in ["in", "out"]:
                    direction_matrix[xi + DX[d]][xj + DY[d]] = get_dir( # type:ignore
                        xi, xj, xi + DX[d], xj + DY[d], direction_matrix[xi + DX[d]][xj + DY[d]]
                    ) 
                    if not isinstance(direction_matrix[xi + DX[d]][xj + DY[d]], int):
                        raise AssertionError()
    
    # 完成了所有交叉点的定向
    pd_code = []
    for xi in range(row_cnt):
        for xj in range(col_cnt):
            if diagram[xi][xj] >= 0: # 跳过非交叉点
                continue
            pd_code.append(get_pd_code_crossing(diagram, direction_matrix, xi, xj))
    return sorted(pd_code)

if __name__ == "__main__":
    diagram_now = [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 1, 0, 11, 11, 11, 11, -2, 10, 10, 10, 10, 0, 0, 0], [0, 0, 0, 1, 0, 11, 0, 0, 0, 2, 0, 0, 0, 10, 0, 0, 0], [0, 0, 0, 1, 0, 11, 0, 9, 9, -1, 10, 10, 10, 10, 0, 0, 0], [0, 0, 0, 1, 0, 11, 0, 9, 0, 3, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 1, 0, 11, 0, 9, 9, -2, 12, 12, 0, 0, 0, 0, 0], [0, 0, 0, 1, 0, 11, 0, 0, 0, 4, 0, 12, 0, 0, 0, 0, 0], [0, 0, 0, 1, 0, 11, 11, 11, 11, -1, 12, 12, 0, 0, 0, 0, 0], [0, 0, 0, 1, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0], [0, 8, 8, -1, 7, 7, 7, 7, 7, -2, 8, 8, 8, 8, 8, 8, 0], [0, 8, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 8, 0], [0, 8, 0, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 8, 0], [0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0], [0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]
    print(diagram_to_pd_code(diagram_now))
