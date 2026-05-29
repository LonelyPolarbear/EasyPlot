import os
import chardet

# 这里填写你要转换的文件夹路径
TARGET_DIR = r"E:\work\aider" 
# 需要处理的文本文件后缀名，可自行添加
TEXT_EXTS = ('.txt', '.csv', '.json', '.xml', '.html', '.css', '.js', '.md', '.py', '.log','.h','.hpp','.cpp')

def smart_convert(file_path):
    with open(file_path, 'rb') as f:
        raw_data = f.read()
    
    # 1. 自动检测文件当前的编码
    result = chardet.detect(raw_data)
    current_encoding = result['encoding']
    
    # 如果检测不到编码（可能是纯二进制文件），直接跳过
    if not current_encoding:
        print(f"⚠️ 无法识别编码，已跳过: {file_path}")
        return

    # 2. 如果已经是 UTF-8 编码，直接跳过
    if current_encoding.lower() == 'utf-8':
        print(f"✅ 已经是 UTF-8，无需转换: {file_path}")
        return

    try:
        # 3. 用检测到的原编码（如 GBK）读取内容
        content = raw_data.decode(current_encoding, errors='ignore')
        
        # 4. 强制以 UTF-8 格式写回文件
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"🔄 成功将 {current_encoding} 转换为 UTF-8: {file_path}")
        
    except Exception as e:
        print(f"❌ 转换失败 {file_path}，错误信息: {e}")

# 遍历文件夹执行转换
for root, _, files in os.walk(TARGET_DIR):
    for file in files:
        if file.lower().endswith(TEXT_EXTS):
            file_path = os.path.join(root, file)
            smart_convert(file_path)

print("\n🎉 所有文件扫描转换完成！")