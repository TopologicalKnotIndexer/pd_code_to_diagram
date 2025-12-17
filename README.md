# PD_CODE_TO_DIAGRAM

给定一个合法的 PD_CODE，程序将输出一个扭结的放置方案，最终将扭结放置到一个网格图中。

## 编译项目
```bash
g++ -std=c++17 -g -o pdcode_to_diagram.exe src/main.cpp 
```

## 运行项目

先把一个扭结或者链环的 PD_CODE 存储到指定文件中（例如 `pdcode.txt`），下面给出了一个 PD_CODE 示例：
```
[[6, 1, 7, 2], [8, 3, 5, 4], [2, 5, 3, 6], [4, 7, 1, 8]]
```

然后运行编译得到的程序。

1. 不带参数运行可以让程序输出三维空间坐标表示的扭结信息。
```bash
pdcode_to_diagram.exe <pdcode.txt
```

2. 带 `--diagram` 或者 `-d` 参数运行可以让程序输出一个直观的扭结在二维空间中的布局图。
```bash
pdcode_to_diagram.exe --diagram <pdcode.txt
```

## 程序原理

现在底图上跑一个最大边数生成森林（森林即生成树的不交并），保证森林中树边均满足折线段段数为一，再进行后续边连接。
