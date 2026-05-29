import os
import sys

def count_lines_in_file(filepath):
    """统计单个文件的代码行数"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            lines = f.readlines()
        
        code_lines = 0
        comment_lines = 0
        blank_lines = 0
        
        in_block_comment = False
        
        for line in lines:
            stripped_line = line.strip()
            
            # 统计空行
            if not stripped_line:
                blank_lines += 1
                continue
            
            # 复制当前行用于分析
            temp_line = stripped_line
            i = 0
            in_string = False
            string_char = None  # 记录当前字符串类型 ('"' 或 "'")
            is_code_line = False
            is_comment_line = False
            
            # 遍历行字符以检测注释和字符串
            while i < len(temp_line):
                char = temp_line[i]
                
                # 如果在字符串中，跳过特殊字符处理直到字符串结束
                if in_string:
                    if char == string_char and (i == 0 or temp_line[i-1] != '\\' or (i >= 2 and temp_line[i-2:i] == '\\\\')):
                        in_string = False
                        string_char = None
                else:
                    # 检查是否开始字符串
                    if char in ('"', "'"):
                        in_string = True
                        string_char = char
                    # 检查是否是单行注释开始
                    elif char == '/' and i + 1 < len(temp_line) and temp_line[i+1] == '/':
                        # 剩余部分都是注释
                        remaining_part = temp_line[i:].strip()
                        if remaining_part:  # 如果注释部分非空
                            is_comment_line = True
                        break
                    # 检查是否是块注释开始
                    elif char == '/' and i + 1 < len(temp_line) and temp_line[i+1] == '*':
                        in_block_comment = True
                        # 检查是否在同一行结束
                        block_end_pos = temp_line.find('*/', i+2)
                        if block_end_pos != -1:
                            # 同一行内块注释结束
                            i = block_end_pos + 1
                            in_block_comment = False
                            continue
                        else:
                            # 块注释延续到后续行
                            is_comment_line = True
                            break
                    # 检查是否是块注释结束
                    elif char == '*' and i + 1 < len(temp_line) and temp_line[i+1] == '/':
                        if in_block_comment:
                            in_block_comment = False
                            i += 1  # 跳过 '/'
                    
                    # 如果不是在字符串或注释中，则为代码字符
                    if not in_block_comment and not in_string and char not in (' ', '\t'):
                        is_code_line = True
                
                i += 1
            
            # 根据分析结果分类行
            if is_comment_line or in_block_comment:
                comment_lines += 1
            elif is_code_line:
                code_lines += 1
            else:
                # 如果既不是注释也不是代码，可能是只有字符串字面量的行
                # 这种情况通常应视为代码行
                code_lines += 1
        
        return code_lines, comment_lines, blank_lines
    except UnicodeDecodeError:
        # 尝试使用其他编码
        try:
            with open(filepath, 'r', encoding='gbk') as f:
                lines = f.readlines()
            
            code_lines = 0
            comment_lines = 0
            blank_lines = 0
            
            in_block_comment = False
            
            for line in lines:
                stripped_line = line.strip()
                
                if not stripped_line:
                    blank_lines += 1
                    continue
                
                temp_line = stripped_line
                i = 0
                in_string = False
                string_char = None
                is_code_line = False
                is_comment_line = False
                
                while i < len(temp_line):
                    char = temp_line[i]
                    
                    if in_string:
                        if char == string_char and (i == 0 or temp_line[i-1] != '\\' or (i >= 2 and temp_line[i-2:i] == '\\\\')):
                            in_string = False
                            string_char = None
                    else:
                        if char in ('"', "'"):
                            in_string = True
                            string_char = char
                        elif char == '/' and i + 1 < len(temp_line) and temp_line[i+1] == '/':
                            remaining_part = temp_line[i:].strip()
                            if remaining_part:
                                is_comment_line = True
                            break
                        elif char == '/' and i + 1 < len(temp_line) and temp_line[i+1] == '*':
                            in_block_comment = True
                            block_end_pos = temp_line.find('*/', i+2)
                            if block_end_pos != -1:
                                i = block_end_pos + 1
                                in_block_comment = False
                                continue
                            else:
                                is_comment_line = True
                                break
                        elif char == '*' and i + 1 < len(temp_line) and temp_line[i+1] == '/':
                            if in_block_comment:
                                in_block_comment = False
                        if not in_block_comment and not in_string and char not in (' ', '\t'):
                            is_code_line = True
                    
                    i += 1
                
                if is_comment_line or in_block_comment:
                    comment_lines += 1
                elif is_code_line:
                    code_lines += 1
                else:
                    code_lines += 1
        
            return code_lines, comment_lines, blank_lines
        except Exception as e:
            print(f"Error reading file {filepath}: {e}")
            return 0, 0, 0
    except Exception as e:
        print(f"Error reading file {filepath}: {e}")
        return 0, 0, 0

def count_code_lines(root_dir):
    """统计指定目录下所有 .h .cpp .hpp 文件的代码行数"""
    total_code_lines = 0
    total_comment_lines = 0
    total_blank_lines = 0
    
    file_extensions = ['.h', '.cpp', '.hpp']
    
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if any(file.lower().endswith(ext) for ext in file_extensions):
                filepath = os.path.join(root, file)
                code, comment, blank = count_lines_in_file(filepath)
                total_code_lines += code
                total_comment_lines += comment
                total_blank_lines += blank
                
                print(f"{filepath}: {code} code lines, {comment} comment lines, {blank} blank lines")
    
    print(f"\nTotal: {total_code_lines} code lines, {total_comment_lines} comment lines, {total_blank_lines} blank lines")
    return total_code_lines, total_comment_lines, total_blank_lines

if __name__ == "__main__":
    if len(sys.argv) > 1:
        root_directory = sys.argv[1]
    else:
        root_directory = "."
    
    count_code_lines(root_directory)
