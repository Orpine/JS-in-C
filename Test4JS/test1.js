function a(i){
    if (i == 0) {return 1;}
    return i * a(i - 1);
};
result = a(0);
