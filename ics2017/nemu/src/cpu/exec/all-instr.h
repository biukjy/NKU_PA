#include "cpu/exec.h"


//prefix.c
make_EHelper(operand_size);


//special.c
make_EHelper(inv);
make_EHelper(nemu_trap);


//system.c



//control.c
make_EHelper(call);
make_EHelper(ret);



//arith.c
make_EHelper(sub);


//logic.c
make_EHelper(xor);


//data-mov.c
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);




