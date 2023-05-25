from ulab import numpy as np  # to get access to ulab numpy functions
#import numpy as np

freq1 = 2 * np.pi
freq2 = 7 * np.pi
freq3 = 10 * np.pi

sinSumArr = [np.sin(freq1 * x) + np.sin(freq2 * x) + np.sin(freq3 * x)
             for x in range(0, 1024)]

sinSumArr = np.fft.fft(np.array(sinSumArr))


for item in sinSumArr[0]:
    print(item)
