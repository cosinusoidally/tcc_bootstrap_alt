typedef int (*FUNCTION) ();

extern int stdout;

int call_wrap(FUNCTION t, int a, int b){
  return t(a,b);
}

/* adapted from bootstrappable_load.c */
char* int2str(int x, int base, int signed_p)
{
        /* Be overly conservative and save space for 32binary digits and padding null */
        char* p = (char *)calloc(34, sizeof(char));
        /* if calloc fails return null to let calling code deal with it */
        if(0 == p) return p;

        p = p + 32;
        unsigned i;
        int sign_p = 0;
        char* table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        if(signed_p && (10 == base) && (0 != (x & 0x80000000)))
        {
                /* Truncate to 31bits */
                i = -x & 0x7FFFFFFF;
                if(0 == i) return "-2147483648";
                sign_p = 1;
        } /* Truncate to 32bits */
        else i = x & (0x7FFFFFFF | (1 << 31));

        do
        {
                p[0] = table[i % base];
                p = p - 1;
                i = i / base;
        } while(0 < i);

        if(sign_p)
        {
                p[0] = '-';
                p = p - 1;
        }

        return p + 1;
}

void puts_num(int x){
  printf("%u\n",x);
}
