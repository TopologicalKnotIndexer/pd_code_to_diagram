import os
import subprocess
DIRNOW = os.path.dirname(os.path.abspath(__file__))

PD_CODE      = os.path.join(DIRNOW, "pd_code")
COORD3D      = os.path.join(DIRNOW, "coord3d")
DIAGRAM      = os.path.join(DIRNOW, "diagram")
DIAGRAM_ZERO = os.path.join(DIRNOW, "diagram_0")
assert os.path.isdir(PD_CODE)
assert os.path.isdir(COORD3D)

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
            shell=True,  # 关键：兼容 Windows 的 cmd 命令（如 dir）和 Linux 的 shell 命令
            encoding=encoding,
            **redirect_kwargs
        )
        return result
    finally:
        # 确保文件句柄关闭（避免泄漏）
        for f in redirect_kwargs.values():
            f.close()

# 输出所有三维结构
def create_all_coord3d():
    for filename in os.listdir(PD_CODE):
        print(filename)
        inp_filepath  = os.path.join(PD_CODE, filename)
        out_filepath = os.path.join(COORD3D, filename)
        run_with_redirect([os.path.join(DIRNOW, "..", "bin", "a.exe"), "-s"], inp_filepath, out_filepath)

# 输出所有二维图
def create_all_diagram():
    for filename in os.listdir(PD_CODE):
        print(filename)
        inp_filepath  = os.path.join(PD_CODE, filename)
        out_filepath = os.path.join(DIAGRAM, filename)
        run_with_redirect([os.path.join(DIRNOW, "..", "bin", "a.exe"), "-d"], inp_filepath, out_filepath)

# 输出所有带 0 二维图
def create_all_diagram_zero():
    for filename in os.listdir(PD_CODE):
        print(filename)
        inp_filepath  = os.path.join(PD_CODE, filename)
        out_filepath = os.path.join(DIAGRAM_ZERO, filename)
        run_with_redirect([os.path.join(DIRNOW, "..", "bin", "a.exe"), "-d", "-z"], inp_filepath, out_filepath)

if __name__ == "__main__":
    create_all_diagram_zero()
