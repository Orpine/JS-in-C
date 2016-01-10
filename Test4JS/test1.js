// simple function scoping test
// fail code
//case 7
//var a = 7;
//function add(x,y) { var a=x+y; return a; }
//result = add(3,6)==9 && a==7;

// case 8
//var bob = {};
//bob.add = function(x,y) { return x+y; };
//
//result = bob.add(3,6)==9;

// case 14
//x = 0; // A global variable
//var y = 'Hello!'; // Another global variable
//z = 0; // yet another global variable
//
//function f(){
//    var z = 'foxes'; // A local variable
//    twenty = 20; // Global because keyword var is not used
//    return x; // We can use x here because it is global
//}
//// The value of z is no longer available
//
//
//// testing
//blah = f();
//result = blah==0 && z!='foxes' && twenty==20;



// references for arrays

var a = [];
a[0] = 10;
a[1] = 22;

b = a;

b[0] = 5;

result = a[0]==5;// && a[1]==22 && b[1]==22;
