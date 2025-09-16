import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft, fftfreq
import matplotlib.font_manager as fm

def setup_chinese_font():
    """设置中文字体"""
    # 尝试使用系统中可用的中文字体
    chinese_fonts = ['SimHei', 'Microsoft YaHei', 'STSong', 'SimSun', 'Arial Unicode MS']
    
    for font in chinese_fonts:
        try:
            fm.FontProperties(fname=font)
            plt.rcParams['font.sans-serif'] = [font]
            plt.rcParams['axes.unicode_minus'] = False
            return True
        except:
            continue
    
    # 如果没有找到中文字体，使用默认字体
    print("Warning: No Chinese font found, using default font.")
    return False

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
        top_frequencies: 前几个主要频率及其幅度
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
    
    # 获取前几个主要频率
    top_indices = np.argsort(magnitude[1:])[::-1][:5] + 1  # 获取前5个最大幅度的索引
    top_frequencies = [(xf[i], magnitude[i]) for i in top_indices]
    
    return dominant_freq, xf[:n//2], magnitude, top_frequencies

def plot_frequency_spectrum(frequencies, magnitude, dominant_freq, top_frequencies):
    """绘制频率谱并标记主频率"""
    plt.figure(figsize=(12, 6))
    plt.plot(frequencies, magnitude)
    plt.title('Frequency Spectrum')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude')
    plt.grid(True)
    
    # 标记主频率
    plt.axvline(x=dominant_freq, color='r', linestyle='--', 
                label=f'Dominant Frequency: {dominant_freq:.2f} Hz')
    
    # 标记前几个主要频率
    for i, (freq, mag) in enumerate(top_frequencies):
        plt.scatter(freq, mag, color=['red', 'green', 'blue', 'orange', 'purple'][i], 
                   label=f'Peak {i+1}: {freq:.2f} Hz')
    
    plt.legend()
    
    plt.tight_layout()
    plt.show()

def plot_time_domain(signal, sampling_rate):
    """绘制时域信号"""
    time = np.arange(len(signal)) / sampling_rate
    plt.figure(figsize=(12, 4))
    plt.plot(time, signal)
    plt.title('Time Domain Signal')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.tight_layout()
    plt.show()

def main():
    # 设置中文字体
    setup_chinese_font()
    
    # 数据文件路径
    file_path = "d:/GOOD/StroboScope/LampControl2.0/EWARM/fft_input_data.txt"
    
    # 采样率 - 根据实际情况调整
    # 注意：这里需要根据实际系统的采样率进行调整
    sampling_rate = 1000  # 默认采样率，可能需要根据实际情况修改
    
    print(f"Analyzing FFT data with sampling rate: {sampling_rate} Hz")
    
    # 分析数据
    dominant_freq, frequencies, magnitude, top_frequencies = analyze_fft_data(file_path, sampling_rate)
    
    # 输出结果到控制台和文件
    result = f"Dominant Frequency: {dominant_freq:.2f} Hz"
    print(result)
    
    print("\nTop 5 frequencies:")
    for i, (freq, mag) in enumerate(top_frequencies):
        print(f"  {i+1}. {freq:.2f} Hz (Magnitude: {mag:.2f})")
    
    # 将结果保存到文件
    with open("d:/GOOD/StroboScope/LampControl2.0/fft_analysis_result.txt", "w") as f:
        f.write(result + "\n")
        f.write(f"Sampling Rate: {sampling_rate} Hz\n")
        f.write("Top 5 frequencies:\n")
        for i, (freq, mag) in enumerate(top_frequencies):
            f.write(f"  {i+1}. {freq:.2f} Hz (Magnitude: {mag:.2f})\n")
        f.write("\nFrequency(Hz)\tMagnitude\n")
        for freq, mag in zip(frequencies, magnitude):
            f.write(f"{freq:.2f}\t{mag:.6f}\n")
    
    # 读取原始数据用于时域分析
    with open(file_path, 'r') as file:
        data = [float(line.strip()) for line in file.readlines()]
    real_data = data[::2]
    
    # 绘制时域信号
    try:
        plot_time_domain(real_data, sampling_rate)
    except Exception as e:
        print(f"Error plotting time domain signal: {e}")
    
    # 绘制频率谱
    try:
        plot_frequency_spectrum(frequencies, magnitude, dominant_freq, top_frequencies)
    except Exception as e:
        print(f"Error plotting frequency spectrum: {e}")
    
    print("\nAnalysis complete. Results saved to fft_analysis_result.txt")

if __name__ == "__main__":
    main()