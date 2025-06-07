#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
构建并汇总错误信息的脚本。
该脚本运行 CMake 构建流程，将所有输出写入 output_error 文件，
在终端显示构建结果。
"""

import os
import subprocess
import sys

LOG_FILE = "output_error"
BUILD_DIR = "build"


def run_cmd(cmd, log):
    """运行命令并将输出写入日志文件"""
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    log.write(result.stdout)
    return result.returncode


def main():
    # 准备日志文件
    with open(LOG_FILE, "w") as log:
        if not os.path.isdir(BUILD_DIR):
            os.mkdir(BUILD_DIR)
        status = run_cmd(["cmake", "-S", ".", "-B", BUILD_DIR], log)
        if status == 0:
            status = run_cmd(["cmake", "--build", BUILD_DIR], log)

    # 收集包含 error 的行
    errors = []
    with open(LOG_FILE, "r") as log:
        for line in log:
            if "error" in line.lower():
                errors.append(line.strip())

    if status == 0:
        message = "Build success"
    else:
        reason = errors[0] if errors else "Unknown error"
        message = f"Build failed: {reason}"

    print(message)

    with open(LOG_FILE, "a") as log:
        log.write(message + "\n")

    # 输出日志内容到终端
    with open(LOG_FILE, "r") as log:
        sys.stdout.write(log.read())


if __name__ == "__main__":
    main()
