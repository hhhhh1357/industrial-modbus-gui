# modbus-master-slave-template

> 工业「上位机 / 下位机」程序模板 —— 主机(Host)与从机(Device)之间通过 **Modbus**（RTU / TCP）互联。
> 一整套已跑通、可直接作为起步框架改造复用的工程与配套资料合集。

本仓库以**程序模板**的形态归档：主机端采集 / 显示 / 控制程序、从机端固件程序，
以及通用库和硬件资料，合并统一管理，便于在各自项目里按需裁剪、复用与改版。

> 说明：为使本模板更通用、可放心对外或跨项目复用，目录 / 工程均采用**不针对特定产品**
> 的通用命名；文档也不再绑定某一具体业务，实际采集对象、寄存器表与界面请自行定制。

## 目录结构

```
.
├── ModbusGUI/        # 上位机（Host）软件源码 —— Qt5 完整工程
│                     #   Modbus RTU / TCP 通信、SQLite 存储、多页面界面
├── 下位机/           # 下位机（Device）固件源码 —— STM32 工程（Keil MDK）
│                     #   FreeRTOS + HAL、常用外设驱动、Modbus 从机通信
│                     #   另含从机配套硬件设计工程 硬件/DeviceBoard.eprj
└── Resources/        # 配套资料 —— libmodbus 源码、SQLite 数据库模板、
                      #   上位机硬件设计文件、系统镜像说明 等
```

## 上位机 ModbusGUI（Host）

Qt5 编写的上位机程序，一套代码两个目标平台，由 `ModbusQt.pro` 按 `QT_ARCH` 自动选择：

| 目标平台      | 说明                                        | 数据库加载路径         |
|---------------|---------------------------------------------|------------------------|
| T113 (ARM)    | 交叉编译，`IMX6_PLATFORM`，链接 `-lmodbus`  | `/opt/Data_lib/ZongLin.db` |
| Ubuntu x86_64 | 本机调试，`UBUNTU_PLATFORM`，链接 `-lmodbus` | `/home/wp20/linux-imx6/XiangMu/ZongLin.db` |

**依赖：**
- Qt 5（`core gui widgets serialport sql`）
- libmodbus（源码见 `Resources/libmodbus库/`，需自行编译 / 安装）
- SQLite（Qt 自带 `sql` 模块即可）

**编译前请注意：** `ModbusQt.pro` 中硬编码了本机路径
（如 `/home/wp20/T113/SDK-yuanma/...` 交叉编译 sysroot、`/home/wp20/tool/libmodbus-ubuntu/...`），
换电脑编译时需要改成你自己的实际路径。

## 下位机 下位机（Device）

STM32F103 从机固件工程（STM32CubeMX 生成风格，FreeRTOS 实时任务框架）：

- **打开工程**：`下位机/MDK-ARM/Device.uvprojx`（Keil MDK 直接编译烧录）。
  `.uvoptx` / `.uvguix.*` 等个人配置不入库，首次打开会自动生成。
- **代码布局**：`Core/Src` 主逻辑与各外设驱动、`Drivers` HAL / CMSIS 驱动库、
  `Middlewares/Third_Party` FreeRTOS。
- **硬件设计**：`下位机/硬件/DeviceBoard.eprj`（从机板立创EDA 工程，未压缩约 56MB 单文件入库）。
- **已剔除内容**：Keil 编译产物、Source Insight 缓存（约 80MB）、以及工程未引用的
  CMSIS 大体积子库（`DSP` / `Lib` 等，源码工程并未链接它们）均通过
  `下位机/.gitignore` 排除，克隆后可正常重开编译。详见该文件说明。

> 主机 ↔ 从机的数据流约定：上位机通过串口 / TCP 发起 Modbus 请求，
> 从机在 FreeRTOS 任务中解析请求、采集数据并回帧。寄存器地址表与数据格式
> 属于具体业务，请结合 `Resources/Sqlite库/ZongLin.db` 等自行对接。

## 配套硬件资料

- `Resources/硬件/T113底板/`：上位机运行平台（T113 最小系统）完整设计输出：
  Gerber 制板文件、BOM 物料清单、SMT 贴片坐标、立创EDA 工程文件。
- `下位机/硬件/DeviceBoard.eprj`：从机板硬件设计工程。
- ⚠️ 系统镜像（固件 `.img`，约 392MB）超过 GitHub 单文件 100MB 上限未入库，
  见 [Resources/系统镜像/说明.txt](Resources/系统镜像/说明.txt)。

## 使用建议

1. 本机（Ubuntu）先打开 `ModbusGUI/ModbusQt.pro` 编译上位机并调试；
2. Keil 打开 `下位机/MDK-ARM/Device.uvprojx` 编译下载从机固件；
3. 串口（RS-485/232）或网络把两端连起来，核对 Modbus 从站地址与波特率一致即可联调；
4. 移植到新项目时，重点替换：界面业务、采集 / 控制任务、寄存器地址表与数据库。

## 授权说明

本仓库为私有归档仓库，尚未选择开源许可证，版权归作者所有。
如需对外开源，请先补齐 `LICENSE` 文件。
