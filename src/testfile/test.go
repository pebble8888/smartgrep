package main

import "exp/eval"

func main() {
world := eval.NewWorld();
	   code, err := world.Compile(`
			   print("Hello\n");
			   `); // does not supoort package

	   if err != nil {
		   panic(err.String(), "\n");
	   } else {
		   /* Hello */
		   if value, err := code.Run(); err != nil {
			   panicln(err.String());
		   } else println(value);
		   // Hello
	   }
}
