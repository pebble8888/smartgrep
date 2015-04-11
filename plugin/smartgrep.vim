"
" Filename : smartgrep.vim 
" Brief    : This file is vim plugin
" Function : grep source file excluding comment and wrapper for major grep command
" Author   : pebble8888@gmail.com 2009-2015 Copyright
" History  :
" 	Ver1.0.0.0 2009-09-13 initial revision for grep excluding comment
"	Ver1.1.0.0 2009-09-27 add word grep function
"	Ver1.2.0.0 2010-01-23 care .hpp file extension
"                         exclude hided directory begins dot(.)
"	Ver1.3.0.0 2010-01-30 add option designate .h and .cpp
"	Ver1.4.0.0 2010-03-20 add option include comment
"						  delete option only .c file
"	Ver1.5.0.0 2010-08-11 add MAC version, add .m/.mm file
"	Ver1.6.0.0 2011-01-12 fix bug doesn't operate in capital letter extension
"	Ver1.7.0.0 2011-02-02 fix bug doesn't operate in option including comment
"	Ver1.7.1.0 2011-02-10 create pgrep.vim and fix up usage 
"	Ver1.7.2.0 2011-02-10 fix bug in pgrep.vim
"   Ver1.7.3.0 2011-10-15 improve help and add makefile for Unix
"   Ver2.0.0.0 2012-10-18 rename pgrep -> smartgrep
"   Ver2.1.0.0 2012-10-19 support .cs (CSharp), .js (Java Script)
"	Ver2.2.0.0 2013-01-09 support #if 0 comment.
"						  mofify option identifier '/' to '-'.
"   Ver2.3.0.0 2013-01-26 modify viriable name g:base_dir to g:smartgrep_basedir.
"						  modify makefile for linux environment.
"						  fix bug in case of #ifdef and #ifndef.
"   Ver2.4.0.0 2013-02-27 add command with newtab.
"	Ver2.5.0.0 2013-05-02 support .rb(ruby) and one line comment.
"   Ver2.6.0.0 2013-05-03 support .py(python), python comment is not supported.
"   Ver2.7.0.0 2013-05-06 support python multi-line comment
"   Ver2.8.0.0 2013-05-19 support .java and .go
"   Ver2.9.0.0 2013-05-24 support .coffee and .scala( not support nest )
"   Ver3.0.0.0 2013-05-28 auto project detect feature using .git and .hg folder.
"   Ver3.1.0.0 2013-05-29 support ruby multi-line comment, perl, visual basic.
"   Ver3.2.0.0 2013-07-19 bug fix in case of using .git repogitory.
"   Ver3.3.0.0 2013-08-29 add auto repogitory detect feature.
"                         delete optional feature.
"   Ver3.4.0.0 2013-11-17 set auto repogitory detect for default.
"   Ver3.4.1.0 2013-11-21 amend wrong flag setting.
"   Ver3.5.0.0 2013-11-30 add git grep wrapper.
"   Ver3.6.0.0 2013-12-01 add ag wrapper.
"   Ver3.7.0.0 2014-03-23 add --ignore-dir option.
"   Ver3.7.1.0 2014-04-06 multiple --ignore-dir option.
"   Ver3.7.2.0 2014-04-09 add g:smartgrep_user_option variable.
"   Ver3.7.3.0 2014-04-27 add .erb, remove git-grep
"   Ver3.7.4.0 2014-05-16 add support .plist/.strings/.pbxproj
"   Ver3.7.5.0 2014-06-03 add support .swift
"   Ver3.7.6.0 2014-06-19 add suppoer .storyboard
"   Ver3.7.7.0 2014-10-13 add support .vim
"   Ver3.7.8.0 2014-12-06 amend .vim
"   Ver3.7.9.0 2015-01-16 add support .html/.css
"   Ver3.8.0.0 2015-01-22 add case insensitive option
"   Ver3.8.1.0 2015-03-27 modify .css support, add support .scss
"   Ver3.8.2.0 2015-04-01 add jvgrep wrapper.
"   Ver3.8.3.0 2015-04-01 support utf16le with bom
"   Ver3.8.4.0 2015-04-05 remove g:smartgrep_basedir
"   Ver3.8.5.0 2015-04-11 add Pro*C
"
" License
"   The MIT License
"
" Support OS
"	Windows/Unix/MacOSX
"
" Support File Extention                            Support Comment Type
"	C++ 		: .c .cpp .cxx .tli .inc .rc 		/* */, //, #if 0 
"   C++ heder	: .h .hpp .hxx .tlh .inl			/* */, //, #if 0
"	Objective-C : .m .mm							/* */, //, #if 0
"   Pro*C       : .pc                               /* */, //, #if 0
"	Swift       : .swift                            /* */, //
"   Xcode       : .plist .strings .pbxproj .storyboard 
"                                                   /* */, //
"   CSharp      : .cs								/* */, //
"	JavaScript  : .js								/* */, //
"   Java        : .java                             /* */, //
"   Scala       : .scala                            /* */(can't nest), //
"   Go          : .go                               /* */, //
"   scss        : .scss                             /* */, //
"   css         : .css                              /* */
"   Ruby        : .rb                               #, =begin, =end
"   Perl        : .pl                               #, =pod, =cut
"	Python		: .py								#, three double(single) quotation
"   Coffee      : .coffee                           #, ###
"   VBNet       : .vb                               '
"   VB6         : .bas .frm .cls                    '
"   Vim         : .vim                              "
"   AS IS       : .erb .html                        nothing
"
" Support File Encoding
"   UTF16LE with BOM for C# and xcode resource file.
"   UTF8 for other file.
" 
" How To Install
"	Put smartgrep.exe or smartgrep in C:\windows\system\ or pathed directory.
"	Put smartgrep.vim in vim plugin directory
"		example:
"			C:\vim\runtime\plugin\		(for windows)
"			~/.vim/plugin/				(for Unix)
"
"   If g:smartgrep_no_detectrepo is not defined
"   the git or mercurial repogitory detected by the current directory is used.
"   If repogitory is not detected, it is the same of the case
"   g:smartgrep_no_detectrepo isn't defined. 
"		example:
"		    let g:smartgrep_no_detectrepo=1 (don't use git or mercurial auto repo detect.)
"
"	If g:smartgrep_user_option is defined, it is used for smartgrep binary option.
"
"	If you use MacVim, add smartgrep path to .vimrc file.
"		example:
"	        let $PATH .= ':~/bin'
"
" Implementation below

