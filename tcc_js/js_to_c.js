f=read("tcc.js").split("\n").reverse();
g=[];
while(f.length>0){
  l=f.pop();
  t=l.slice(0,3);
  if(t==="// ") {
    continue;
  }
  if(t==="var "){
    g.push(l);
    continue;
  }
  
  print(l);
}

for(var i=0;i<g.length;g++){
  print(g[i]);
}
