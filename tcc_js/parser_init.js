var twochar_str=0;

function get_twochar_str(){
    if(twochar_str){
        return twochar_str;
    }
    print("init twochar_str");
    twochar_str = mk_c_string("<=\236>=\235!=\225&&\240||\241++\244--\242==\224<<\1>>\2+=\253-=\255*=\252/=\257%=\245&=\246^=\336|=\374->\247..\250##\266");
    var t;
    t=malloc(256);
    print("hex dump 1");hd(twochar_str,256);
    var o=t;
    print("hex dump 2");hd(t,256);
    return twochar_str;
}
