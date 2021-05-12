
// print 为 slowjs 内置函数，替代 console.log 用以向标准输出设备输出数据
// node 环境中没有 print, 用 node 运行这份测试代码时，打开下面一行注释
// print = console.log;

// 不支持自动加分号，语句后面一定要加分号；
var globalString = "global_var ";
var doubleQuote = "double string";
var singleQuote = 'single string';
var boolTrue = true;
var boolFalse = false;
var singedNumber = -3.14;
var consolelog = print;

print("[ globalString: ]", globalString);
print("[ doubleQuote: ]", doubleQuote);
print("[ singleQuote: ]", singleQuote);
print("[ singedNumber: ]", singedNumber);
print("[ boolTrue: ]", boolTrue);
print("[ boolFalse: ]", boolFalse);
print("[ consolelog: ]", consolelog);

print("[ function add hoist: ]", add);
print("[ var add10 hoist: ]", add10);

function add(a, b) {
    // '+' 加号运算符支持数字之间或字符串之间加，不支持隐式类型转换
    return a + b;
}
function addFactory(bindFirst) {
    function closure(second) {
        return add(bindFirst, second);
    }
    return closure;
}
var add10 = addFactory(10);
var add200 = addFactory(200);

print('[ 10 + 0.1 = ]', add10(0.1));
print('[ 200 + -1 = ]', add200(-1));

function highOrderFunction(add) {
    var local_string = 'local_var ';
    function closure() {
        function innerer() {
            print('[ nested innerner ]');
        }
        innerer();
        return add(local_string, globalString);
    };
    return closure;
};

var concat_global_local = highOrderFunction(add);
print('[ closure & first class function ]', concat_global_local());

var trueCount = 0;
var falseCount = 0;
var add1 = addFactory(1);
for (var i = 0; i < add200(1); i++) {
    if (i < 30) continue;
    
    if (i < 50) {
        trueCount = add1(trueCount);
    } else {
        falseCount = add1(falseCount);
    }

    if (i > 100) break;
}
print('[ for & if trueCount: ]', trueCount);
print('[ for & if falseCount: ]', falseCount);
