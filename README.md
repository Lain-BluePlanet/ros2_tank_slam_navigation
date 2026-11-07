# ROS 2 四轮差速机器人 SLAM 与 Nav2 导航仿真

## 📖 项目简介
本项目基于 Ubuntu 22.04、ROS 2 Humble 与 Gazebo 11 环境，从零手搓搭建了一台四轮差速移动机器人。项目涵盖了从 URDF/Xacro 运动学建模、物理引擎动力学调优、激光雷达仿真，到 SLAM 建图与 Nav2 自主导航的完整闭环。这是我大一阶段最具挑战性、也收获最大的硬核项目。

## 🛠️ 技术栈
- **系统环境**: Ubuntu 22.04 (VirtualBox 虚拟机)
- **中间件**: ROS 2 Humble
- **仿真环境**: Gazebo 11
- **核心算法**: slam_toolbox, Nav2 (AMCL, DWB Controller)

## ✨ 核心功能
1. **Xacro 建模**：使用宏定义手写四轮差速底盘，包含底盘、4个驱动轮、车顶激光雷达。
2. **底层控制**：独立编写 C++ 节点实现 WASD 键盘控制，发布 `/cmd_vel` 话题。
3. **SLAM 建图**：基于 `slam_toolbox` 实现 2D 栅格地图构建，并成功保存 `.pgm` 与 `.yaml` 文件。
4. **Nav2 导航**：实现静态地图加载、AMCL 初始定位、全局路径规划与局部避障。

---

## 🚨 血泪踩坑记录（Troubleshooting）

### 1. 物理引擎的“玄学”与调参
*   **原地打滑 / 螃蟹步**：初期给轮子设了过大的摩擦力，导致转弯卡顿。通过调整前后轮的 `mu1/mu2`（切向/法向摩擦），并给从动轮设置极小的摩擦（0.01）当做万向轮解决。

*   ### 🎨 附：空间几何与“螃蟹步”原理手绘图



为了彻底搞懂并记录这个空间坐标系的冲突，我手绘了下面的示意图。

<img width="1920" height="1080" alt="微信图片_20261107145537_268_238" src="https://github.com/user-attachments/assets/3874ab5c-2600-4ade-b43e-1c978a21497d" />


**原理解析**：
1. 如图一（上方）所示，轮子初始立着时，Y轴朝向左右。
2. 如图二（下方）所示，如果在 `<joint>` 中直接使用 `rpy="-1.5708 0 0"` 将坐标系绕 X 轴翻转 90 度，原本水平的 Y 轴会被翻转成垂直地面的轴。
3. 此时电机指令 `<axis xyz="0 1 0"/>` 会让轮子像“地上的方向盘”一样水平打转，导致小车横向平移（螃蟹步）。
4. **完美解决方案**：将物理关节 `<joint>` 的 `rpy` 归零，保持 Y 轴水平；把旋转操作移到 `<visual>` 和 `<collision>` 中。这样电机的旋转轴是水平的，而外观模型是躺平的，实现了物理与视觉的完美解耦。
    
*   **底盘升天 / 穿模**：在加入 `base_footprint` 坐标系后，由于底盘高度（`z` 轴）与轮子半径计算不匹配，导致 Gazebo 物理引擎疯狂抽搐，甚至把车弹飞。最终通过精确计算关节 `origin` 的偏移量，让轮子底部完美贴合地面。
*   **小车跑偏**：按直线前进时，小车会轻微拐弯。这是虚拟机的物理引擎算力不足，加上左右轮扭矩分布微小的不均匀导致的，待解决。

### 2. ROS 2 通信与 QoS 协议
*   **现象**：Nav2 的代价地图（Costmap）无法接收雷达数据，终端疯狂刷 `incompatible QoS`。
*   **原因**：Gazebo 发布的激光雷达数据默认是 `Best Effort`（尽力而为，不保证送达），而 Nav2 默认订阅是 `Reliable`（可靠，保证不丢包）。两边协议不匹配，直接断联。
*   **解决**：修改底层 `nav2_params.yaml`，在 `scan` 下强制添加 `reliability_policy: "best_effort"`。

### 3. TF 树断裂与坐标系打架
*   **现象**：AMCL 报错 `Invalid frame ID "base_footprint"`，无法计算位姿。
*   **原因**：官方 Nav2 默认底盘叫 `base_footprint`，而我建模时只有 `base_link`。
*   **解决**：在 Xacro 最顶层新建 `base_footprint`，并用 `fixed` 关节连接到 `base_link`。

### 4. 同步仿真时间和真实时间（加use_sim_time:=true）

---

<img width="2056" height="1225" alt="微信图片_20261107165056_273_238" src="https://github.com/user-attachments/assets/25c5fc0b-82e3-47b6-b82c-bf1488aef00e" />
<img width="1977" height="1280" alt="微信图片_20261107164825_272_238" src="https://github.com/user-attachments/assets/bd6f94b9-1f9a-4600-b575-7717222b2f9a" />

