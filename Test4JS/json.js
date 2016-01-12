var foo = {
    add: function (x, y) {
        return x + y;
    },
    bar: 9
};

result = foo.add(3, 6) == 9 && foo.bar == 9;
