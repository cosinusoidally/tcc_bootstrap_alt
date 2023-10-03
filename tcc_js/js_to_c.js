f=read("tcc.js").split("\n").reverse();
g=[];

b=[];

while(f.length>0){
  l=f.pop();
  t=l.slice(0,3);
  if(t==="// ") {
    continue;
  }
  t=l.slice(0,4);
  if(t==="var "){
    g.push(l);
    continue;
  }
  b.push(l);
}

for(var i=0;i<g.length;i++){
  c=g[i];
  print(c);
  gd=[];
}
