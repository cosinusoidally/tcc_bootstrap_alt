var twochar_str=0;

function wsb(o,s,b){
    var l;
    l=strlen(s);
    strcpy(o,s);
    wi8(o+l,b);
    return o+l+1;
}

function get_twochar_str(){
    if(twochar_str){
        return twochar_str;
    }
    print("init twochar_str");
/*
    twochar_str = mk_c_string("<=\236>=\235!=\225&&\240||\241++\244--\242==\224<<\1>>\2+=\253-=\255*=\252/=\257%=\245&=\246^=\336|=\374->\247..\250##\266");
*/
    var t;
    t=malloc(256);
    print("hex dump 1");hd(twochar_str,256);
    var o=t;
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
    print("hex dump 2");hd(t,256);
    o=wsb(o,mk_c_string("<="),0236);
    twochar_str = t;
    return twochar_str;
}

tok_list_str=0;
