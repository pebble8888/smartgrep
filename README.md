smartgrep
=======
Grep word in source files excluding comment for vim plugin.  
Grep is very fast because C compiled binary is used.  
Auto project detection using .git or .hg folder.  

Support OS
=======
Unix, MacOS X, Windows

Suport Language and Comment Type
=======
C, C++, Objective-C, C#, Java Script, Java, Scala, Go   /* */, //, #if 0  
Perl        #, =pod, =cut  
Ruby 	    #, =begin, =end  
Python      #, """, '''  
Coffee      #, ###  
Shell       #  
VB.Net,VB6  '  
   
If you would like to use it for another language, please request me or try this plugin [coolgrep.vim](https://github.com/tyru/coolgrep.vim).  
  
Usage (smartgrep.vim command)
=======
Always git and mercurial repository auto detection works.  
If you don't want to use auto detection,  
define g:smartgrep_basedir in your .vimrc file.  
    example:  
        let g:smartgrep_basedir="/develop/"  
If g:smartgrep_basedir isn't defined, the current directory is used.  
  
,g   -> recursive word grep for support file exclude comment by mouse cursored word  
,h   -> recursive word grep for h file exclude comment by mouse cursored word  
,gt  -> tabnew version for ,g  
,ht  -> tabnew version for ,h  

:R   -> recursive word grep for support file exclude comment  
:Rh  -> recursive word grep for h file exclude comment  
:Ri  -> recursive grep for supported file extensions include comment  
  
Usage (smartgrep binary)
=======
$ cd {directory-you-want-to-grep}  
$ smartgrep  
Usage: smartgrep {-e[w]|-i[w]|-h[w]} [-g] word_you_grep  
  -e[w] : recursive [word] grep for supported file extensions excluding comment  
  -i[w] : recursive [word] grep for supported file extensions including comment  
  -h[w] : recursive [word] grep for .h excluding comment  
  -g : use auto detect git or mercurial repository with the current directory.  
  support file extensions : .cpp/.c/.mm/.m/.h/.js/.coffee/.pl/.rb/.py/.java/.scala/.go  
                            .cs/.vb/.bas/.frm/.cls  

INSTALL
=======
If you are Windows user, build 'smartgrep.exe' from source with MinGW or Visual Studio 6  
and read ./plugin/smartgrep.vim file.  
If you are Mac or Unix user, build 'smartgrep' from source to type 'make + [enter]'  
and read ./plugin/smartgrep.vim file.  

SCREEN SHOT
=======
![screen shot](https://raw.github.com/pebble8888/smartgrep/master/smartgrep_screenshot.png)
