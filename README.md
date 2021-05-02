## 这是什么

这是一个脚本语言（目前是 ECMAScript 5 的子集）的运行时环境，类似于 Node.js，通过命令行方式执行。

如果你熟悉 ES5 ，那么你可以很轻松地编写可被正确执行的脚本。

## 这有什么

已支持的语言特性：

- [ ] 类型
  - [x] String
    - [x] 单引号
    - [x] 双引号
  - [x] Number
    - [x] 符号数
    - [x] 浮点数
  - [x] Boolean
    - [x] true
    - [x] false
  - [x] Undefined
    - [x] undefined
  - [x] Null
    - [x] Null
  - [ ] Object
    - [x] function 普通调用，闭包，支持函数一等公民。暂不支持作为对象的方法。
    - [ ] other
- [ ] 语法
  - [ ] 自动分号，暂不支持，目前语句一定要加分号。
  - [x] 语句
    - [x] var 声明语句
    - [x] 函数声明语句
    - [x] return
    - [x] 空语句
    - [x] if
    - [x] for
    - [x] break
    - [x] 表达式语句
  - [ ] 表达式
    - [ ] 成员表达式 如 `object.property object[property]`
    - [ ] 算术
      - [x]  \+ \- \* / 
      - [ ] %
    - [ ] 关系
      - [x]  \>  \< 
      - [ ] \>=  \<=
    - [ ] 赋值 
      - [x] =
      - [ ] += -= *= /= %=
    - [ ] 相等
      - [x] ==
      - [ ] !=
    - [x] 自增
      - [x] ++ --
    - [x] 函数调用
    - [x] 主值表达式
      - [ ] this 
      - [x] 标识符
        - [x] 首字母
          - [x] $
          - [x] _
          - [x] a-z $ _ 0-9
        - [x] a-z
      - [x] 字面量
        - [x] Primitive `undefined null true false 0-9 'singleQuoteString' "doubleQuoteString"`
        - [ ] 对象字面量
        - [ ] 数组字面量
      - [x] 括号表达式 如 `( expression )`
    - [ ] 注释
      - [x] 单行注释
      - [ ] 多行注释
  - [ ] 控制流
    - [x] 分支
      - [x] if
        - [x] 可选 else
    - [x] 循环
      - [x] for
      - [ ] do
      - [ ] While
- [ ] 内置全局对象
  - [x] print （目前用来代替 console.log 向标准输出设备输出数据）
  - [ ] Promise
  - [ ] Gererator
  - [ ] setTimeout
- [ ] 垃圾回收
- [ ] 异常处理
  - [x] 异常类型提示
  - [ ] 异常位置提示

已支持的运行环境特性：（目前竟然全不支持 T_T）

- [ ] Event Loop
- [ ] 多进程/多线程
- [ ] I/O
  - [ ] File 
  - [ ] Network

## 这为什么

忽想起高中哲学老师额拔说的话：看一百遍答案，不如做一遍。深以为然。

## 这怎么用

1. 如果是 MacOS ，直接下载仓库根目录下的 slow 可执行文件。

2. 否则在终端执行以下命令（注: $ 为命令行指令的意思，不需要输到终端里）。

   ```bash
   $ git clone <repo-url>
   $ cd slowjs
   $ make
   ```

3. 如果成功，在根目录下会生成 slow 可执行文件，更改 test.js 或者使用自定义的文件路径。

   ```bash
   $ ./slow test.js
   ```
   
4. 如果失败，可能原因：

   i. 没装 make，解决方案：安装一下。

   ii. 没装 g++，解决方案：安装一下。

   iii. slow 文件没有执行权限，解决方案：在 slow 文件的目录下执行 `$ chmod 777 slow` 。

   iii. 运行时报错，解决方案：大概率是语法错误，根据提示修改语法。
   
   iv. 其他问题，欢迎提 Issues。

## 这有什么用

Just For Fun

## 巨人的肩膀

1. 词法分析参考 [the-super-tiny-compiler](https://github.com/jamiebuilds/the-super-tiny-compiler)
2. 语法分析参考 [acorn](https://github.com/acornjs/acorn) 和 [编译原理](https://book.douban.com/subject/3296317/)
3. 文法结构参考 [ECMAScript5.1 Annex A Grammar](https://262.ecma-international.org/5.1/#sec-A)
4. 语法树结构参考  [astexplorer](https://astexplorer.net/)
5. 语言类型参考 [quickjs](https://github.com/bellard/quickjs/blob/master/quickjs.h)

