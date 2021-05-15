// 暂不支持自动加分号，语句后面一定要加分号；
print = console.log;

function assert(actual, expected) {
    if (actual === expected) return;

    print("assertion failed: got |", actual, "|", ", expected |", expected, "|");
}

function testHoist() {
    assert(hoist_var, undefined);
    print('testHoist done');
}
testHoist();
var hoist_var = 'a';

function testType() {
    var test_undefined = undefined;
    assert(test_undefined, undefined);

    var test_null = null;
    assert(test_null, null);

    var test_int = 1;
    assert(test_int, 1);

    var test_signed = -3.14;
    assert(test_signed, -3.14);

    var test_float = 1.0;
    assert(test_float, 1.0);

    var test_false = false;
    var test_true = true;
    assert(test_false, !test_true);
    assert(test_true, !test_false);

    var test_string = "string";
    assert(test_string, "string");

    var test_obj = new Object();
    test_obj.num = 1;
    test_obj.bool = true;
    test_obj.str = 'obj';
    test_obj.nu = null;
    assert(test_obj.num, 1);
    assert(test_obj.bool, true);
    assert(test_obj.str, 'obj');
    assert(test_obj.u, undefined);
    assert(test_obj.nu, null);

    print('testType done');
}

function testConstructor() {
    function Person(name, age) {
        this.name = name;
        this.age = age;
    }
    Person.prototype.height = 180;
    Person.prototype.weight = 70;

    function Student(grade) {
        this.grade = grade;
    }

    assert(Person, Person);
    var per = new Person();
    var stu = new Student();
    var tempPer = per;
    assert(tempPer, per);
    assert(per.height, 180);
    assert(per.weight, 70);

    print('testConstructor done');
}

var a = 1;
function testClosure() {
    var b = 2;
    function outer() {
        var c = 3;
        function inner() {
            var d = 4;
            return a + b + c + d;
        }
        return inner;
    }
    var closure = outer();
    assert(closure(), 10);

    print("testClosure done");
}

function testHighOrderFunction() {
    function add(a, b) {
        return a + b;
    }
    function highOrderFunction(fn, num1, num2) {
        function closure() {
            return fn(num1, num2);
        };
        return closure;
    }
    var test = highOrderFunction(add, 1, 2);
    assert(test(), 3);

    print('testHighOrderFunction done');
}

function testIFAndForStatment() {
    var lessCount = 0;
    var greatCount = 0;

    for (var i = 0; i < 200; i++) {
        if (i <= 10) continue;

        if (i <= 30) {
            lessCount = lessCount + 1;
        } else {
            greatCount = greatCount + 1;
        }

        if (i >= 100) break;
    }
    assert(lessCount, 20);
    assert(greatCount, 70);

    print('testIFAndForStatment done');
}

function testOperator() {
    var left = 8;
    var right = 2;

    assert(left + right, 10);
    assert(left - right, 6);
    assert(left * right, 16);
    assert(left / right, 4);
    assert(left > right, true);
    assert(left >= right, true);
    assert(left < right, false);
    assert(left <= right, false);
    assert(left === right, false);
    assert(left !== right, true);
    assert(left && right, 2);
    assert(left || right, 8);
    assert(0 && right, 0);
    assert(0 || right, 2);
    assert(!left, false);
    assert(!!left, true);

    print("testOperator done");
}


testType();
testConstructor();
testClosure();
testHighOrderFunction();
testIFAndForStatment();
testOperator();




