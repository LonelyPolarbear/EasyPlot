import xml.etree.ElementTree as ET
import importlib.util
import sys
import os

def parse_xml(xml_string):
    tree = ET.parse(xml_string)
    root = tree.getroot()
    print("Root tag:", root.tag)
    return root

def main():
    parse_xml("data.xml")
    return

#调用函数


sys.path.append(r"E:\work\boostPython\output\bin")
os.chdir(r"E:\work\boostPython\output\bin")

def load_module():
    module_name = 'swj'
    file_path = r'E:\work\boostPython\output\bin\myPy.dll'
    spec = importlib.util.spec_from_file_location(module_name, file_path)
    if not os.path.exists(file_path):
        print(f"文件不存在: {file_path}")
        return None
    
    if spec is None:
        print("无法创建模块规范")
        return None
    