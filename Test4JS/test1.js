a={
  e:function(i){
      if(i==0) {
          return 1;
      }
      else {
          return i * a.e(i - 1);
      }
  }
};
result= a.e(5);