import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft, fftfreq

def analyze_fft_data(file_path, sampling_rate=1000):
    """
    分析FFT输入数据并计算最匹配的频率
    
    参数:
        file_path: FFT输入数据文件路径
        sampling_rate: 采样率(Hz)，默认为1000Hz
        
    返回:
        dominant_freq: 主频率(Hz)
        frequencies: 频率数组
        magnitude: 幅度谱
    """
    # 读取数据
    with open(file_path, 'r') as file:
        data = [float(line.strip()) for line in file.readlines()]
    
    # 从数据中提取实部（因为数据格式是实部、0、实部、0...）
    real_data = data[::2]  # 每隔一个元素取一个，即所有实部
    
    # 转换为numpy数组
    signal = np.array(real_data)
    
    # 执行FFT
    n = len(signal)
    yf = fft(signal)
    xf = fftfreq(n, 1/sampling_rate)
    
    # 计算幅度谱
    magnitude = 2.0/n * np.abs(yf[0:n//2])
    
    # 找到主频率（忽略直流分量）
    dominant_idx = np.argmax(magnitude[1:]) + 1  # 跳过直流分量(0Hz)
    dominant_freq = xf[dominant_idx]
    
    return dominant_freq, xf[:n//2], magnitude

def plot_frequency_spectrum(frequencies, magnitude, dominant_freq):
    """绘制频率谱并标记主频率"""
    plt.figure(figsize=(12, 6))
    plt.plot(frequencies, magnitude)
    plt.title('频率谱')
    plt.xlabel('频率 (Hz)')
    plt.ylabel('幅度')
    plt.grid(True)
    
    # 标记主频率
    plt.axvline(x=dominant_freq, color='r', linestyle='--', 
                label=f'主频率: {dominant_freq:.2f} Hz')
    plt.legend()
    
    plt.tight_layout()
    plt.show()

def main():
    # 数据文件路径
    file_path = "d:/GOOD/StroboScope/LampControl2.0/EWARM/fft_input_data.txt"
    
    # 采样率 - 根据实际情况调整
    # 注意：这里需要根据实际系统的采样率进行调整
    sampling_rate = 1000  # 默认采样率，可能需要根据实际情况修改
    
    # 分析数据
    dominant_freq, frequencies, magnitude = analyze_fft_data(file_path, sampling_rate)
    
    # 输出结果到控制台和文件
    result = f"最匹配的频率: {dominant_freq:.2f} Hz"
    print(result)
    
    # 将结果保存到文件
    with open("d:/GOOD/StroboScope/LampControl2.0/fft_analysis_result.txt", "w") as f:
        f.write(result + "\n")
        f.write(f"采样率: {sampling_rate} Hz\n")
        f.write("频率(Hz)\t幅度\n")
        for freq, mag in zip(frequencies, magnitude):
            f.write(f"{freq:.2f}\t{mag:.6f}\n")
    
    # 绘制频率谱
    try:
        plot_frequency_spectrum(frequencies, magnitude, dominant_freq)
    except Exception as e:
        print(f"绘图时出错: {e}")
        print("结果已保存到文件中")

if __name__ == "__main__":
    main()