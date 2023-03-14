# eco
Ecologically clean coroutines. (whatever it means)

I need it for my other project ([ether](https://github.com/pidhii/ether)). For this reason I wasnt bothering about portability and etc.
Many thanks to [hnes](https://github.com/hnes) with his [libaco](https://github.com/hnes/libaco).
His library does not satisfy me, but 90% of my code are "strongly influenced by him" (copy-pasting took place).

Anyway. **Eco** is a minimalistic library that allows you to switch between different stack+regsiters context at will and in arbitrary order.
I.e. stuff like
  A -> B -> C -> A -> B -> A
is totaly fine.

In **eco** each coroutine onws a personal private stack. Probably it doesn't fit some needs, but I dont need shared stacks for my needs.
Copying of a state is not implemented. It is commont to carry around pointers to the variables allocated on stack, but it prevents you from reallocation of a stack (unless you have complete controll over everything inside your coroutines).

The general picture/functionality is the following:
- To switch into another coroutine you use `eco_switch(<from>, <to>, ...)`. The `<from>` argument may seem redundant (you always put yourself there), but I dont like global variables, so youll have to carry your identity yourself.
- You can pass a value (any 64-bit blob) to the coroutine you are switching to, and, similarly, others can send some presents when switching to you.
- Together with a 64-bit-present you can see who swiped you right in that `eco_switch()` ;\*.

Besides `eco_switch()` the rest of the functions are trivial: initialization of a coroutine environment and allocation of a stack.
Take a look into the "eco.h" (it is only ~50 lines of code).
And you can check out a sample program: "t.c".

Feel free to fork and change whatever you want. I havent left too many comments, but I believe the code is clear.
