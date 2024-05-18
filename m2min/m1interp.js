print("hello world");
a=read("artifacts/M2_simple_asm-0.hex2");

a=a.split("\n");

labels={};

relp = [];
absp = [];

heap = [];

function append_hex(s) {
  print(s);
}

for(var i=0;i<a.length;i++){
  var l;
  var l0;
  l=a[i];
  l0=l[0];
  if(l0===":"){
    labels[l.split(":")[1]] = i;
  } else if(l0==="%"){
    relp.push({name: l.split("%")[1], line: i});
    append_hex("DEADBEEF");
  } else if(l0==="&"){
    absp.push({name: l.split("&")[1], line: i});
    append_hex("DEADBEEF");
  } else {
    if(l.length >0) {
      append_hex(l);
    }
  }
}
