[一篇未完成的说明文章](https://github.com/halolantern/blog/blob/main/%E8%8A%B1%E4%BA%86%E5%8D%81%E5%A4%A9%E6%97%B6%E9%97%B4%EF%BC%8C%E5%86%99%E4%BA%86%E4%B8%80%E9%97%A8%E8%A7%A3%E9%87%8A%E5%9E%8B%E8%AF%AD%E8%A8%80.md)

## 这是什么

这是一个脚本语言（目前是 ECMAScript 5.1 的子集）的运行时环境，类似于 Node.js，通过命令行方式执行。

如果你熟悉 ES5 ，那么你可以很轻松地编写可被正确执行的脚本。

## 这有什么

#### 已支持的语言特性：

- [x] 类型
  - [x] Undefined
    - [x] undefined
  - [x] Null
    - [x] null
  - [x] Number
    - [x] 符号数
    - [x] 浮点数
  - [x] Boolean
    - [x] true
    - [x] false
  - [x] String
    - [x] 单引号
    - [x] 双引号
  - [x] Object
    - [x] 创建对象 `new Object() 或 new 函数名() 或 {}`
    - [x] Function
      - [x] 创建函数  `function name(){} 或 var name = function(){}`
      - [x] 普通调用
      - [x] new 调用
      - [x] 闭包
      - [x] 一等公民
      - [x] 对象方法
    - [x] Array
      - [x] 创建数组 `new Array([item [, ...] ]) 或 new Array(length) 或 []`
- [ ] 语法
  - [x] 自动分号
  - [x] 语句
    - [x] 空语句
    - [x] var 声明语句
    - [x] 函数声明语句
    - [x] 表达式语句
    - [x] return
    - [x] if
    - [x] for
    - [x] break
    - [x] continue
    - [x] throw
  - [x] 表达式
    - [x] 函数表达式
    - [x] 函数调用表达式
    - [x] 成员表达式
      - [x] `object.property`
      - [x] `object[property]`
    - [x] 主值表达式
      - [x] this
      - [x] 标识符
        - [x] 首字母
          - [x] $
          - [x] _
          - [x] a-z
        - [x] a-z $ _ 0-9
      - [x] 字面量
        - [x] Primitive `undefined null true false 0-9 'singleQuoteString' "doubleQuoteString"`
        - [x] 对象字面量
        - [x] 数组字面量
      - [x] 括号表达式 如 `( expression )`
  - [ ] 注释
    - [x] 单行注释
    - [ ] 多行注释
  - [ ] 运算符
    - [ ] 算术
      - [x] \+ \- \* /
      - [ ] %
    - [x] 逻辑
      - [x] && || !
    - [x] 关系
      - [x] \>  \< \>=  \<= === !==
      - [ ] == !=
    - [ ] 赋值
      - [x] =
      - [ ] += -= *= /= %=
    - [x] 自增自减
      - [x] ++ --
    - [x] 条件
      - [x] test ? consequent : alternate
  - [x] 控制流
    - [x] 分支
      - [x] if
        - [x] 可选 else
    - [x] 循环
      - [x] for
      - [x] while
      - [x] do while
- [x] 内置全局对象 `global`
- [ ] 全局对象属性
  - [x] print （同 `console.log` ）
  - [x] console
    - [x] log
  - [x] Object
    - [x] prototype
  - [x] Function
    - [x] prototype
      - [x] call
      - [x] bind
  - [x] Array
    - [x] prototype
      - [x] push
      - [x] pop
  - [x] process
    - [x] nextTick
  - [x] setTimeout 目前只支持 0 ms
  - [ ] Promise
  - [ ] Gererator
- [ ] 异常处理
  - [x] 异常类型提示
  - [x] 异常位置提示
  - [ ] try catch

[可执行代码示例](https://github.com/halolantern/slowjs/blob/main/test.js)

#### 已支持的运行环境特性：

- [x] Event Loop
- [ ] 垃圾回收
- [ ] 多进程
- [ ] 多线程
- [ ] 协程
- [ ] I/O
  - [ ] File
  - [ ] Network

## 这为什么

忽想起高中哲学老师额拔说的话：看一百遍答案，不如做一遍。深以为然。

## 这怎么用

1. 如果是 MacOS ，直接下载仓库根目录下的 slow 可执行文件。

2. 否则在终端执行以下命令（注: $ 为命令行指令的意思，不需要输到终端里）。

   ```bash
   $ git clone git@github.com:halolantern/slowjs.git
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

   iii. slow 文件权限问题

   ​        a. 没有执行权限。解决方案：在 slow 文件的目录下执行 `$ chmod 777 slow` 。

   ​        b. MacOS 下没有打开权限或未知开发者。解决方案：System Preference -> Security & Privacy -> Allow Anyway 。

   iv. 运行时报错，解决方案：大概率是语法错误，根据提示修改语法。

   v. 其他问题，欢迎提 Issues。

## 这有什么用

Just For Fun.

## 巨人的肩膀

1. 词法分析参考 [the-super-tiny-compiler](https://github.com/jamiebuilds/the-super-tiny-compiler)
2. 语法分析参考 [acorn](https://github.com/acornjs/acorn) 和 [编译原理](https://book.douban.com/subject/3296317/)
3. 文法结构参考 [ECMAScript5.1 Annex A Grammar](https://262.ecma-international.org/5.1/#sec-A)
4. 语法树结构参考  [astexplorer](https://astexplorer.net/)
5. 语言类型参考 [quickjs](https://github.com/bellard/quickjs/blob/b5e62895c619d4ffc75c9d822c8d85f1ece77e5b/quickjs.h#L67) 和 [v8](https://github.com/v8/v8/blob/ed73693de815988a27ead2216acd7ab9955d9e92/include/v8.h#L2699)
