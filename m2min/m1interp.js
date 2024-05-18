print("hello world");
a=read("artifacts/M2_simple_asm-0.hex2");

a=a.split("\n");

labels={};

for(var i=0;i<a.length;i++){
  var l;
  l=a[i];
  if(l[0]===":"){
    labels[l.split(":")[1]] = i;
  } else {

  }
}
