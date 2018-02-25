# lighting
A C++ render tool using OpenGL to render a .off 3Dmodel with lighting source

.off 3D模型文件中只有顶点的坐标数据，并没有纹理和法线数据，本程序适用于由三角面片组成的.off 3D模型，并自行根据各面片计算顶点的法线，得到3D模型的渲染结果
![out1](http://github.com/yuzhegao/lighting/raw/master/screenshot/1.png)
lighting.cpp 是渲染光照模型的程序，generate_data3.cpp是本人用于生成数据集的程序
