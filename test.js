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
    var test_single_quote = 'string';
    assert(test_string, test_single_quote);

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
    left++;
    assert(left, 9);
    left--;
    assert(left, 8);
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

function testConditional(assert) {
    var num = 10;
    var less = 0;
    var great = 0;

    if (num === 0) {
        less++;
    } else if (num === 1) {
        less--;
    } else {
        great++;
    }
    assert(less, 0);
    assert(great, 1);

    less === 0 ? great++ : great--;
    assert(great, 2);
}
function testIteration(assert) {
    var i = 0;
    var forLess10 = 0;
    var forGreat10 = 0;
    for (; i < 20; i++) {
        if (i < 10) {
            forLess10++;
            continue;
        }
        if (i >= 15) break;
        forGreat10++;
    }
    assert(forLess10, 10);
    assert(forGreat10, 5);

    var j = 0;
    var whileLess10 = 0;
    var whileGreat10 = 0;
    while (j < 20) {
        j++;

        if (j < 10) {
            whileLess10++;
            continue;
        }
        if (j >= 15) break;
        whileGreat10++;
    }
    assert(whileLess10, 9);
    assert(whileGreat10, 5);

    var k = 0;
    var doLess10 = 0;
    var doGreat10 = 0;
    do {
        k++;

        if (k < 10) {
            doLess10++;
            continue;
        }
        if (k >= 15) break;
        doGreat10++;
    } while (k < 20);

    assert(doLess10, 9);
    assert(doGreat10, 5);
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

function testNextTick(assert) {
    var s = '';
    function addChar(char) {
        s = s + char;
    }
    process.nextTick(addChar, '1');
    addChar('2');
    addChar('3');
    assert(s, '23');

    function delayRead() {
        assert(s, '231');
    }
    process.nextTick(delayRead);
}

function testPromiseLike(assert) {

    // simple Promise definition
    function Promise(executor) {
        var _this = this;
        function nextTickDoResolveCallback(value) {
            if (_this.onFulfilledCallback) {
                _this.onFulfilledCallback(value);
            }
        }
        function PromiseResolve(value) {
            if (_this.state === 'PENDING') {
                _this.state = 'FULFILLED';
                _this.value = value;

                process.nextTick(nextTickDoResolveCallback, value);
            }
        }
        function nextTickDoRejectCallback(reason) {
            if (_this.onRejectedCallback)
                _this.onRejectedCallback(reason);
        }
        function PromiseReject(reason) {
            if (_this.state === 'PENDING') {
                _this.state = 'REJECTED';
                _this.value = reason;

                process.nextTick(nextTickDoRejectCallback, reason);
            }
        }

        this.state = 'PENDING';
        this.value = null;
        this.onFulfilledCallback = null;
        this.onRejectedCallback = null;

        executor(PromiseResolve, PromiseReject);
    }
    function prototypeThen(onFulfilled, onRejected) {
        var p1 = this;
        p1.onFulfilledCallback = onFulfilled;
        p1.onRejectedCallback = onRejected;
    }
    Promise.prototype.then = prototypeThen;



    // simple Promise use case
    function executor(resolve, reject) {
        process.nextTick(resolve, 'success');
    }
    function onFulfilled(value) {
        assert(value, 'success');
    }
    function onRejected(reason) {
        assert(reason, 'fail');
    }
    var promise = new Promise(executor);
    promise.then(onFulfilled, onRejected);

    // simple Promise use case 2
    function executor2(resolve, reject) {
        reject('fail');
    }
    function onFulfilled2(value) {
        assert(value, 'success');
    }
    function onRejected2(reason) {
        assert(reason, 'fail');
    }
    var promise2 = new Promise(executor2);
    promise2.then(onFulfilled2, onRejected2);
}

describe('testType', testType);
describe('testOperator', testOperator);
describe('testClosure', testClosure);
describe('testHighOrderFunction', testHighOrderFunction);
describe('testConditional', testConditional);
describe('testIteration', testIteration);
describe('testObjectPrototype', testObjectPrototype);
describe('testNextTick', testNextTick);
describe('testPromiseLike', testPromiseLike);
