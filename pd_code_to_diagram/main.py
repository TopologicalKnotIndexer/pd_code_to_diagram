import cpp_simple_interface
import pd_code_sanity
import os
from typing import Optional

try:
    from .run_file import run_program_with_input
except:
    from run_file import run_program_with_input

DIRNOW = os.path.dirname(os.path.abspath((__file__)))
CPP_MAIN = os.path.join(DIRNOW, "cpp_src", "main.cpp")
BIN_FOLDER = os.path.join(DIRNOW, "bin")
EXE_FILE = os.path.join(BIN_FOLDER, "a.exe")

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

if __name__ == "__main__":
    L10a1 = [[6, 1, 7, 2], [14, 7, 15, 8], [4, 15, 1, 16], [10, 6, 11, 5], [8, 4, 9, 3], [18, 11, 19, 12], [20, 17, 5, 18], [12, 19, 13, 20], [16, 10, 17, 9], [2, 14, 3, 13]]
    
    with open("test.txt", "w") as fp:
        for line in get_diagram_from_pd_code(L10a1, border_val=1):
            for term in line:
                if term != 0:
                    fp.write(f"{term:4d}")
                else:
                    fp.write("    ")
            fp.write("\n")
