//// test for array contains fail
//var a = [1,2,4,5,7];
//var b = ["bread","cheese","sandwich"];
//
//result = a.contains(1) && !a.contains(42) && b.contains("cheese") && !b.contains("eggs");

//var a = [1,2,4,5,7];
//
//a.remove(2);
//a.remove(5);
//
//result = a.length==3 && a[0]==1 && a[1]==4 && a[2]==7;


//// test for array join failed
//var a = [1,2,4,5,7];
//
//result = a.join(",")=="1,2,4,5,7";


//// test for string split failed
//var b = "1,4,7";
//var a = b.split(",");
//
//result = a.length==3 && a[0]==1 && a[1]==4 && a[2]==7;

//result = (true?3:4)==3 && (false?5:6)==6;

//function a() {
//    var a;
//    a = 1;
//    function b() {
//        var b;
//        b = 1;
//        return b;
//    }
//    return b();
//}

//var i;
//for (i = 0; i < 1000000; i++) {
//
//}

var Foo = {
    value : function() { return this.x + this.y; }
};

// var a = { prototype: Foo, x: 1, y: 2 };
var b = new Foo();
b.x = 2;
b.y = 3;

// var result1 = a.value();
var result2 = b.value();
result = result2==5;


function Person(name) {
    this.name = name;
    this.kill = function() { this.name += " is dead"; };
}

var a = new Person("Kenny");
a.kill();
result = a.name == "Kenny is dead";

