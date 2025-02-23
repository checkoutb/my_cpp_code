
extern "C" {    
    void fun_c();
}

/*
.

`gcc -c be_invoked_c.c`
`g++ main.cpp be_invoked_c.o`
`./a.out`    ok


*/    
int main() {
    fun_c();
}