function! RSmartHilight(word)
  silent! execute "ij " . a:word
  normal*
  redraw!
endfunction

if !exists('g:smartgrep_user_option')
  let g:smartgrep_user_option=''
endif

function! RSmartGrepEWG(word)
  if exists("g:smartgrep_no_detectrepo")
    set grepprg=smartgrep\ -ew
  else
    set grepprg=smartgrep\ -ew\ -g
  endif
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

function! RSmartGrepEG(word)
  if exists("g:smartgrep_no_detectrepo")
    set grepprg=smartgrep\ -e
  else
    set grepprg=smartgrep\ -e\ -g
  endif
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction


function! RSmartGrepHWG(word)
  if exists("g:smartgrep_no_detectrepo") 
    set grepprg=smartgrep\ -hw
  else
    set grepprg=smartgrep\ -hw\ -g
  endif
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

function! RSmartGrepIG(word)
  if exists("g:smartgrep_no_detectrepo")
    set grepprg=smartgrep\ -i
  else
    set grepprg=smartgrep\ -i\ -g
  endif
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

function! RSmartGrepCG(word)
  if exists("g:smartgrep_no_detectrepo")
    set grepprg=smartgrep\ -c
  else
    set grepprg=smartgrep\ -c\ -g
  endif
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

function! RSilverSearcherGrep(word)
  let s:basedir = system('git rev-parse --show-toplevel')
  let s:cmd = 'lcd ' . s:basedir
  execute s:cmd
  set grepprg=ag\ --nogroup\ --nocolor\ --column
  silent! execute "lgrep " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

function! RJvgrep(word)
  let s:basedir = system('git rev-parse --show-toplevel')
  let s:cmd = 'lcd ' . s:basedir
  execute s:cmd
  set grepprg=jvgrep
  silent! execute "lgrep " . a:word . " ."
  set grepprg=jvgrep\ --enc\ utf-16
  silent! execute "lgrepadd " . a:word . " ."
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

if !exists('g:smartgrep_no_default_key_mappings')
  " ,g  : recursive word grep for supported files exclude comment by mouse cursored word 
  " ,h  : recursive word grep for h file exclude comment by mouse cursored word
  " ,i  : recursive grep for supported files include comment
  " ,u  : git grep by mouse cursored word
  " ,s  : ag by mouse cursored word
  " ,k  : jvgrep by ouse cursored word
  noremap ,g :call RSmartGrepEWG("<C-R><C-W>")<CR>
  noremap ,h :call RSmartGrepHWG("<C-R><C-W>")<CR>
  noremap ,i :call RSmartGrepIG("<C-R><C-W>")<CR>
  noremap ,s :call RSilverSearcherGrep("<C-R><C-W>")<CR>
  noremap ,k :call RJvgrep("<C-R><C-W>")<CR>
endif

if !exists('g:smartgrep_no_default_key_mappings')
  " :R  -> recursive word grep for supported files exclude comment
  " :Rg -> recursive grep for supported files exclude comment
  " :Rh -> recursive word grep for h file exclude comment
  " :Ri -> recursive grep for supported files include comment
  " :Rc -> recursive case insensitive grep for supported files include comment
  " :Rs -> ag
  " :Rk -> jvgrep
  command! -nargs=1 -complete=file R call RSmartGrepEWG("<args>")
  command! -nargs=1 -complete=file Rg call RSmartGrepEG("<args>")
  command! -nargs=1 -complete=file Rh call RSmartGrepHWG("<args>")
  command! -nargs=1 -complete=file Ri call RSmartGrepIG("<args>")
  command! -nargs=1 -complete=file Rc call RSmartGrepCG("<args>")
  command! -nargs=1 -complete=file Rs call RSilverSearcherGrep("<args>")
  command! -nargs=1 -complete=file Rk call RJvgrep("<args>")
endif
