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
      b.push("/* load: "+fn+" */");
      ft=read(fn).split("\n");
      while(ft.length>0){
        f.push(ft.pop());
      }
    }
    continue;
  }
  t=eat_whitespace(l);
  t2=t.slice(0,6);
  if(t2==="print("){
    continue;
  }
  t2=t.slice(0,3);
  if(t2==="// ") {
    continue;
  }
  b.push(l);
}

// define globals:
for(var i=0;i<g.length;i++){
  c=g[i];
  print(c);
  gd=[];
}

// rest of prog
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
