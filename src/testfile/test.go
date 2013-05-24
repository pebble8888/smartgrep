package main

import "exp/eval"

func main() {
world := eval.NewWorld();
	   code, err := world.Compile(`
			   print("go_hello\n");
			   `); // does not supoort package

	   if err != nil {
		   panic(err.String(), "\n");
	   } else {
		   /* go_hello */
		   if value, err := code.Run(); err != nil {
			   panicln(err.String());
		   } else println(value);
		   // Hello
	   }
}
