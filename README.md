smartgrep
=======
Grep word in source file excluding comment for vim plugin.  
Grep is very fast because C compiled binary is used.  
Auto project detection using .git or .hg folder.  

Support OS
=======
Unix, MacOS X, Windows

Suport Language and Comment Type
=======
C, C++, Objective-C, C#(CSharp), Java Script, Java, Scala, Go   /* */, //, #if 0  
Ruby 	#  
Python  #, """  
Coffee  #, ###  
  
If you would like to use it for another language, please request me or try this plugin [coolgrep.vim](https://github.com/tyru/coolgrep.vim).  
  
Usage (smartgrep.vim command)
=======
Define g:smartgrep_basedir in your .vimrc file.  
	example:  
		let g:smartgrep_basedir="/develop/"  
If g:smartgrep_basedir isn't defined, the current directory is used.  
Always git and mercurial repository auto detection works.  
  
,g   -> recursive word grep for support file exclude comment by mouse cursored word  
,h   -> recursive word grep for h file exclude comment by mouse cursored word  
,gt  -> tabnew version for ,g  
,ht  -> tabnew version for ,h  

:R   -> recursive word grep for support file exclude comment  
:Rh  -> recursive word grep for h file exclude comment  
:Ri  -> recursive grep for c,h file include comment  
  
:Rl  -> recursive word grep for c,h file exclude comment in sys_dir_w  
:Rw  -> recursive word grep for c,h file exclude comment in sys_dir_l  

Usage (smartgrep binary)
=======
$ cd {directory-you-want-to-grep}  
$ smartgrep  
Usage: smartgrep {-e[w]|-i[w]|-h[w]} [-g] word_you_grep  
  -e[w] : recursive [word] grep for supported file extensions excluding comment  
  -i[w] : recursive [word] grep for supported file extensions including comment  
  -h[w] : recursive [word] grep for .h excluding comment  
  -g : use auto detect git or mercurial repository with the current directory.  
  support file extensions : .cpp/.c/.mm/.m/.h/.cs/.js/.coffee/.rb/.py/.java/.scala/.go  


INSTALL
=======
If you are Windows user, build 'smartgrep.exe' from source with MinGW or Visual Studio 6  
and read ./plugin/smartgrep.vim file.  
If you are Mac or Unix user, build 'smartgrep' from source to type 'make + [enter]'  
and read ./plugin/smartgrep.vim file.  

SCREEN SHOT
=======
![screen shot](https://raw.github.com/pebble8888/smartgrep/master/smartgrep_screenshot.png)
