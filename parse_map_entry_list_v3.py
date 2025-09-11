#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
程序功能：从LampControl.map文件中提取ENTRY LIST下的所有变量和函数，按size大小排序，并生成到一个新的txt文件中
"""

import re
import os

def parse_entry_list(map_file_path, output_file_path):
    """
    解析map文件中的ENTRY LIST部分，提取变量和函数信息，按大小排序后输出到文件
    
    Args:
        map_file_path: map文件路径
        output_file_path: 输出文件路径
    """
    # 读取map文件
    with open(map_file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    # 找到ENTRY LIST开始位置
    start_idx = -1
    for i, line in enumerate(lines):
        if '*** ENTRY LIST' in line:
            start_idx = i + 3  # 跳过标题行
            break
    
    if start_idx == -1:
        print("未找到ENTRY LIST部分")
        return
    
    # 找到ENTRY LIST结束位置
    end_idx = len(lines)
    for i in range(start_idx, len(lines)):
        if lines[i].startswith('***') and i > start_idx:
            end_idx = i
            break
    
    # 解析条目
    entries = []
    i = start_idx
    while i < end_idx:
        line = lines[i].rstrip()
        if not line.strip():
            i += 1
            continue
            
        # 处理可能跨行的情况
        if line.startswith('                         ') and entries:
            # 这是上一行的延续
            entries[-1]['name'] += ' ' + line.strip()
            i += 1
            continue
        
        # 使用正则表达式匹配行
        # 示例行: ADC_ConversionStop       0x800'8153    0xe0  Code  Gb  stm32g4xx_hal_adc.o [5]
        # 示例行: ?main                    0x801'12b5          Code  Gb  cmain.o [12]
        parts = re.split(r'\s{2,}', line.strip())
        if len(parts) >= 4:
            name = parts[0]
            address = parts[1]
            
            # 判断是否有size字段
            if len(parts) >= 5 and re.match(r'^0x[0-9a-fA-F]+$', parts[2].replace("'", "")):
                size = parts[2].replace("'", "")
                type1 = parts[3]
                type2 = parts[4] if len(parts) > 4 else ""
                object_info = ' '.join(parts[5:]) if len(parts) > 5 else ""
            else:
                size = '0'
                type1 = parts[2]
                type2 = parts[3] if len(parts) > 3 else ""
                object_info = ' '.join(parts[4:]) if len(parts) > 4 else ""
                
            type_ = f"{type1} {type2}".strip()
            
            # 将十六进制size转换为十进制
            try:
                if size.startswith('0x'):
                    size_int = int(size, 16)
                else:
                    size_int = int(size, 16) if size else 0
            except:
                size_int = 0
                
            entries.append({
                'name': name,
                'address': address,
                'size': size,
                'size_int': size_int,
                'type': type_,
                'object': object_info
            })
            
            print(f"解析到条目: name={name}, size={size_int}, type={type_}")
        else:
            print(f"跳过无法解析的行: {line}")
            
        i += 1
    
    # 过滤出地址为RAM（0x2开头）的条目
    ram_entries = [entry for entry in entries if entry['address'].startswith('0x2')]
    
    # 按size_int降序排序
    ram_entries.sort(key=lambda x: x['size_int'], reverse=True)
    
    # 写入输出文件
    with open(output_file_path, 'w', encoding='utf-8') as f:
        f.write("ENTRY LIST 中的RAM变量和函数（按大小排序）\n")
        f.write("=" * 80 + "\n")
        f.write(f"{'序号':<5} {'名称':<40} {'地址':<15} {'大小(字节)':<12} {'类型':<8} {'对象'}\n")
        f.write("-" * 80 + "\n")
        
        for i, entry in enumerate(ram_entries, 1):
            f.write(f"{i:<5} {entry['name']:<40} {entry['address']:<15} {entry['size_int']:<12} {entry['type']:<8} {entry['object']}\n")
        
        # 添加统计信息
        total_size = sum(entry['size_int'] for entry in ram_entries)
        f.write("\n" + "=" * 80 + "\n")
        f.write(f"总计: {len(ram_entries)} 个条目, 总大小: {total_size} 字节\n")
        
        # 按类型统计
        type_stats = {}
        for entry in ram_entries:
            type_ = entry['type']
            if type_ not in type_stats:
                type_stats[type_] = {'count': 0, 'size': 0}
            type_stats[type_]['count'] += 1
            type_stats[type_]['size'] += entry['size_int']
            
        f.write("\n按类型统计:\n")
        for type_, stats in type_stats.items():
            f.write(f"{type_}: {stats['count']} 个条目, 总大小: {stats['size']} 字节\n")
    
    print(f"处理完成，结果已保存到: {output_file_path}")

if __name__ == "__main__":
    # 设置输入和输出文件路径
    map_file_path = r"d:\GOOD\StroboScope\LampControl2.0\EWARM\LampControl\List\LampControl.map"
    output_file_path = r"d:\GOOD\StroboScope\LampControl2.0\EWARM\LampControl\List\entry_list_sorted.txt"
    
    # 确保输出目录存在
    os.makedirs(os.path.dirname(output_file_path), exist_ok=True)
    
    # 解析并生成排序后的条目列表
    parse_entry_list(map_file_path, output_file_path)