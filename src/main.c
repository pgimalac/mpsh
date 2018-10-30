#include "stdio.h"
#include "regexp.h"

int main (void) {
    state s1, s2, s3;

    for (int i = 0; i < 256; i++) {
	s1.nexts[i] = 0;
	s2.nexts[i] = 0;
	s3.nexts[i] = 0;
    }
    
    s1.is_terminal = 0;
    s1.nexts['a'] = &s2;

    s2.is_terminal = 1;
    s2.nexts['b'] = &s3;

    s3.is_terminal = 0;
    s3.nexts['a'] = &s2;

    printf("%d\n", match("a", &s1));
    printf("%d\n", match("ababa", &s1));
    printf("%d\n", match("ab", &s1));
    
    return 0;
}
