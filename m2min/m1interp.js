print("hello world");
a=read("artifacts/M2_simple_asm-0.hex2");

a=a.split("\n");

labels={};

relp = [];
absp = [];

for(var i=0;i<a.length;i++){
  var l;
  var l0;
  l=a[i];
  l0=l[0];
  if(l0===":"){
    labels[l.split(":")[1]] = i;
  } else if(l0==="%"){
    relp.push([[l.split("%")[1]], i]);
  } else if(l0==="&"){
    absp.push([[l.split("&")[1]], i]);
  } else {
    if(l.length >0) {
      print("hex: "+l);
    }
  }
}
