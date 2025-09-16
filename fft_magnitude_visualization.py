#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
FFT幅度数据可视化分析程序
用于分析ScanAdcPage.cpp中生成的fft_magnitude.txt文件
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.font_manager import FontProperties
import os

# 设置中文字体
plt.rcParams['font.sans-serif'] = ['SimHei']  # 用来正常显示中文标签
plt.rcParams['axes.unicode_minus'] = False  # 用来正常显示负号

def load_fft_magnitude(file_path):
    """
    加载FFT幅度数据
    :param file_path: FFT幅度数据文件路径
    :return: 幅度数据数组
    """
    try:
        with open(file_path, 'r') as f:
            data = [float(line.strip()) for line in f.readlines()]
        return np.array(data)
    except Exception as e:
        print(f"加载文件失败: {e}")
        return None

def analyze_fft_data(magnitude_data, sample_rate=1000, fft_length=2048):
    """
    分析FFT数据
    :param magnitude_data: FFT幅度数据
    :param sample_rate: 采样率
    :param fft_length: FFT长度
    :return: 分析结果字典
    """
    # 计算频率分辨率
    freq_resolution = sample_rate / fft_length
    
    # 生成频率轴（只到奈奎斯特频率）
    freq_axis = np.arange(fft_length // 2) * freq_resolution
    
    # 只取前半部分（对称性）
    magnitude_half = magnitude_data[:fft_length // 2]
    
    # 计算平均功率（1-50Hz范围）
    start_idx = int(1 / freq_resolution)
    end_idx = int(50 / freq_resolution)
    avg_power = np.mean(magnitude_half[start_idx:end_idx])
    
    # 计算阈值
    threshold = avg_power * 4
    
    # 寻找峰值（1-50Hz范围内）
    peaks = []
    for i in range(start_idx + 1, end_idx - 1):
        if (magnitude_half[i] > threshold and 
            magnitude_half[i] > magnitude_half[i-1] and 
            magnitude_half[i] > magnitude_half[i+1]):
            # 使用二次抛物线插值法提高频率估计精度
            y1 = magnitude_half[i-1]
            y2 = magnitude_half[i]
            y3 = magnitude_half[i+1]
            
            # 计算插值偏移量
            denominator = 2 * (2*y2 - y1 - y3)
            if abs(denominator) > 1e-10:  # 避免除零
                delta = (y3 - y1) / denominator
                # 限制偏移量在[-0.5, 0.5]范围内
                delta = max(-0.5, min(0.5, delta))
                
                # 计算插值后的频率
                freq = (i + delta) * freq_resolution
                peaks.append((freq, magnitude_half[i], delta))
    
    # 按幅度排序
    peaks.sort(key=lambda x: x[1], reverse=True)
    
    return {
        'freq_axis': freq_axis,
        'magnitude_half': magnitude_half,
        'avg_power': avg_power,
        'threshold': threshold,
        'peaks': peaks,
        'freq_resolution': freq_resolution
    }

def plot_fft_analysis(results, save_path=None):
    """
    绘制FFT分析结果
    :param results: 分析结果字典
    :param save_path: 图片保存路径
    """
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
    
    # 绘制完整频谱
    ax1.plot(results['freq_axis'], results['magnitude_half'], 'b-', linewidth=1)
    ax1.set_title('FFT频谱分析', fontsize=14)
    ax1.set_xlabel('频率 (Hz)', fontsize=12)
    ax1.set_ylabel('幅度', fontsize=12)
    ax1.grid(True, alpha=0.3)
    
    # 绘制阈值线
    threshold_line = np.ones_like(results['freq_axis']) * results['threshold']
    ax1.plot(results['freq_axis'], threshold_line, 'r--', alpha=0.7, label=f'阈值: {results["threshold"]:.2f}')
    
    # 标记峰值点
    for i, (freq, magnitude, delta) in enumerate(results['peaks'][:5]):  # 只显示前5个峰值
        ax1.plot(freq, magnitude, 'ro', markersize=8)
        ax1.annotate(f'{freq:.2f}Hz', 
                    xy=(freq, magnitude),
                    xytext=(freq, magnitude * 1.1),
                    arrowprops=dict(facecolor='black', shrink=0.05, width=1, headwidth=8),
                    ha='center')
    
    ax1.legend()
    ax1.set_xlim(0, 100)  # 限制显示0-100Hz范围
    
    # 绘制1-50Hz范围内的详细频谱
    mask = (results['freq_axis'] >= 1) & (results['freq_axis'] <= 50)
    ax2.plot(results['freq_axis'][mask], results['magnitude_half'][mask], 'g-', linewidth=1.5)
    ax2.set_title('1-50Hz范围内的详细频谱', fontsize=14)
    ax2.set_xlabel('频率 (Hz)', fontsize=12)
    ax2.set_ylabel('幅度', fontsize=12)
    ax2.grid(True, alpha=0.3)
    
    # 绘制阈值线
    ax2.plot(results['freq_axis'][mask], threshold_line[mask], 'r--', alpha=0.7, label=f'阈值: {results["threshold"]:.2f}')
    
    # 标记峰值点
    for i, (freq, magnitude, delta) in enumerate(results['peaks'][:5]):
        if 1 <= freq <= 50:
            ax2.plot(freq, magnitude, 'ro', markersize=8)
            ax2.annotate(f'{freq:.2f}Hz (δ={delta:.3f})', 
                        xy=(freq, magnitude),
                        xytext=(freq, magnitude * 1.1),
                        arrowprops=dict(facecolor='black', shrink=0.05, width=1, headwidth=8),
                        ha='center')
    
    ax2.legend()
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"分析结果已保存到: {save_path}")
    
    plt.show()

def save_analysis_results(results, file_path):
    """
    保存分析结果到文件
    :param results: 分析结果字典
    :param file_path: 保存文件路径
    """
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write("FFT幅度数据分析结果\n")
        f.write("=" * 50 + "\n\n")
        
        f.write(f"采样率: 1000 Hz\n")
        f.write(f"FFT长度: 2048\n")
        f.write(f"频率分辨率: {results['freq_resolution']:.6f} Hz\n")
        f.write(f"平均功率: {results['avg_power']:.2f}\n")
        f.write(f"检测阈值: {results['threshold']:.2f}\n\n")
        
        f.write("检测到的峰值频率（前5个）:\n")
        f.write("-" * 50 + "\n")
        for i, (freq, magnitude, delta) in enumerate(results['peaks'][:5]):
            f.write(f"{i+1}. 频率: {freq:.6f} Hz, 幅度: {magnitude:.2f}, 插值偏移量: {delta:.6f}\n")
        
        f.write("\n频率-幅度数据（1-50Hz范围）:\n")
        f.write("-" * 50 + "\n")
        f.write("频率(Hz)\t幅度\n")
        
        mask = (results['freq_axis'] >= 1) & (results['freq_axis'] <= 50)
        for freq, magnitude in zip(results['freq_axis'][mask], results['magnitude_half'][mask]):
            f.write(f"{freq:.2f}\t{magnitude:.6f}\n")
    
    print(f"分析结果已保存到: {file_path}")

def main():
    # 文件路径
    data_file = "d:/GOOD/StroboScope/LampControl2.0/EWARM/fft_magnitude.txt"
    output_image = "d:/GOOD/StroboScope/LampControl2.0/fft_magnitude_analysis.png"
    output_file = "d:/GOOD/StroboScope/LampControl2.0/fft_magnitude_analysis_result.txt"
    
    # 加载数据
    print("加载FFT幅度数据...")
    magnitude_data = load_fft_magnitude(data_file)
    if magnitude_data is None:
        return
    
    # 分析数据
    print("分析FFT数据...")
    results = analyze_fft_data(magnitude_data)
    
    # 打印主要结果
    print("\n分析结果:")
    print(f"频率分辨率: {results['freq_resolution']:.6f} Hz")
    print(f"平均功率: {results['avg_power']:.2f}")
    print(f"检测阈值: {results['threshold']:.2f}")
    print("\n检测到的峰值频率（前5个）:")
    for i, (freq, magnitude, delta) in enumerate(results['peaks'][:5]):
        print(f"{i+1}. 频率: {freq:.6f} Hz, 幅度: {magnitude:.2f}, 插值偏移量: {delta:.6f}")
    
    # 绘制分析结果
    print("\n绘制分析结果...")
    plot_fft_analysis(results, output_image)
    
    # 保存分析结果
    print("保存分析结果...")
    save_analysis_results(results, output_file)
    
    print("\n分析完成！")

if __name__ == "__main__":
    main()