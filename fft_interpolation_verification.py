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

def analyze_fft_with_interpolation(file_path, sampling_rate=1000):
    """
    分析FFT输入数据并使用插值法计算更精确的频率
    
    参数:
        file_path: FFT输入数据文件路径
        sampling_rate: 采样率(Hz)，默认为1000Hz
        
    返回:
        dominant_freq: 主频率(Hz)
        interpolated_freq: 插值后的主频率(Hz)
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
    
    # 使用二次抛物线插值法提高频率估计精度
    if dominant_idx > 0 and dominant_idx < len(magnitude) - 1:
        # 获取峰值点及其左右两个点的幅度
        y1 = magnitude[dominant_idx - 1]
        y2 = magnitude[dominant_idx]
        y3 = magnitude[dominant_idx + 1]
        
        # 计算插值偏移量
        # 使用二次抛物线插值公式: delta = (y3 - y1) / (2 * (2*y2 - y1 - y3))
        delta = (y3 - y1) / (2 * (2*y2 - y1 - y3))
        
        # 限制偏移量在[-0.5, 0.5]范围内，确保插值结果在相邻两个频率点之间
        if delta > 0.5: delta = 0.5
        if delta < -0.5: delta = -0.5
        
        # 计算插值后的频率
        interpolated_freq = (dominant_idx + delta) * (sampling_rate / n)
    else:
        interpolated_freq = dominant_freq
        delta = 0
    
    # 获取前几个主要频率
    top_indices = np.argsort(magnitude[1:])[::-1][:5] + 1  # 获取前5个最大幅度的索引
    top_frequencies = [(xf[i], magnitude[i]) for i in top_indices]
    
    return dominant_freq, interpolated_freq, xf[:n//2], magnitude, top_frequencies, delta

def plot_frequency_spectrum_with_interpolation(frequencies, magnitude, dominant_freq, interpolated_freq, delta):
    """绘制频率谱并标记主频率和插值后的频率"""
    plt.figure(figsize=(12, 6))
    plt.plot(frequencies, magnitude)
    plt.title('Frequency Spectrum with Interpolation')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude')
    plt.grid(True)
    
    # 标记原始主频率
    plt.axvline(x=dominant_freq, color='r', linestyle='--', 
                label=f'Original Peak: {dominant_freq:.2f} Hz')
    
    # 标记插值后的主频率
    plt.axvline(x=interpolated_freq, color='g', linestyle='-', 
                label=f'Interpolated Peak: {interpolated_freq:.2f} Hz (delta={delta:.3f})')
    
    # 放大主频率附近的区域
    zoom_freq_min = max(0, dominant_freq - 2)
    zoom_freq_max = min(frequencies[-1], dominant_freq + 2)
    plt.xlim(zoom_freq_min, zoom_freq_max)
    
    plt.legend()
    
    plt.tight_layout()
    plt.show()

def main():
    # 设置中文字体
    setup_chinese_font()
    
    # 数据文件路径
    file_path = "d:/GOOD/StroboScope/LampControl2.0/EWARM/fft_input_data.txt"
    
    # 采样率 - 根据实际情况调整
    sampling_rate = 1000  # 默认采样率，可能需要根据实际情况修改
    
    print(f"Analyzing FFT data with sampling rate: {sampling_rate} Hz")
    print("Using quadratic parabolic interpolation for improved frequency estimation\n")
    
    # 分析数据
    dominant_freq, interpolated_freq, frequencies, magnitude, top_frequencies, delta = analyze_fft_with_interpolation(file_path, sampling_rate)
    
    # 输出结果到控制台和文件
    print(f"Original dominant frequency: {dominant_freq:.6f} Hz")
    print(f"Interpolated frequency: {interpolated_freq:.6f} Hz")
    print(f"Frequency difference: {abs(interpolated_freq - dominant_freq):.6f} Hz")
    print(f"Interpolation delta: {delta:.6f}")
    print(f"Improvement: {abs(interpolated_freq - dominant_freq) / dominant_freq * 100:.4f}%")
    
    print("\nTop 5 frequencies:")
    for i, (freq, mag) in enumerate(top_frequencies):
        print(f"  {i+1}. {freq:.2f} Hz (Magnitude: {mag:.2f})")
    
    # 将结果保存到文件
    with open("d:/GOOD/StroboScope/LampControl2.0/fft_interpolation_result.txt", "w") as f:
        f.write(f"Original dominant frequency: {dominant_freq:.6f} Hz\n")
        f.write(f"Interpolated frequency: {interpolated_freq:.6f} Hz\n")
        f.write(f"Frequency difference: {abs(interpolated_freq - dominant_freq):.6f} Hz\n")
        f.write(f"Interpolation delta: {delta:.6f}\n")
        f.write(f"Improvement: {abs(interpolated_freq - dominant_freq) / dominant_freq * 100:.4f}%\n")
        f.write("\nTop 5 frequencies:\n")
        for i, (freq, mag) in enumerate(top_frequencies):
            f.write(f"  {i+1}. {freq:.2f} Hz (Magnitude: {mag:.2f})\n")
        f.write("\nFrequency(Hz)\tMagnitude\n")
        for freq, mag in zip(frequencies, magnitude):
            f.write(f"{freq:.2f}\t{mag:.6f}\n")
    
    # 绘制频率谱
    try:
        plot_frequency_spectrum_with_interpolation(frequencies, magnitude, dominant_freq, interpolated_freq, delta)
    except Exception as e:
        print(f"Error plotting frequency spectrum: {e}")
    
    print("\nAnalysis complete. Results saved to fft_interpolation_result.txt")

if __name__ == "__main__":
    main()