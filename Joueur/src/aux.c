#include "aux.h"
void PrependZeros(char *dest, const char *src, unsigned minimal_width) {
  size_t len = strlen(src);
  size_t zeros = (len > minimal_width) ? 0 : minimal_width - len;
  memset(dest, '0', zeros);
  strcpy(dest + zeros, src);
}

char * remove_hashtags(char*src)
{   
    int cpt=0;
    for (int i = 0; i < 15; i++)
    {
        if (src[i]=='#')
        {
            cpt++;
        }
        
    }
    int ip_len=strlen(src)-cpt;
    char * d=malloc(ip_len);
    strncpy(d,src,ip_len);
    return d;

    

}
