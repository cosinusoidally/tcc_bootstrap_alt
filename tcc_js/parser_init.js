var twochar_str=0;

function wsb(o,s,b){
    var l;
    l=v_strlen(s);
    v_strcpy(o,s);
    wi8(o+l,b);
    return o+l+1;
}

function get_twochar_str(){
    var t;
    var o;
    if(twochar_str){
        return twochar_str;
    }
    print("init twochar_str");
// both M2 and cc_x86 can't handle this string
/*
    twochar_str = mk_c_string("<=\236>=\235!=\225&&\240||\241++\244--\242==\224<<\1>>\2+=\253-=\255*=\252/=\257%=\245&=\246^=\336|=\374->\247..\250##\266");
*/
    t=v_malloc(256);
//    print("hex dump 1");hd(twochar_str,256);
    o=t;
//  <=\236
    o=wsb(o,mk_c_string("<="),158);
//  >=\235
    o=wsb(o,mk_c_string(">="),157);
//  !=\225
    o=wsb(o,mk_c_string("!="),149);
//  &&\240
    o=wsb(o,mk_c_string("&&"),160);
//  ||\241
    o=wsb(o,mk_c_string("||"),161);
//  ++\244
    o=wsb(o,mk_c_string("++"),164);
//  --\242
    o=wsb(o,mk_c_string("--"),162);
//  ==\224
    o=wsb(o,mk_c_string("=="),148);
//  <<\1
    o=wsb(o,mk_c_string("<<"),1);
//  >>\2
    o=wsb(o,mk_c_string(">>"),2);
//  +=\253
    o=wsb(o,mk_c_string("+="),171);
//  -=\255
    o=wsb(o,mk_c_string("-="),173);
//  *=\252
    o=wsb(o,mk_c_string("*="),170);
//  /=\257
    o=wsb(o,mk_c_string("/="),175);
//  %=\245
    o=wsb(o,mk_c_string("%="),165);
//  &=\246
    o=wsb(o,mk_c_string("&="),166);
//  ^=\336
    o=wsb(o,mk_c_string("^="),222);
//  |=\374
    o=wsb(o,mk_c_string("|="),252);
//  ->\247
    o=wsb(o,mk_c_string("->"),167);
//  ..\250
    o=wsb(o,mk_c_string(".."),168);
//  ##\266
    o=wsb(o,mk_c_string("##"),182);
//    print("hex dump 2");hd(t,256);
    o=wsb(o,mk_c_string("<="),158);
    twochar_str = t;
    return twochar_str;
}

var tok_list_str=0;

function get_tok_list_str(){
    var t;
    var o;
    if(tok_list_str){
        return tok_list_str;
    }
// cc_x86 can't handle this string
/*
    tok_list_str=mk_c_string("int\0void\0char\0if\0else\0while\0break\0return\0for\0extern\0static\0unsigned\0goto\0do\0continue\0switch\0case\0const\0volatile\0long\0register\0signed\0auto\0inline\0restrict\0float\0double\0_Bool\0short\0struct\0union\0typedef\0default\0enum\0sizeof\0define\0include\0ifdef\0ifndef\0elif\0endif\0defined\0undef\0error\0line\0__LINE__\0__FILE__\0__DATE__\0__TIME__\0__VA_ARGS__\0__func__\0main\0");
*/
//    print("hex dump 1");hd(tok_list_str,512);
    t=v_malloc(512);
    o=t;
    o=wsb(o,mk_c_string("int"),0);
    o=wsb(o,mk_c_string("void"),0);
    o=wsb(o,mk_c_string("char"),0);
    o=wsb(o,mk_c_string("if"),0);
    o=wsb(o,mk_c_string("else"),0);
    o=wsb(o,mk_c_string("while"),0);
    o=wsb(o,mk_c_string("break"),0);
    o=wsb(o,mk_c_string("return"),0);
    o=wsb(o,mk_c_string("for"),0);
    o=wsb(o,mk_c_string("extern"),0);
    o=wsb(o,mk_c_string("static"),0);
    o=wsb(o,mk_c_string("unsigned"),0);
    o=wsb(o,mk_c_string("goto"),0);
    o=wsb(o,mk_c_string("do"),0);
    o=wsb(o,mk_c_string("continue"),0);
    o=wsb(o,mk_c_string("switch"),0);
    o=wsb(o,mk_c_string("case"),0);
    o=wsb(o,mk_c_string("const"),0);
    o=wsb(o,mk_c_string("volatile"),0);
    o=wsb(o,mk_c_string("long"),0);
    o=wsb(o,mk_c_string("register"),0);
    o=wsb(o,mk_c_string("signed"),0);
    o=wsb(o,mk_c_string("auto"),0);
    o=wsb(o,mk_c_string("inline"),0);
    o=wsb(o,mk_c_string("restrict"),0);
    o=wsb(o,mk_c_string("float"),0);
    o=wsb(o,mk_c_string("double"),0);
    o=wsb(o,mk_c_string("_Bool"),0);
    o=wsb(o,mk_c_string("short"),0);
    o=wsb(o,mk_c_string("struct"),0);
    o=wsb(o,mk_c_string("union"),0);
    o=wsb(o,mk_c_string("typedef"),0);
    o=wsb(o,mk_c_string("default"),0);
    o=wsb(o,mk_c_string("enum"),0);
    o=wsb(o,mk_c_string("sizeof"),0);
    o=wsb(o,mk_c_string("define"),0);
    o=wsb(o,mk_c_string("include"),0);
    o=wsb(o,mk_c_string("ifdef"),0);
    o=wsb(o,mk_c_string("ifndef"),0);
    o=wsb(o,mk_c_string("elif"),0);
    o=wsb(o,mk_c_string("endif"),0);
    o=wsb(o,mk_c_string("defined"),0);
    o=wsb(o,mk_c_string("undef"),0);
    o=wsb(o,mk_c_string("error"),0);
    o=wsb(o,mk_c_string("line"),0);
    o=wsb(o,mk_c_string("__LINE__"),0);
    o=wsb(o,mk_c_string("__FILE__"),0);
    o=wsb(o,mk_c_string("__DATE__"),0);
    o=wsb(o,mk_c_string("__TIME__"),0);
    o=wsb(o,mk_c_string("__VA_ARGS__"),0);
    o=wsb(o,mk_c_string("__func__"),0);
    o=wsb(o,mk_c_string("main"),0);

//    print("hex dump 2");hd(t,512);
    tok_list_str=t;
    return tok_list_str;
}
