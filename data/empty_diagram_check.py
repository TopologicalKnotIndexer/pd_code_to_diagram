import os

def find_empty_txt_files(folder_path):
    """
    查找指定文件夹中所有空的.txt文件并输出文件名
    
    参数:
        folder_path: 要检查的文件夹路径（绝对路径/相对路径均可）
    """
    # 检查文件夹路径是否存在
    if not os.path.isdir(folder_path):
        print(f"错误：文件夹 '{folder_path}' 不存在！")
        return
    
    empty_txt_files = []
    
    # 遍历文件夹中的所有文件
    for filename in os.listdir(folder_path):
        # 拼接文件完整路径
        file_path = os.path.join(folder_path, filename)
        
        # 只处理文件（排除子文件夹）且是.txt后缀的文件
        if os.path.isfile(file_path) and filename.lower().endswith('.txt'):
            # 检查文件大小是否为0字节（空文件）
            if os.path.getsize(file_path) == 0:
                empty_txt_files.append(filename)
    
    # 输出结果
    if empty_txt_files:
        print(f"在文件夹 '{folder_path}' 中找到以下空的.txt文件：")
        for idx, file_name in enumerate(empty_txt_files, 1):
            print(f"{idx}. {file_name}")
    else:
        print(f"文件夹 '{folder_path}' 中没有找到空的.txt文件。")

# 主程序入口
if __name__ == "__main__":
    # 请在这里修改为你要检查的文件夹路径
    # 示例：Windows路径 "C:\\Users\\YourName\\Desktop\\test_folder"
    # 示例：Mac/Linux路径 "/Users/YourName/Desktop/test_folder"
    DIRNOW = os.path.dirname(os.path.abspath(__file__))
    target_folder = os.path.join(DIRNOW, "diagram_0")
    find_empty_txt_files(target_folder)
    