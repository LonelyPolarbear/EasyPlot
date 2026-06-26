import os
import chardet

# ================= 可配置参数 =================
# 需要处理的文本文件后缀（不区分大小写）
TEXT_EXTS = ('.txt', '.csv', '.json', '.xml', '.html', '.css', '.js', '.md', '.py', '.log', '.h', '.hpp', '.cpp')
# 需要跳过的目录名（完全匹配），这些目录不会被遍历
SKIP_DIRS = {'.git', '__pycache__', 'venv', 'env', 'node_modules', '.idea', '.vscode','3rdParty','doxygen1.16.1','Python38','output'}
# 是否跳过脚本自身（避免误改正在运行的文件）
SKIP_SELF = True
# ============================================

# 当前脚本所在目录（绝对路径）
TARGET_DIR = os.path.dirname(os.path.abspath(__file__))

def smart_convert(file_path):
    """检测并转换文件为 UTF-8 编码"""
    with open(file_path, 'rb') as f:
        raw_data = f.read()

    # 1. 自动检测文件当前的编码
    result = chardet.detect(raw_data)
    current_encoding = result['encoding']

    # 如果检测不到编码，跳过
    if not current_encoding:
        print(f"⚠️ 无法识别编码，已跳过: {file_path}")
        return

    # 标准化编码名称（如 'UTF-8-SIG' 视为 'utf-8'）
    normalized_encoding = current_encoding.lower().replace('-sig', '').strip()
    if normalized_encoding == 'utf-8':
        print(f"✅ 已经是 UTF-8，无需转换: {file_path}")
        return

    try:
        # 2. 用检测到的原编码读取内容（忽略无法解码的字节）
        content = raw_data.decode(current_encoding, errors='ignore')
        # 3. 以 UTF-8 格式写回
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"🔄 成功将 {current_encoding} 转换为 UTF-8: {file_path}")
    except Exception as e:
        print(f"❌ 转换失败 {file_path}，错误信息: {e}")

# 遍历目录，跳过指定的子目录
for root, dirs, files in os.walk(TARGET_DIR):
    # 原地修改 dirs，剔除需要跳过的目录，这样 os.walk 就不会进入它们
    dirs[:] = [d for d in dirs if d not in SKIP_DIRS]

    for file in files:
        # 检查文件扩展名
        if not file.lower().endswith(TEXT_EXTS):
            continue

        file_path = os.path.join(root, file)

        # 可选：跳过脚本自身（如果脚本位于目标目录下且扩展名为 .py）
        if SKIP_SELF and os.path.abspath(file_path) == os.path.abspath(__file__):
            print(f"⏭️ 跳过脚本自身: {file_path}")
            continue

        smart_convert(file_path)

print("\n🎉 所有文件扫描转换完成！")