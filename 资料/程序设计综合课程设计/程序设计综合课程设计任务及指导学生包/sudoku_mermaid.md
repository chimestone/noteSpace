# 数独模块Mermaid流程图

## 数独系统总体架构

```mermaid
flowchart TD
    A[数独系统] --> B[数独生成模块]
    A --> C[数独游戏模块]
    A --> D[规则验证模块]
    A --> E[CNF转换模块]
    
    B --> B1[随机化回溯算法]
    B --> B2[挖空处理]
    B --> B3[完整解生成]
    
    C --> C1[文件选择]
    C --> C2[游戏交互]
    C --> C3[状态管理]
    
    D --> D1[标准数独规则]
    D --> D2[百分号特殊约束]
    D --> D3[完整性验证]
    
    E --> E1[约束生成]
    E --> E2[CNF编码]
    E --> E3[文件输出]
    
    B -.-> E
    C -.-> D
    D -.-> C
    
    classDef module fill:#87CEEB
    classDef submodule fill:#DDA0DD
    
    class A,B,C,D,E module
    class B1,B2,B3,C1,C2,C3,D1,D2,D3,E1,E2,E3 submodule
```

## 数独生成流程

```mermaid
flowchart TD
    A[开始] --> B[随机化回溯生成完整解]
    B --> C{找到空位?}
    C -->|是| D[随机尝试数字1-9]
    C -->|否| E[生成完成]
    D --> F{验证规则?}
    F -->|通过| G[填入并递归]
    F -->|失败| H[尝试下个数字]
    G -->|成功| E
    G -->|失败| I[回溯]
    I --> H
    H --> J{还有数字?}
    J -->|是| D
    J -->|否| K[返回失败]
    E --> L[随机挖空]
    L --> M[保存为CNF]
    
    classDef success fill:#90EE90
    classDef process fill:#87CEEB
    classDef failure fill:#FFB6C1
    
    class E,M success
    class B,D,G,L process
    class K,I failure
```

## 数独游戏简略大纲

```mermaid
flowchart TD
    A[数独游戏] --> B[生成数独]
    A --> C[游戏交互]
    A --> D[规则验证]
    A --> E[CNF处理]
    
    B --> B1[完整解生成]
    B --> B2[随机挖空]
    
    C --> C1[文件选择]
    C --> C2[用户输入]
    C --> C3[状态显示]
    
    D --> D1[标准规则]
    D --> D2[百分号约束]
    
    E --> E1[CNF转换]
    E --> E2[文件保存]
    
    classDef main fill:#87CEEB
    classDef sub fill:#DDA0DD
    
    class A,B,C,D,E main
    class B1,B2,C1,C2,C3,D1,D2,E1,E2 sub
```

## 数独游戏流程

```mermaid
flowchart TD
    A[开始游戏] --> B[选择CNF文件]
    B --> C[解析提示数字]
    C --> D[显示数独]
    D --> E{完成?}
    E -->|是| F[游戏成功]
    E -->|否| G[用户输入]
    G --> H{验证规则?}
    H -->|通过| I[填入数字]
    H -->|失败| J[显示错误]
    I --> D
    J --> G
    
    classDef success fill:#90EE90
    classDef process fill:#87CEEB
    classDef error fill:#FFB6C1
    
    class F success
    class B,C,D,I process
    class J error
```

## 百分号数独规则验证

```mermaid
flowchart TD
    A[验证数字] --> B[检查行]
    B --> C[检查列]
    C --> D[检查3x3盒子]
    D --> E[检查反对角线]
    E --> F[检查窗口(2,2)-(4,4)]
    F --> G[检查窗口(6,6)-(8,8)]
    G --> H[返回结果]
    
    classDef process fill:#87CEEB
    class A,B,C,D,E,F,G,H process
```

## CNF转换流程

```mermaid
flowchart TD
    A[开始转换] --> B[生成格约束]
    B --> C[生成行约束]
    C --> D[生成列约束]
    D --> E[生成盒子约束]
    E --> F[生成对角线约束]
    F --> G[生成窗口约束]
    G --> H[添加提示约束]
    H --> I[保存CNF文件]
    
    classDef process fill:#87CEEB
    class A,B,C,D,E,F,G,H,I process
```