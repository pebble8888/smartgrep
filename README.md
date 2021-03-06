
smartgrep  
=======

Grep word in source files excluding comment for vim plugin.  
Grep is very fast because C compiled binary is used.  
Auto project detection using .git or .hg folder.  

Example
=======
If C source file func.c looks like
```c
void glareAt(int human, int mage){
    /* BUGFIX
    petrify(human);
     */
    petrify(mage);
}
```
```
$ smartgrep -ew petrify
```
This command puts
```
/Users/pebble/func.c:5:    petrify(mage);
```
Support OS
=======
Unix, MacOS X, Windows

Suport Language and Comment Type
=======
C, C++, Objective-C, Swift, C#, Java Script, Java, Scala, Go   /* */, //, #if 0  
Perl        #, =pod, =cut  
Ruby 	    #, =begin, =end  
Python      #, """, '''  
Coffee      #, ###  
Shell       #  
VB.Net,VB6  '  
Vim         "

If you would like to use it for another language, please request me or try this plugin [coolgrep.vim](https://github.com/tyru/coolgrep.vim).  

Usage (smartgrep.vim command)
=======
Git and mercurial repository auto detection works by default setting.  
If you don't want to use auto detection,  
define g:smartgrep_basedir in your .vimrc file.  
    example:  
        let g:smartgrep_basedir="/develop/"  
If g:smartgrep_basedir isn't defined, the current directory is used.  

,g   -> recursive word grep for support file exclude comment by mouse cursored word  
,h   -> recursive word grep for h file exclude comment by mouse cursored word  
,i   -> recursive grep for supported files include comment  
,r   -> git grep by mouse cursored word  

:R   -> recursive word grep for support file exclude comment  
:Rh  -> recursive word grep for h file exclude comment  
:Ri  -> recursive grep for supported file extensions include comment  
:Rr  -> git grep  

Usage (smartgrep binary)
=======
$ cd {directory-you-want-to-grep}  
$ smartgrep  
Usage: smartgrep {-e[w]|-i[w]|-h[w]} [-g] [--nojs] word_you_grep  
  -e[w] : recursive [word] grep for supported file extensions excluding comment  
  -i[w] : recursive [word] grep for supported file extensions including comment  
  -h[w] : recursive [word] grep for .h excluding comment  
  -c : recursive case insensitive grep for supported file extension including comment  

 [options]  
  -g : use auto detect git or mercurial repository with the current directory  
  --nohtml : exclude .html file  
  --nojs : exclude js file  
  --ignore-dir NAME : exclude NAME folder  
  --noworker : no use worker for sequencial output

  support file extensions : .cpp/.c/.mm/.m/.h/.js/.coffee/.rb/.py/.pl/.sh/.rs/.toml  
                            .java/.scala/.go/.cs/.xaml/.resx/.vb/.bas/.frm/.cls/.pc/.cu  
                            .php/.plist/.pbxproj/.strings/.storyboard/.swift/.gyv/.vim  
                            .css/.scss  
  asis support file extensions : .erb/.html/.xml  

  ignore directory : .git/.hg/.svn/.vs  

INSTALL
=======
If you use Bundle, add 'pebble8888/smartgrep.git' to ~/.vimrc and :BundleInstall.  
Windows : build 'smartgrep.exe' from source file in smartgrep/src with MinGW or Visual Studio 6.  
MacOSX/Linux : build 'smartgrep' from source file in smartgrep/src with g++ to type 'make + [enter]'.  
So copy smartgrep binary to pathed directory.  
