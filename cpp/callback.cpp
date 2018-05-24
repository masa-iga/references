#include <cstdio>
using namespace std;

typedef void (* FUNC_POINTER)(const char *);

void callback1(const char *s)
{
    printf("%s: %s\n", __func__, s);
}


void callback2(const char *s)
{
    printf("%s: [%s]\n", __func__, s);
}


void func(const char *s, FUNC_POINTER p)
{
    p(s);
}


int main()
{
    FUNC_POINTER p;

    p = callback1;
    func("call callback1()", p);

    p = callback2;
    func("call callback2()", p);

    return 0;
}
