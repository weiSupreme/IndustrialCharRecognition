## 基于opencv的工业现场液体饮料包装的字符（喷码）识别系统		

*created by zhuwei*			

*email: zxdzhuwei@foxmail.com(@hnu.edu.cn)*			

*blog: https://weiSupreme.github.io*			

*2018.08.18*			

开发环境：vs2013，opencv3.4		

&emsp;&emsp;目前利用opencv识别字符的项目和代码很多，但主要针对车牌识别。对于工业现场如易拉罐字符识别等，opencv的代码较少，halcon居多，而且能找到的代码都是处理背景非常简单的图片。halcon在企业用的多，但opencv更适合学术之用。我本着学习opencv和C++的目的，创建了这个项目。本项目从易到难，先从最简单的图片开始，一步一步往复杂背景方向靠拢。		

### 准备数据
&emsp;&emsp;下图是经过打光处理的易拉罐罐底图片，项目首先处理这一类的图片。（为了排版，缩小了图片，原图在代码文件夹内。图片为8为灰度图片）					

![待处理图片](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/src.png)

### 滤波与增强			
![滤波结果](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/emphasize.png)			

### 阈值分割				
![分割结果](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/binary.png)			

### 形态学处理			
![形态学处理结果](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/morphalogy.png)			

### 文字区域定位			
![文字区域定位结果](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/findCharRotated.png)			

### 旋转			
![旋转结果](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/rotated.png)			

### 文字区域重新定位			
![定位结果](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/findChar.png)			

### 缩小区域			
![缩小区域](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/reduced.png)			


### 单字符分割			
![单字符分割](https://github.com/weiSupreme/IndustrialCharRecognition/blob/master/readmeImgs/singleChar.png)			

### ANN识别字符

