# FFT数据分析程序

这个Python程序用于分析FFT输入数据并计算最匹配的频率。

## 功能

1. 从文本文件中读取FFT输入数据
2. 执行FFT分析并计算频率谱
3. 找出最匹配的频率（主频率）
4. 显示前5个主要频率及其幅度
5. 绘制时域信号和频率谱图
6. 将分析结果保存到文本文件

## 使用方法

1. 确保已安装必要的Python库：
   ```
   pip install numpy matplotlib scipy
   ```

2. 运行程序：
   ```
   python fft_analysis_enhanced.py
   ```

3. 程序将：
   - 分析`EWARM/fft_input_data.txt`文件中的数据
   - 在控制台输出分析结果
   - 显示时域信号和频率谱图
   - 将详细结果保存到`fft_analysis_result.txt`文件

## 输入数据格式

输入数据文件应包含每行一个浮点数，格式为实部、0、实部、0...交替排列。

例如：
```
221.000000
0.000000
217.000000
0.000000
228.000000
0.000000
...
```

## 输出结果

程序将输出：

1. 控制台输出：
   - 最匹配的频率（主频率）
   - 前5个主要频率及其幅度

2. 图形显示：
   - 时域信号图
   - 频率谱图，标记主频率和前5个主要频率

3. 文本文件（fft_analysis_result.txt）：
   - 最匹配的频率
   - 采样率
   - 前5个主要频率及其幅度
   - 完整的频率-幅度数据表

## 参数调整

在程序中，您可以调整以下参数：

- `sampling_rate`：采样率（Hz），默认为1000Hz
- `file_path`：输入数据文件路径
- 输出文件路径和名称

## 示例输出

```
Analyzing FFT data with sampling rate: 1000 Hz
Dominant Frequency: 6.35 Hz

Top 5 frequencies:
  1. 6.35 Hz (Magnitude: 544.36)
  2. 12.70 Hz (Magnitude: 457.95)
  3. 19.04 Hz (Magnitude: 369.63)
  4. 25.39 Hz (Magnitude: 309.77)
  5. 25.88 Hz (Magnitude: 288.28)

Analysis complete. Results saved to fft_analysis_result.txt
```

## 注意事项

1. 确保输入数据文件存在且格式正确
2. 根据实际系统的采样率调整程序中的`sampling_rate`参数
3. 如果中文字体显示有问题，程序会自动使用英文字体
4. 图形窗口可能需要手动关闭才能继续程序执行