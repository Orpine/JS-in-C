a={
  d:5,
  e:function(i){
      if(i==0) {
          return 1;
      }
      else {
          return i * a.e(i - 1);
      }
  }
};
result1 = a.d;
result= a.e(5);