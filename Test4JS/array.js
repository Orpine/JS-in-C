var i;
var a = [];
for (i = 1; i <= 10; i++) {
    a[i] = i * i;
}

var result = 0;
for (i = 1; i <= 10; i++) {
    result += a[i];
}

result += a.length;
