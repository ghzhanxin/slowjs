print = console.log

function describe(title, fn) {
    function assert(actual, expected) {
        if (actual === expected) return

        print("FAIL in " + title)
        print("got |", actual, "|", ", expected |", expected, "|")
        throw ""
    }
    function done() {
        print('PASS ' + title)
    }

    if (!fn(assert, done))
        done()
}

describe('testHoist', function (assert) {
    assert(hoist_var, undefined)
})
var hoist_var = 'a'

function testType(assert) {
    var test_undefined = undefined
    assert(test_undefined, undefined)

    var test_null = null
    assert(test_null, null)

    var test_int = 1
    assert(test_int, 1)

    var test_signed = -3.14
    assert(test_signed, -3.14)

    var test_float = 1.0
    assert(test_float, 1.0)

    var test_false = false
    var test_true = true
    assert(test_false, !test_true)
    assert(test_true, !test_false)

    var test_string = "string"
    assert(test_string, "string")
    var test_single_quote = 'string'
    assert(test_string, test_single_quote)

    var test_obj = new Object()
    test_obj.num = 1
    test_obj.bool = true
    test_obj.str = 'obj'
    test_obj.nu = null
    assert(test_obj.num, 1)
    assert(test_obj.bool, true)
    assert(test_obj.str, 'obj')
    assert(test_obj.u, undefined)
    assert(test_obj.nu, null)
}

function testOperator(assert) {
    var left = 8
    var right = 2

    assert(left + right, 10)
    assert(left - right, 6)
    assert(left * right, 16)
    assert(left / right, 4)
    assert(left > right, true)
    assert(left >= right, true)
    assert(left < right, false)
    assert(left <= right, false)
    assert(left === right, false)
    assert(left !== right, true)
    assert(left && right, 2)
    assert(left || right, 8)
    assert(0 && right, 0)
    assert(0 || right, 2)
    assert(!left, false)
    assert(!!left, true)
    left++
    assert(left, 9)
    left--
    assert(left, 8)
}

var a = 1
function testClosure(assert) {
    var b = 2
    function outer() {
        var c = 3
        return function inner() {
            var d = 4
            return a + b + c + d
        }
    }
    var closure = outer()
    assert(closure(), 10)
}

function testHighOrderFunction(assert) {
    function add(a, b) {
        return a + b
    }
    function highOrderFunction(fn, num1, num2) {
        return function closure() {
            return fn(num1, num2)
        }
    }
    var test = highOrderFunction(add, 1, 2)
    assert(test(), 3)
}

function testConditional(assert) {
    var num = 10
    var less = 0
    var great = 0

    if (num === 0) {
        less++
    } else if (num === 1) {
        less--
    } else {
        great++
    }
    assert(less, 0)
    assert(great, 1)

    less === 0 ? great++ : great--
    assert(great, 2)
}
function testIteration(assert) {
    var i = 0
    var forLess10 = 0
    var forGreat10 = 0
    for (; i < 20; i++) {
        if (i < 10) {
            forLess10++
            continue
        }
        if (i >= 15) break
        forGreat10++
    }
    assert(forLess10, 10)
    assert(forGreat10, 5)

    var j = 0
    var whileLess10 = 0
    var whileGreat10 = 0
    while (j < 20) {
        j++

        if (j < 10) {
            whileLess10++
            continue
        }
        if (j >= 15) break
        whileGreat10++
    }
    assert(whileLess10, 9)
    assert(whileGreat10, 5)

    var k = 0
    var doLess10 = 0
    var doGreat10 = 0
    do {
        k++

        if (k < 10) {
            doLess10++
            continue
        }
        if (k >= 15) break
        doGreat10++
    } while (k < 20)

    assert(doLess10, 9)
    assert(doGreat10, 5)
}

function testObject(assert) {
    Object.prototype.onObjectPrototype = 0
    var obj = new Object()
    assert(obj.onObjectPrototype, 0)
    var obj__proto = Object.getPrototypeOf(obj)
    assert(obj__proto, Object.prototype)
    var OPrototype__proto = Object.getPrototypeOf(Object.prototype)
    assert(OPrototype__proto, null)

    function Person(name, age) {
        this.name = name
        this.age = age
    }

    Person.prototype.canWalk = true
    Person.prototype.canFly = false

    var per = new Person('zx', 18)
    assert(per.onObjectPrototype, 0)
    assert(per.name, 'zx')
    assert(per.age, 18)

    var per__proto = Object.getPrototypeOf(per)
    assert(per__proto, Person.prototype)

    var per__proto__proto = Object.getPrototypeOf(per__proto)
    assert(per__proto__proto, Object.prototype)

    function Student(name, age, grade) {
        Person.call(this, name, age)
        this.grade = grade
    }
    function Temp() { }
    Temp.prototype = Person.prototype
    Student.prototype = new Temp()

    var stu = new Student('zx', 18, 3)
    assert(stu.onObjectPrototype, 0)
    assert(stu.canWalk, true)
    assert(stu.canFly, false)
    assert(stu.name, 'zx')
    assert(stu.age, 18)
    assert(stu.grade, 3)

    var stu__proto = Object.getPrototypeOf(stu)
    assert(stu__proto, Student.prototype)

    var stu__proto__proto = Object.getPrototypeOf(stu__proto)
    assert(stu__proto__proto, Person.prototype)

    var stu__proto__proto__proto = Object.getPrototypeOf(stu__proto__proto)
    assert(stu__proto__proto__proto, Object.prototype)
}

