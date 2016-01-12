function foo() {
    var n = 999;
    nAdd = function() {n += 1;};
    function bar() {
        return n;
    }
    return bar;
}

res = foo();
var a = res();
nAdd();
var b = res();
result = a == 999 && b == 1000;

