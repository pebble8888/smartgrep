smartgrep
=======
Grep word in source file excluding comment for vim plugin.

Cover OS
=======
Unix, MacOS X, Windows

Cover Language and Comment Type
=======
C, C++, Objective-C, C#(CSharp), Java Script   /* */, //

Usage
=======
$ cd {directory-you-want-to-grep}  
$ smartgrep  
Usage: smartgrep /h {word}  : recursive      grep for .h                        excluding comment  
                 /b {word}  : recursive      grep for .cpp .c .mm .m .cs .js .h excluding comment  
                 /n {word}  : recursive      grep for .cpp .c .mm .m .cs .js .h including comment  
                 /hw {word} : recursive word grep for .h                        excluding comment  
                 /bw {word} : recursive word grep for .cpp .c .mm .m .cs .js .h excluding comment  
                 /nw {word} : recursive word grep for .cpp .c .mm .m .cs .js .h including comment  

INSTALL
=======
If you are Windows or Mac OS X user, read ./plugin/smartgrep.vim file.  
If you are Unix user, type 'make + [enter]' and read ./plugin/smartgrep.vim file.  

