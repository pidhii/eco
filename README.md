# eco
Ecologically clean coroutines. (whatever it means)

I need it for my other project ([ether](https://github.com/pidhii/ether)). For this reason I wasnt bothering about portability and etc.
Many thanks to [hnes](https://github.com/hnes) with his [libaco](https://github.com/hnes/libaco).
His library does not satisfy me, but 90% of my code are "strongly influenced by him" (copy-pasting took place).

Anyway. **Eco** is a minimalistic library that allows you to switch between different stack+regsiters context at will and in arbitrary order.
I.e. stuff like
  A -> B -> C -> A -> B -> A
is totaly fine.

The general picture/functionality is the following:
- To switch into another coroutine you use `eco_switch(<from>, <to>, ...)`. The `<from>` argument may seem redundant (you always put yourself there), but I dont like global variables, so youll have to carry your identity yourself.
- You can pass a value (any 64-bit blob) to the coroutine you are switching to, and, similarly, others can send some presents when switching to you.
- Together with a 64-bit-present you can see who swiped you right in that `eco_switch()` ;\*.
- When/if coroutine returns it will automatically switch to the last caller. This behaviour may lead to unwanted consequences (continue reading), thus there is a possibility to add custom return handle (see `eco_t` in "eco.h").  

Besides `eco_switch()` the rest of the functions are trivial: initialization of a coroutine environment and allocation of a stack.
Take a look into the "eco.h" (it is only ~50 lines of code).
And you can check out a sample program: "t.c".

In **eco** each coroutine onws a personal private stack. Probably it doesn't fit some needs, but I dont need shared stacks for my needs.
Copying of a state is not implemented. It is commont to carry around pointers to the variables allocated on stack, but it prevents you from reallocation of a stack (unless you have complete controll over everything inside your coroutines).

**Note**: while arbitrary sequencing of switches between coroutines is allowed, it is your responcibility to avoid any funny positions you may run your program into. Consider a trivial setup:

     1| main:
     2|   switch-to (foo)
     3|   ...
     4| 
     5| foo:
     6|   switch-to (bar)
     7|   return
     8| 
     9| bar:
    10|   switch-to (foo)
    11|   return
    
Lines of this code would be executed in the following order:

    1,2 --> 5,6 --> 9,10 --> 6,7 --> 10,11 --> 7 --> 11 --> 7 --> 11 ...

Feel free to fork and change whatever you want. I havent left too many comments, but I believe the code is clear.
