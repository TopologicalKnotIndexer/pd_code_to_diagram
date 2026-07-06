import json

import cpp_simple_interface
import pd_code_sanity
import os
from typing import Optional

try:
    from .run_file import run_program_with_input
    from .from_diagram import diagram_to_pd_code
except:
    from run_file import run_program_with_input
    from from_diagram import diagram_to_pd_code

DIRNOW = os.path.dirname(os.path.abspath((__file__)))
CPP_MAIN = os.path.join(DIRNOW, "cpp_src", "main.cpp")
BIN_FOLDER = os.path.join(DIRNOW, "bin")
EXE_FILE = os.path.join(BIN_FOLDER, "pd_code_to_diagram.exe")

# 确保 exe 文件已经存在
def create_exe_file() -> tuple[bool, str]:

    # 确保二进制文件夹存在
    os.makedirs(BIN_FOLDER, exist_ok=True)

    if os.path.isfile(EXE_FILE): # 已经存在了，不用重新生成
        return True, "Success: EXE_FILE already compiled."
    
    # 试图进行编译
    return cpp_simple_interface.compile_cpp_files(
        cpp_files=[CPP_MAIN],
        exe_output_path=EXE_FILE
    )

# 给定一个 pd_code，计算它的平面图放置方案
#  0 表示空地
# -1 表示纵向线在下方
# -2 表示横向线在下方
#  x 表示编号为 x 的弧段
def get_diagram_from_pd_code(
        pd_code:list[list[int]], border_val:Optional[int]=None) -> list[list[int]]:

    # 确保 exe 程序存在
    suc, msg = create_exe_file()
    if not suc:
        raise RuntimeError(msg)

    if not pd_code_sanity.sanity(pd_code): # type: ignore
        raise TypeError()
    
    # 这里只能支持 int 模式的 pd_code
    for crossing in pd_code:
        for item in crossing:
            if not isinstance(item, int):
                raise TypeError()
    
    # 考虑边界元素
    arg_list = ["--diagram", "--with_zero"]
    if border_val is not None and border_val > 0:
        arg_list += ["--" + str(border_val)]
    
    # 运行程序
    stdout_val, stderr_val, ret_val = run_program_with_input(
        EXE_FILE, arg_list,
        str(pd_code))

    if ret_val != 0:
        raise RuntimeError(stderr_val)
    
    # 截获所有行内容
    arr = []
    for line in stdout_val.split("\n"):
        line = line.strip()
        if line == "": # 跳过空行
            continue
        line_now = []
        for term in line.split(): # 枚举同一行内的所有数值
            term = term.strip()
            if term == "":
                continue
            line_now.append(int(term))
        arr.append(line_now)
    return arr

# 得到平面图的序列化表示
def get_diagram_str_from_pd_code(
        pd_code:list[list[int]], border_val:Optional[int]=None) -> str:
    diagram_matrix = get_diagram_from_pd_code(pd_code, border_val)

    # 计算最大长度
    max_len = 0
    for line in diagram_matrix:
        for val in line:
            max_len = max(max_len, len(str(val)))
    max_len += 1

    ans = ""
    for line in diagram_matrix:
        line_now = ""
        for item in line:
            if item == 0:
                line_now += (" " * max_len)
            else:
                line_now += ("%" + str(max_len) + "d") % item
        ans += line_now + "\n"
    return ans

def pd_code_diagram_sanity(pd_code: list[list[int]], *args, **kwargs) -> tuple[bool, list[list[int]]]:
    pd_code = sorted(json.loads(json.dumps(pd_code)))

    def _add_arc(nxt: dict[int, set[int]], a: int, b: int):
        if a not in nxt:
            nxt[a] = set()
        nxt[a].add(b)
        nxt[a] = set(sorted(nxt[a]))

    def _add_edge(nxt: dict[int, set[int]], a: int, b: int):
        _add_arc(nxt, a, b)
        _add_arc(nxt, b, a)

    nxt = {}
    for crossing in pd_code:
        _add_edge(nxt, crossing[0], crossing[2])
        _add_edge(nxt, crossing[1], crossing[3])

    node_set_dict = {}

    def _dfs(node: int, nxt: dict[int, set[int]], root: int, vis: dict[int, int]):
        vis[node] = root
        node_set_dict[root].add(node)
        for nxt_node in sorted(nxt[node]):
            if nxt_node in vis:
                continue
            _dfs(nxt_node, nxt, root, vis)

    vis = {}
    for node in sorted(nxt):
        if node in vis:
            continue
        node_set_dict[node] = set()
        _dfs(node, nxt, node, vis)

    # 构建节点编号列表
    node_list_dict = {
        root: sorted(list(node_set_dict[root]))
        for root in node_set_dict
    }

    # 计算节点真正的后继节点
    real_nxt = {}
    for _, nodes in node_list_dict.items():
        for i, node in enumerate(nodes):
            nxt_node = nodes[(i + 1) % len(nodes)]
            real_nxt[node] = nxt_node

    # 至少保证确实是后继
    for crossing in pd_code:
        a, b, c, d = crossing
        if real_nxt[a] != c and real_nxt[c] != a:
            return False, []
    
    # 构建新顺序
    for i in range(len(pd_code)):
        if real_nxt[pd_code[i][0]] != pd_code[i][2]:
            pd_code[i] = pd_code[i][2:4] + pd_code[i][0:2] # 循环左移两位
            assert real_nxt[pd_code[i][0]] == pd_code[i][2]

    diagram = get_diagram_from_pd_code(pd_code, *args, **kwargs)
    new_pd_code = sorted(diagram_to_pd_code(diagram))
    return (pd_code == new_pd_code), new_pd_code


if __name__ == "__main__":
    link = [[2, 9, 3, 10], [4, 11, 5, 12], [6, 7, 1, 8], [8, 5, 7, 6], [10, 1, 11, 2], [12, 3, 9, 4]]
    
    print(get_diagram_from_pd_code(link))

    with open("test.txt", "w") as fp:
        fp.write(get_diagram_str_from_pd_code(link))
