// 暂不支持自动加分号，语句后面一定要加分号；
print = console.log;

function describe(title, fn) {
    function assert(actual, expected) {
        if (actual === expected) return;

        print("FAIL in " + title);
        print("got |", actual, "|", ", expected |", expected, "|");
        throw "";
    }

    fn(assert);
    print('PASS ' + title);
}

describe('testHoist', testHoist);

function testHoist(assert) {
    assert(hoist_var, undefined);
}
var hoist_var = 'a';

function testType(assert) {
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
}

function testOperator(assert) {
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
}

var a = 1;
function testClosure(assert) {
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
}

function testHighOrderFunction(assert) {
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
}

function testIFAndForStatement(assert) {
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
}

function testObjectPrototype(assert) {
    Object.prototype.onObjectPrototype = 0;
    var obj = new Object();
    assert(obj.onObjectPrototype, 0);
    var obj__proto = Object.getPrototypeOf(obj);
    assert(obj__proto, Object.prototype);
    var OPrototype__proto = Object.getPrototypeOf(Object.prototype);
    assert(OPrototype__proto, null);

    function Person(name, age) {
        this.name = name;
        this.age = age;
    }

    Person.prototype.canWalk = true;
    Person.prototype.canFly = false;

    var per = new Person('zx', 18);
    assert(per.onObjectPrototype, 0);
    assert(per.name, 'zx');
    assert(per.age, 18);

    var per__proto = Object.getPrototypeOf(per);
    assert(per__proto, Person.prototype);

    var per__proto__proto = Object.getPrototypeOf(per__proto);
    assert(per__proto__proto, Object.prototype);

    function Student(name, age, grade) {
        Person.call(this, name, age);
        this.grade = grade;
    }
    function Temp() { }
    Temp.prototype = Person.prototype;
    Student.prototype = new Temp();

    var stu = new Student('zx', 18, 3);
    assert(stu.onObjectPrototype, 0);
    assert(stu.canWalk, true);
    assert(stu.canFly, false);
    assert(stu.name, 'zx');
    assert(stu.age, 18);
    assert(stu.grade, 3);

    var stu__proto = Object.getPrototypeOf(stu);
    assert(stu__proto, Student.prototype);

    var stu__proto__proto = Object.getPrototypeOf(stu__proto);
    assert(stu__proto__proto, Person.prototype);

    var stu__proto__proto__proto = Object.getPrototypeOf(stu__proto__proto);
    assert(stu__proto__proto__proto, Object.prototype);
}

function delay(a, b, c) {
    print('in delay', a, b, c);
}
process.nextTick(delay, 1, 2, 3);

describe('testType', testType);
describe('testOperator', testOperator);
describe('testClosure', testClosure);
describe('testHighOrderFunction', testHighOrderFunction);
describe('testIFAndForStatement', testIFAndForStatement);
describe('testObjectPrototype', testObjectPrototype);

