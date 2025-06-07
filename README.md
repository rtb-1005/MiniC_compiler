# MiniC编译器示例

该项目来源于课程作业，原始代码使用Visual Studio工程。现已整理为跨平台结构，并使用CMake构建。

## 构建方式

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

构建完成后会生成`minic`可执行文件，可用于分析 `input.minic` 示例。
