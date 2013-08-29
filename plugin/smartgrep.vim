"
" Filename : smartgrep.vim 
" Brief    : This file is vim plugin
" Function : grep source file excluding comment 
" Author   : pebble8888@gmail.com 2009-2013 Copyright
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
"
" Support OS
"	Windows/Unix
"
" Support File Extention                            Support Comment Type
"	C++ 		: .c .cpp .cxx .tli .inc .rc 		/* */, //, #if 0 
"   C++ heder	: .h .hpp .hxx .tlh .inl			/* */, //, #if 0
"	Objective-C : .m .mm							/* */, //, #if 0
"   CSharp      : .cs								/* */, //
"	JavaScript  : .js								/* */, //
"   Java        : .java                             /* */, //
"   Scala       : .scala                            /* */(can't nest), //
"   Go          : .go                               /* */, //
"   Ruby        : .rb                               #, =begin, =end
"   Perl        : .pl                               #, =pod, =cut
"	Python		: .py								#, three double(single) quotation
"   Coffee      : .coffee                           #, ###
"   VBNet       : .vb                               '
"   VB6         : .bas .frm .cls                    '
" 
" How To Install
"	Put smartgrep.exe or smartgrep in C:\windows\system\ or pathed directory.
"	Put smartgrep.vim in vim plugin directory
"		example:
"			C:\vim\runtime\plugin\		(for windows)
"			~/.vim/plugin/				(for Unix)
"
"   If g:smartgrep_detectrepo isn't defined and
"   g:smartgrep_basedir isn't defined 
"   the current directory is used for grep base directory.
"	If you would like to usually use a fixed grep base directory,
"   define 'g:smartgrep_basedir' in your .vimrc file.
" 	    example:
" 			let g:smartgrep_basedir="c:\\develop" (for windows)
"			let g:smartgrep_basedir="/develop/"	 (for Unix)
"
"   If g:smartgrep_detectrepo is defined
"   the git or mercurial repogitory detected by the current directory is used.
"   If repogitory is not detected, it is the same of the case
"   g:smartgrep_detectrepo isn't defined. 
"		example:
"		    let g:smartgrep_detectrepo=1 (use git or mercurial auto repo detect.)
"
"	If you use MacVim, add smartgrep path to .vimrc file.
"		example:
"	        let $PATH .= ':~/bin'
"
" Implementation below

function! RSmartGrepHWG(word)
  if exists("g:smartgrep_detectrepo") 
    set grepprg=smartgrep\ -hw\ -g
  else
    set grepprg=smartgrep\ -hw
  endif
  execute "cd " . get(g:, 'smartgrep_basedir', '.')
  silent! execute "lgrep " . a:word
  silent! lopen
  set grepprg&
endfunction

function! RSmartGrepEWG(word)
  if exists("g:smartgrep_detectrepo")
    set grepprg=smartgrep\ -ew\ -g
  else
    set grepprg=smartgrep\ -ew
  endif
  execute "cd " . get(g:, 'smartgrep_basedir', '.')
  silent! execute "lgrep " . a:word
  silent! lopen
  set grepprg&
endfunction

function! RSmartGrepIG(word)
  if exists("g:smartgrep_detectrepo")
    set grepprg=smartgrep\ -i\ -g
  else
    set grepprg=smartgrep\ -i
  endif
  execute "cd " . get(g:, 'smartgrep_basedir', '.')
  silent! execute "lgrep " . a:word
  silent! lopen
  set grepprg&
endfunction

if !exists('g:smartgrep_no_default_key_mappings')
  " ,g  : recursive word grep for supported files exclude comment by mouse cursored word 
  " ,h  : recursive word grep for h file exclude comment by mouse cursored word
  " ,gt : tabnew version for ,g
  " ,ht : tabnew version for ,h
  noremap ,g :call RSmartGrepEWG("<C-R><C-W>")<CR>
  noremap ,h :call RSmartGrepHWG("<C-R><C-W>")<CR>
endif

if !exists('g:smartgrep_no_default_key_mappings')
  " :R  -> recursive word grep for supported files exclude comment
  " :Rh -> recursive word grep for h file exclude comment
  " :Ri -> recursive grep for supported files include comment
  command! -nargs=1 -complete=file R call RSmartGrepEWG("<args>")
  command! -nargs=1 -complete=file Rh call RSmartGrepHWG("<args>")
  command! -nargs=1 -complete=file Ri call RSmartGrepIG("<args>")
endif
