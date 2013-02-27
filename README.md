smartgrep
=======
Grep word in source file excluding comment for vim plugin.

Support OS
=======
Unix, MacOS X, Windows

Suport Language and Comment Type
=======
C, C++, Objective-C, C#(CSharp), Java Script   /* */, //, #if 0  
  
If you would like to use it for another language, please request me or try this plugin [coolgrep.vim](https://github.com/tyru/coolgrep.vim).  
  
Usage (smartgrep.vim command)
=======
Define g:smartgrep_basedir in your .vimrc file.  
	example:  
		let g:smartgrep_basedir="/develop/"  
If g:smartgrep_basedir isn't defined, the current directory is used.  
  
,g   -> recursive word grep for c,h file exclude comment by mouse cursored word  
,h   -> recursive word grep for h file exclude comment by mouse cursored word  
,gt  -> tabnew version for ,g  
,ht  -> tabnew version for ,h  
:Rn  -> recursive word grep for c,h file include comment  
:Rh  -> recursive word grep for h file exclude comment  
:R   -> recursive word grep for c,h file exclude comment  
:Rno -> recursive grep for c,h file include comment  
:Rho -> recursive grep for h file exclude comment  
:Ro  -> recursive grep for c,h file exclude comment  
:Rl  -> recursive word grep for c,h file exclude comment in sys_dir_w  
:Rw  -> recursive word grep for c,h file exclude comment in sys_dir_l  

Usage (smartgrep binary)
=======
$ cd {directory-you-want-to-grep}  
$ smartgrep  
Usage: smartgrep -h {word}  : recursive      grep for .h                        excluding comment  
                 -b {word}  : recursive      grep for .cpp .c .mm .m .cs .js .h excluding comment  
                 -n {word}  : recursive      grep for .cpp .c .mm .m .cs .js .h including comment  
                 -hw {word} : recursive word grep for .h                        excluding comment  
                 -bw {word} : recursive word grep for .cpp .c .mm .m .cs .js .h excluding comment  
                 -nw {word} : recursive word grep for .cpp .c .mm .m .cs .js .h including comment  

INSTALL
=======
If you are Windows user, build 'smartgrep.exe' from source with MinGW or Visual Studio 6  
and read ./plugin/smartgrep.vim file.  
If you are Mac or Unix user, build 'smartgrep' from source to type 'make + [enter]'  
and read ./plugin/smartgrep.vim file.  

SCREEN SHOT
=======
![screen shot](https://github.com/pebble8888/smartgrep/smartgrep_screenshot.png)
