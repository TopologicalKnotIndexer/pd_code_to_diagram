# PD_CODE_TO_DIAGRAM

给定一个合法的 PD_CODE，程序将输出一个扭结的放置方案，最终将扭结放置到一个网格图中。

## 编译项目
```bash
g++ -std=c++17 -g -o pdcode_to_diagram.exe src/main.cpp 
```

## 运行项目

### 从文件读入扭结

先把一个扭结或者链环的 PD_CODE 存储到指定文件中（例如 `pdcode.txt`），下面给出了一个 PD_CODE 示例：
```
[[6, 1, 7, 2], [8, 3, 5, 4], [2, 5, 3, 6], [4, 7, 1, 8]]
```

然后运行编译得到的程序。

1. 不带参数运行可以让程序输出三维空间坐标表示的扭结信息。
```bash
pdcode_to_diagram.exe --serial <pdcode.txt
```

2. 带 `--diagram` 或者 `-d` 参数运行可以让程序输出一个直观的扭结在二维空间中的布局图。
```bash
pdcode_to_diagram.exe --diagram <pdcode.txt
```

3. 在带 `--diagram` 或者 `-d` 的基础上，使用 `--with_zero` 或者 `-z` 参数可以让布局图中使用 `0` 填补空位。
```bash
pdcode_to_diagram.exe --diagram --with_zero <pdcode.txt
```
注：在没有 `--diagram` 以及 `-d` 的前提下，使用 `--with_zero` 或者 `-z` 不会对程序产生影响。

4. 使用 `--border` 或者 `-b` 展示布局图中位于边界上的点。
```bash
pdcode_to_diagram.exe --border <pdcode.txt
```

### 从标准输入读入扭结

如果您试图从标准输入读入扭结，请注意您需要为标准输入引入 EOF，具体而言：

1. 对 Windows 类操作系统，在输入完全部数据后，你还需要连续输入 Enter, Ctrl+Z, Enter 三个按键。
2. 对类 Unix 操作系统（Linux, Mac OS 等等），在输入完全部数据后，你还需要连续输入 Enter, Ctrl+D, Enter 三个按键。

## 程序原理

现在底图上跑一个最大边数生成森林（森林即生成树的不交并），保证森林中树边均满足折线段段数为一，再进行后续边连接。
