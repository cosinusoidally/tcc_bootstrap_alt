var twochar_str=0;

function wsb(o,s,b){
    var l;
    l=strlen(s);
    strcpy(o,s);
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
/*
    twochar_str = mk_c_string("<=\236>=\235!=\225&&\240||\241++\244--\242==\224<<\1>>\2+=\253-=\255*=\252/=\257%=\245&=\246^=\336|=\374->\247..\250##\266");
*/
    t=malloc(256);
//    print("hex dump 1");hd(twochar_str,256);
    o=t;
//  <=\236
    o=wsb(o,mk_c_string("<="),0236);
//  >=\235
    o=wsb(o,mk_c_string(">="),0235);
//  !=\225
    o=wsb(o,mk_c_string("!="),0225);
//  &&\240
    o=wsb(o,mk_c_string("&&"),0240);
//  ||\241
    o=wsb(o,mk_c_string("||"),0241);
//  ++\244
    o=wsb(o,mk_c_string("++"),0244);
//  --\242
    o=wsb(o,mk_c_string("--"),0242);
//  ==\224
    o=wsb(o,mk_c_string("=="),0224);
//  <<\1
    o=wsb(o,mk_c_string("<<"),1);
//  >>\2
    o=wsb(o,mk_c_string(">>"),2);
//  +=\253
    o=wsb(o,mk_c_string("+="),0253);
//  -=\255
    o=wsb(o,mk_c_string("-="),0255);
//  *=\252
    o=wsb(o,mk_c_string("*="),0252);
//  /=\257
    o=wsb(o,mk_c_string("/="),0257);
//  %=\245
    o=wsb(o,mk_c_string("%="),0245);
//  &=\246
    o=wsb(o,mk_c_string("&="),0246);
//  ^=\336
    o=wsb(o,mk_c_string("^="),0336);
//  |=\374
    o=wsb(o,mk_c_string("|="),0374);
//  ->\247
    o=wsb(o,mk_c_string("->"),0247);
//  ..\250
    o=wsb(o,mk_c_string(".."),0250);
//  ##\266
    o=wsb(o,mk_c_string("##"),0266);
//    print("hex dump 2");hd(t,256);
    o=wsb(o,mk_c_string("<="),0236);
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
    tok_list_str=mk_c_string("int\0void\0char\0if\0else\0while\0break\0return\0for\0extern\0static\0unsigned\0goto\0do\0continue\0switch\0case\0const\0volatile\0long\0register\0signed\0auto\0inline\0restrict\0float\0double\0_Bool\0short\0struct\0union\0typedef\0default\0enum\0sizeof\0define\0include\0ifdef\0ifndef\0elif\0endif\0defined\0undef\0error\0line\0__LINE__\0__FILE__\0__DATE__\0__TIME__\0__VA_ARGS__\0__func__\0main\0");
    print("hex dump 1");hd(tok_list_str,512);
    t=malloc(512);
    o=t;
    print("hex dump 2");hd(t,512);
    return tok_list_str;
}