function testMicroTask(assert, done) {
    var s = ''
    function addChar(char) {
        s = s + char
    }
    process.nextTick(addChar, '1')
    addChar('2')
    addChar('3')
    assert(s, '23')

    process.nextTick(function () {
        assert(s, '231')
        done()
    })

    return done
}

function testMacroTask(assert, done) {
    var s = ''
    function addChar(char) {
        s = s + char
    }
    setTimeout(addChar, 0, '1')
    addChar('2')
    addChar('3')
    assert(s, '23')

    setTimeout(function () {
        assert(s, '231')
        done()
    })

    return done
}

function testMicroTaskAndMacroTask(assert, done) {
    var s = ''
    function addChar(char) {
        s = s + char
    }

    // enqueue macro task
    setTimeout(addChar, 0, '1')
    // enqueue micro task
    process.nextTick(addChar, '2')
    addChar('3')
    addChar('4')

    assert(s, '34')
    process.nextTick(function () {
        assert(s, '342')
    })
    setTimeout(function () {
        assert(s, '3421')
        done()
    })

    return done
}

function testPromiseLike(assert, done) {

    // simple Promise definition
    function Promise(executor) {
        var _this = this
        function PromiseResolve(value) {
            if (_this.state === 'PENDING') {
                _this.state = 'FULFILLED'
                _this.value = value

                process.nextTick(function (value) {
                    if (_this.onFulfilledCallback) {
                        _this.onFulfilledCallback(value)
                    }
                }, value)
            }
        }
        function PromiseReject(reason) {
            if (_this.state === 'PENDING') {
                _this.state = 'REJECTED'
                _this.value = reason

                process.nextTick(function (reason) {
                    if (_this.onRejectedCallback)
                        _this.onRejectedCallback(reason)
                }, reason)
            }
        }

        this.state = 'PENDING'
        this.value = null
        this.onFulfilledCallback = null
        this.onRejectedCallback = null

        executor(PromiseResolve, PromiseReject)
    }

    Promise.prototype.then = function (onFulfilled, onRejected) {
        var p1 = this
        p1.onFulfilledCallback = onFulfilled
        p1.onRejectedCallback = onRejected
    }


    // simple Promise use case
    var sequence = ''
    var p1 = new Promise(function (resolve, reject) {
        process.nextTick(resolve, 'success')
    })
    p1.then(function (value) {
        assert(value, 'success')
        sequence = sequence + '-p1success-'
    }, function (reason) {
        assert(reason, 'fail')
    })

    var p2 = new Promise(function (resolve, reject) {
        reject('fail')
    })
    p2.then(function (value) {
        assert(value, 'success')
    }, function (reason) {
        sequence = sequence + '-p2fail-'
        assert(reason, 'fail')
    })

    setTimeout(function () {
        assert(sequence, "-p2fail--p1success-")
        done()
    })

    return done
}

function testCallAndBind(assert) {
    var obj = new Object()
    obj.greeting = 'hi'
    function fn1(s) {
        return this === obj && this.greeting + s
    }
    assert(fn1('zx'), false)
    assert(fn1.call(obj, 'zx'), 'hizx')

    function fn2(s1, s2) {
        return this === obj && this.greeting + s1 + s2
    }
    var binded = fn2.bind(obj, 'z')
    assert(fn2('x'), false)
    assert(binded('x'), 'hizx')
}

function testArray(assert) {
    var arr = new Array();
    assert(arr.length, 0)

    arr.push(100)
    assert(arr.length, 1)
    assert(arr[0], 100)

    arr.push(true)
    assert(arr.length, 2)
    assert(arr[1], true)

    arr.push('s', undefined, null, new Object(), new Array());
    assert(arr.length, 7)

    arr.pop()
    arr.pop()
    var _null = arr.pop();
    assert(_null, null)
    var _unde = arr.pop();
    assert(_unde, undefined)
    var s = arr.pop()
    assert(s, 's')
    assert(arr.length, 2)

    var arr1 = new Array(10)
    assert(arr1.length, 10)
    arr1.push(1)
    assert(arr1.length, 11)
    var num = arr1.pop()
    assert(num, 1)
    assert(arr1.length, 10)
}

describe('testType', testType)
describe('testOperator', testOperator)
describe('testClosure', testClosure)
describe('testHighOrderFunction', testHighOrderFunction)
describe('testConditional', testConditional)
describe('testIteration', testIteration)
describe('testObject', testObject)
describe('testArray', testArray)
describe('testMicroTask', testMicroTask)
describe('testMacroTask', testMacroTask)
describe('testMicroTaskAndMacroTask', testMicroTaskAndMacroTask)
describe('testPromiseLike', testPromiseLike)
describe('testCallAndBind', testCallAndBind)

