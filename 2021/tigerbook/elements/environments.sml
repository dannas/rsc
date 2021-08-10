(* We can use symbolic identifiers *)
val $ = 1 + 2 
(* But we can't mix symbolic identifiers with alphanumeric identifiers *)
val $a = 1 + 2 (* non-constructor applied to argument in pattern $ *)
(* _ are allowed in the alphanumeric set but you can't start an identifier with _ *)
val a_ = 2

val pi = 3.14159
val radius = 4.0
pi * radius * radius
val area = pi * radius * radius
val radius = 5.0

val The7Dwarves = 7		
val 7Dwarfes = 7	    (* ERR starts with digit *)
val SevenDwarves,The = 7    (* ERR , is reserved *)
val a<=b = 7 		    (* ERR mixing symbolic and alphanumeric id *)
val hurrah! = 7             (* ERR mixing symbolic and alphanumeric id *)
val #1 = 7		    (* ERR mixing *)
val '123 = 7 	            (* ERR ' only for type vals *)

val a = 3
val b = 98.6
val a = "three"
val c = a ^ str (chr(floor(b)))