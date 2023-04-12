#include "cpu/exec.h"


//prefix.c
make_EHelper(operand_size);


//special.c
make_EHelper(inv);
make_EHelper(nemu_trap);
make_EHelper(nop);


//system.c
make_EHelper(in);
make_EHelper(out);


//control.c
make_EHelper(call);
make_EHelper(ret);

make_EHelper(jmp);
make_EHelper(jmp_rm);
make_EHelper(jcc);
make_EHelper(call_rm);


//arith.c
make_EHelper(sub);

make_EHelper(add);
make_EHelper(adc);
make_EHelper(cmp);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(sbb);
make_EHelper(div);
make_EHelper(idiv);
make_EHelper(neg);

//logic.c
make_EHelper(xor);

make_EHelper(and);
make_EHelper(or);
make_EHelper(not);
make_EHelper(setcc);
make_EHelper(test);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(rol);

//data-mov.c
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);

make_EHelper(lea);
make_EHelper(leave);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper(pusha);
make_EHelper(popa);



