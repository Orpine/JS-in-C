a=1;
function f1(){
    var n=999;
    nAdd=function(){n+=1;};
    function f2() {
        return n;
    };
    return f2;
}
var res=f1();
result = res();// == 1000;

