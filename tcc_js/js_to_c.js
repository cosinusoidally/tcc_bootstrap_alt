f=read("tcc.js").split("\n").reverse();
g=[];

b=[];

while(f.length>0){
  l=f.pop();
  t=l.slice(0,4);
  if(t==="var "){
    g.push(l);
    continue;
  }
  t=l.slice(0,5);
  if(t==="load("){
    fn=l.split("\"")[1];
    if(fn!=="support.js") {
      b.push("loading: "+fn);
    }
    continue;
  }
  t=eat_whitespace(l);
  t=t.slice(0,6);
  if(t==="print("){
    continue;
  }
  t=t.slice(0,3);
  if(t==="// ") {
    continue;
  }
  b.push(l);
}

for(var i=0;i<g.length;i++){
  c=g[i];
  print(c);
  gd=[];
}

for(var i=0;i<b.length;i++){
  print(b[i]);
}

function eat_whitespace(x){
  var o=0;
  while(x[o]===" "){
    o++;
  }
  x=x.slice(o);
  return x;
}
