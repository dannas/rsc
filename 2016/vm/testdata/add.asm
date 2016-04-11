.def start args=0, locals=0
   iconst 1
   iconst 2
   call .hello
   print
   halt
.def hello args=2, locals=0
   load -2
   load -3
   iadd
   ret
