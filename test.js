
print = console.log;

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

print('[ operator ]');
print('8 + 2', 8 + 2);
print('8 - 2', 8 - 2);
print('8 * 2', 8 * 2);
print('8 / 2', 8 / 2);
print('8 > 3', 8 > 3);
print('8 >= 3', 8 >= 3);
print('8 < 3', 8 < 3);
print('8 <= 3', 8 <= 3);
print('8 == 3', 8 == 3);
print('8 === 3', 8 === 3);
print('8 && 0', 8 && 0);
print('8 || 3', 8 || 3);
print('!false', !false);

function ab() { this.a = 1; }
// Object.prototype = new ab();
// Object.prototype.NAME = 1;
// var obj = new Object();
// var obj_proto = Object.getPrototypeOf(obj);
// print(obj_proto);


function Person(name, age) {
    this.name = name;
    this.age = age;
}

var p = new Person();

function Student(grade) {
    this.grade = grade;
}
Student.prototype = new Person('zx', 18);

var stu = new Student(3);
// console.log(stu, stu.name, stu.age, stu.grade);

var p_proto = Object.getPrototypeOf(p);
print(p_proto);
var a = Object.getPrototypeOf(p_proto);
print(a);
var a_proto = Object.getPrototypeOf(a);
print(a_proto);
var b = Object.getPrototypeOf(a_proto);
print(b);
var b_proto = Object.getPrototypeOf(b);
print(b_proto);
print(p.c);




