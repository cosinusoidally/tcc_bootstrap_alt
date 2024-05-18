print("hello world");
a=read("artifacts/M2_simple_asm-0.hex2");

a=a.split("\n");

labels={};

relp = [];
absp = [];

hex_frags = [];

heap = [];

ho = 0;

function append_hex(s) {
  var v;
  print(s);
  s=s.split("");
  for(var i=0; i<s.length; i=i+2){
    v=[];
    v.push("0x");
    v.push(s[i]);
    v.push(s[i+1]);
    v=v.join("");
    print(v);
  }
}

for(var i=0;i<a.length;i++){
  var l;
  var l0;
  l=a[i];
  l0=l[0];
  if(l0===":"){
    labels[l.split(":")[1]] = {line: i, heap_offset: ho};
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
