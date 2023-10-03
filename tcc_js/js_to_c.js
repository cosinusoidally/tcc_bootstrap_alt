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
  n=num_whitespace(l);
  t2=t.slice(0,6);
  if(t2==="print("){
    continue;
  }
  t2=t.slice(0,3);
  if(t2==="// ") {
    continue;
  }
  t2=t.slice(0,4);

  if(t2==="var "){
    t3=[];
    while(n>0){
      t3.push(" ");
      n--;
    }
    t3=t3.join("");
    l=t3+"int "+t.slice(4);
  }
  b.push(l);
}

// define globals:
gd=[];
gi=[];
for(var i=0;i<g.length;i++){
  c=g[i];
  c=c.split("=");
  gd.push(c[0]+";");
  if(c.length>1){
    gi.push(c[0].slice(4)+"="+c[1]);
  }
}

print("/* declare globals */");
for(var i=0;i<gd.length;i++){
  print("int "+gd[i].slice(4));
}
print("");

print("/* init globals */");
print("void init_globals(void) {");
for(var i=0;i<gi.length;i++){
  print("    "+gi[i]);
}
print("}");
print("");
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

function num_whitespace(x){
  var o=0;
  while(x[o]===" "){
    o++;
  }
  return o;
}
