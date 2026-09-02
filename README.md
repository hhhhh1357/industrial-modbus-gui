# industrial-modbus-gui 工业 Modbus 上位机（GUI）项目合集

基于 Qt5 的通用工业 **Modbus 上位机图形界面**程序及整机配套资料（库源码、数据库、T113 底板硬件、系统镜像说明），合并归档，便于统一版本管理。

> ⚠️ 系统镜像（固件 `.img`，约 392MB）因超过 GitHub 单文件 100MB 上限，未纳入本仓库，见 [Modbuscailiao/系统镜像/说明.txt](Modbuscailiao/系统镜像/说明.txt)。

## 目录结构

```
industrial-modbus-gui/
├── Modbus20260603/              # 上位机软件源码（Qt5 完整工程）
│   ├── ModbusQt.pro             # qmake 工程文件
│   ├── *.cpp / *.h / *.ui       # 各界面(主页/设置/运行/调试/查询/登录/屏3)
│   ├── modbusrtu / modbustcp    # Modbus RTU / TCP 通信
│   ├── sqlitemanager            # SQLite 数据库封装
│   └── Images/                  # 界面用到的图片资源
└── Modbuscailiao/               # 配套资料
    ├── libmodbus库/             # libmodbus 源码(zip)：ubuntu 版 / 3.1.7
    ├── Sqlite库/ZongLin.db      # SQLite 数据库模板
    ├── 硬件/T113底板/           # T113 底板硬件文件(Gerber/BOM/贴片/立创EDA工程)
    └── 系统镜像/说明.txt        # 固件说明（镜像本身不入库）
```

## 上位机软件说明

Qt5 编写的 Modbus 采集/显示上位机，一套代码两个目标平台，由 `ModbusQt.pro`
按 `QT_ARCH` 自动选择：

| 目标平台      | 说明                                        | 数据库加载路径         |
|---------------|---------------------------------------------|------------------------|
| T113 (ARM)    | 交叉编译，`IMX6_PLATFORM`，链接 `-lmodbus`  | `/opt/Data_lib/ZongLin.db` |
| Ubuntu x86_64 | 本机调试，`UBUNTU_PLATFORM`，链接 `-lmodbus` | `/home/wp20/linux-imx6/XiangMu/ZongLin.db` |

**依赖：**
- Qt 5（`core gui widgets serialport sql`）
- libmodbus（源码见 `Modbuscailiao/libmodbus库/`，需自行编译/安装）
- SQLite（Qt 自带 `sql` 模块即可）

**编译前请注意：** `ModbusQt.pro` 中硬编码了本机路径
（如 `/home/wp20/T113/SDK-yuanma/...` 交叉编译 sysroot、`/home/wp20/tool/libmodbus-ubuntu/...`），
换电脑编译时需要改成你自己的实际路径。

## 硬件说明

`Modbuscailiao/硬件/T113底板/` 为 T113 最小系统底板的完整设计输出：

| 文件 | 内容 |
|------|------|
| `ProPrj_*.epro` | 立创EDA 工程文件 |
| `Gerber_*.zip`   | 制板 Gerber 文件 |
| `BOM_*.xlsx`     | BOM 物料清单 |
| `PickAndPlace_*.xlsx` | SMT 贴片坐标 |

## 使用建议

1. 先在本机（Ubuntu）打开 `Modbus20260603/ModbusQt.pro` 编译调试；
2. 确认无误后再用工具链交叉编译出 ARM 版本，随镜像烧录到 T113 开发板；
3. 把数据库模板放到板端 `/opt/Data_lib/ZongLin.db`。

## 授权说明

本仓库为私有归档仓库，尚未选择开源许可证，版权归作者所有。
如需对外开源，请先补齐 `LICENSE` 文件。
