import sys
import time

def is_windows_by_sys() -> bool:
    """
    通过 sys 模块判断当前系统是否为 Windows
    返回值：
        bool - True 表示是 Windows 系统，False 表示不是
    """
    # sys.platform 在 Windows 系统上返回 'win32'（无论 32/64 位）
    return sys.platform.startswith('win')

import os
import subprocess
DIRNOW = os.path.dirname(os.path.abspath(__file__))
A_EXE = os.path.join(DIRNOW, "..", "bin", "a.exe")

PD_CODE      = os.path.join(DIRNOW, "pd_code")
COORD3D      = os.path.join(DIRNOW, "coord3d")
DIAGRAM      = os.path.join(DIRNOW, "diagram")
DIAGRAM_ZERO = os.path.join(DIRNOW, "diagram_0")
DIAGRAM_TEST = os.path.join(DIRNOW, "diagram_test")

def run_with_redirect(cmd:list[str], stdin_path=None, stdout_path=None, encoding="utf-8"):
    """
    跨平台执行命令，重定向标准输入/输出到文件（支持 Windows/Linux）
    
    :param cmd: 待执行的命令（列表形式，如 ["echo", "hello"]）
    :param stdin_path: 标准输入文件路径（None 表示不重定向）
    :param stdout_path: 标准输出文件路径（None 表示不重定向）
    :param encoding: 文本编码（默认 utf-8，解决中文乱码）
    :return: subprocess.CompletedProcess 对象（包含返回码等信息）
    """
    # 构建重定向参数
    redirect_kwargs = {}
    # 重定向标准输入
    if stdin_path:
        redirect_kwargs["stdin"] = open(stdin_path, "r", encoding=encoding)
    # 重定向标准输出
    if stdout_path:
        redirect_kwargs["stdout"] = open(stdout_path, "w", encoding=encoding)
    
    try:
        # 执行命令（shell=True 适配 Windows 批处理命令，跨平台兼容）
        # cmd 传列表时，shell=True 不影响 Linux，仅适配 Windows
        result = subprocess.run(
            cmd,
            shell=not is_windows_by_sys(),  # 关键：兼容 Windows 的 cmd 命令（如 dir）和 Linux 的 shell 命令
            encoding=encoding,
            **redirect_kwargs
        )
        return result
    finally:
        # 确保文件句柄关闭（避免泄漏）
        for f in redirect_kwargs.values():
            f.close()

def crossing_num(knotname:str) -> int:
    return int(knotname.lower().split("l")[-1].split("a")[0].split("n")[0])

def knot_type(knotname:str) -> str:
    if knotname.find("a") != -1:
        return "a"
    else:
        return "n"

def knot_inner_index(knotname:str) -> int:
    if knotname.endswith(".txt"):
        knotname = knotname[:-4]
    return int(knotname.split("a")[-1].split("n")[-1])

def sorted_knot_name(knot_list:list[str]) -> list[str]:
    return sorted(knot_list, key=lambda knotname: (
        crossing_num(knotname), knot_type(knotname), knot_inner_index(knotname)))

def process_all_file(pre:list, perfile:list, after:list):
    for item in pre:
        item()
    
    knot_name_list = sorted_knot_name(list(os.listdir(PD_CODE)))
    for idx, filename in enumerate(knot_name_list):
        inp_filepath = os.path.join(PD_CODE, filename)
        for func in perfile:
            func(inp_filepath, idx, len(knot_name_list))
    for item in after:
        item()

def output_template(aim_dir:str, cmd:list[str]):
    begin_time = time.time()
    process_all_file([
        lambda: os.makedirs(aim_dir, exist_ok=True)
    ], [
        lambda x, idx, total: print(
            f"{time.time() - begin_time:7.3f}", 
            f"{idx + 1:4d}", "/", total, os.path.basename(x)),
        
        lambda x, idx, total: (run_with_redirect([A_EXE] + cmd, 
                                     x, os.path.join(aim_dir, os.path.basename(x))))
    ], [])

# 输出所有三维结构
def create_all_coord3d():
    output_template(COORD3D, ["-s"])

# 输出所有二维图
def create_all_diagram():
    output_template(DIAGRAM, ["-d"])

# 输出所有带 0 二维图
def create_all_diagram_zero():
    output_template(DIAGRAM_ZERO, ["-d", "-z"])

# 测试同一个 pd_code 是否能总保证任何连通分支都有暴露在外面的构型
def create_all_diagram_test():
    output_template(DIAGRAM_TEST, ["-t"])

if __name__ == "__main__":
    create_all_diagram_test()
