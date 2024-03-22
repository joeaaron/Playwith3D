# EasyCloud

运行环境：Win10 + PCL1.9 + VTK8.1 + Qt5.12 + VS2017

基于PCL点云库的三维点云数据处理软件

#### **1．软件用户界面**

本软件的用户界面主要包括以下几个部分（如图1）：

[1]菜单栏，将软件功能模块划分为不同菜单，包括文件、视图、显示，点云简化、特征提取、三维重建以及帮助等；

[2]工具栏，常用工具按钮，如打开，保存，更改点云颜色等；

[3]点云文件管理窗口，用资源树的形式组织管理当前的点云文件；

[4]属性窗口，记录点云文件个数、点得个数，颜色等常用属性；

[5]操作记录窗口，记录用户操作日志，如操作时间、操作对象等；

[6]状态栏，显示软件当前工作状态；

[7]点云视图窗口，点云可视化的主要窗口，用来显示渲染点云；

[8]显示管理窗口，用来快速修改点云颜色及显示尺寸等。

其中[3]、[4]、[5]、[8]都是停靠窗口，可以在菜单栏的视图菜单中勾选以设置是否显示，方便用户个性化设置。

![](.\media\1.png)

图1：软件用户界面

除基本用户界面外，本软件设计了一些必要的处理消息弹出窗口，提高交互性且不会冗余繁杂，提升用户体验。

#### **2．软件功能设计**

##### **2.1 点云I/O功能**

点云I/O功能是进行三维点云数据处理的基础。PCL点云库中IO模块包含读取和写入点云数据（PCD）文件的类和函数，本软件在此基础上实现了多种格式的**三维点云数据读取、保存、格式转换**等功能。点云I/O功能常用操作在文件菜单及工具栏中均有实现（如图2）。

![](.\media\2.png)

打开动作可以实现PCD、PLY和OBJ在内的三种格式的三维点云数据的读取，并支持多选进行批量打开。

添加动作可以在不清空当前缓存点云数据的情况下，添加新的点云到内存中。方便用户同时对多个点云在同一点云视图窗口进行可视化和处理。

保存动作可以将点云数据保存为PCD和PLY格式，并且支持多个点云保存为一个文件，这样同时实现了点云格式转换和点云合并的功能。

清空动作用于一键清空内存中的点云数据，提高工作效率。

##### **2.2 点云可视化功能**

PCL点云库中的Visulization模块实现三维点云数据可视化，提供点云渲染和显示属性设置等功能。该类具有很全面的功能，如显示法线、绘制多种形状和多个视口等，其内部实现了添加各种3D对象以及交互实现等。其中addCoordinateSystem()函数可以在可视化窗口中的坐标原点(0，0，0）处添加一个红绿蓝三色的三维指示坐标轴，红色是X轴，绿色是Y轴，蓝色是Z，也就是说PCL点云库中使用的是右手三维坐标系。本软件调用此模块实现的点云可视化功能包括设置点云显示颜色、设置点云显示尺寸以及设置背景颜色等功能。

![](.\media\3.png)

图3：点云可视化

##### **2.3 点云滤波功能**

该功能主要是三维空间中的点云数据集上进行滤波处理，以去除噪声、平滑数据或提取感兴趣的特征。

![](.\media\4.png)

图4：点云滤波

##### 2.4 点云特征提取功能

该功能主要是利用基于邻域分析的特征提取算法，实现油菜叶片边缘轮廓的提取。

##### **2.5 点云简化功能**

该功能主要是利用保留点云边界特征的点云简化方法，实现油菜点云简化处理。

##### **2.6 点云三维重建功能**

该功能基于贪婪三角投影算法，实现三维点云重建，并可以实现三角面片和三角网格两种显示。

##### **2.7 其他功能**

点云文件管理窗口通过资源树的形式实现当前点云文件组织管理，右键响应菜单实现点云的隐藏、显示、删除以及颜色设置等操作（如图5），非常快捷，之后还可以进一步扩展功能。

![](.\media\5.png)

图5：点云资源树右键响应菜单

属性窗口显示记录了点云的一些属性：打开的点云的总个数、选中点云的点的数量、颜色信息、以及所有点云点的数量等，方便用户查看（如图6）。

![](.\media\6.png)

图6：属性窗口

操作记录窗口记录用户操作日志，包括时间，操作对象，运行时间等信息，方便用户查看，右键响应菜单支持用户清空操作记录，或自行选择禁用或使用操作记录窗口（如图7）。

![](.\media\7.png)

图7：操作记录窗口

------

对源码有需求的，请订阅后后台私信我~