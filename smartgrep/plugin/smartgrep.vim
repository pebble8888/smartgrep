"
" Filename : smartgrep.vim 
" Brief    : This file is vim plugin
" Function : grep source file excluding comment 
" Author   : pebble8888@gmail.com 2009-2012 Copyright
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
"   Ver2.1.0.0 2012-10-19 cover .cs (CSharp), .js (Java Script)
"
" Cover OS
"	Windows/Unix
"
" Cover File Extention                                  Cover Comment Type
"	C++ 		: *.c *.cpp *.cxx *.tli *.inc *.rc 		/* */, // 
"   C++ heder	: *.h *.hpp *.hxx *.tlh *.inl			/* */, //
"	Objective-C : *.m *.mm								/* */, //
"   CSharp      : *.cs									/* */, //
"	JavaScript  : *.js									/* */, //
" 
" How To Install
"	Put smartgrep.exe or smartgrep in C:\windows\system32\ or pathed directory.
"	Put smartgrep.vim in vim plugin directory
"		example:
"			C:\vim7\runtime\plugin\		(for windows)
"			~/.vim/plugin/				(for Unix)
"
" 	Define g:base_dir in your .vimrc file.
" 	    example:
" 			let g:base_dir="c:\\develop" (for windows)
"			let g:base_dir="/develop/"	 (for Unix)
"
"	If you use MacVim, add smartgrep path to .vimrc file.
"		example:
"	        let $PATH .= ':~/bin'
"
"	Optional 
"	Define s:sys_dir_w and s:sys_dir_l in your .vimrc file.
"		example:
" 			let g:sys_dir_w="c:\\WinDDK"			(for windows)
" 			let g:sys_dir_l="c:\\linux\\include"	(for windows)
"
" Implementation below

function! RSmartGrepNW(word)
  set grepprg=smartgrep\ /nw
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RSmartGrepHW(word)
  set grepprg=smartgrep\ /hw
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RSmartGrepW(word)
  set grepprg=smartgrep\ /bw
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RSmartGrepN(word)
  set grepprg=smartgrep\ /n
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RSmartGrepH(word)
  set grepprg=smartgrep\ /h
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RSmartGrep(word)
  set grepprg=smartgrep\ /b
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RSmartGrepWA(word)
  set grepprg=smartgrep\ /bw
  execute "cd " . g:sys_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RSmartGrepWW(word)
  set grepprg=smartgrep\ /bw
  execute "cd " . g:sys_dir_w
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RSmartGrepWL(word)
  set grepprg=smartgrep\ /bw
  execute "cd " . g:sys_dir_l
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

" ,g : recursive word grep for c,h file exclude comment by mouse cursored word 
" ,h : recursive word grep for h file exclude comment by mouse cursored word
noremap ,g :call RSmartGrepW("<C-R><C-W>")<CR>
noremap ,h :call RSmartGrepHW("<C-R><C-W>")<CR>

" :Rn  -> recursive word grep for c,h file include comment
" :Rh  -> recursive word grep for h file exclude comment
" :R   -> recursive word grep for c,h file exclude comment
" :Rno -> recursive grep for c,h file include comment
" :Rho -> recursive grep for h file exclude comment
" :Ro  -> recursive grep for c,h file exclude comment
command! -nargs=1 -complete=file Rn call RSmartGrepNW("<args>")
command! -nargs=1 -complete=file Rh call RSmartGrepHW("<args>")
command! -nargs=1 -complete=file R call RSmartGrepW("<args>")
command! -nargs=1 -complete=file Rno call RSmartGrepN("<args>")
command! -nargs=1 -complete=file Rho call RSmartGrepH("<args>")
command! -nargs=1 -complete=file Ro call RSmartGrep("<args>")

" :Rl  -> recursive word grep for c,h file exclude comment in sys_dir_w
" :Rw  -> recursive word grep for c,h file exclude comment in sys_dir_l
command! -nargs=1 -complete=file Rl call RSmartGrepWL("<args>")
command! -nargs=1 -complete=file Rw call RSmartGrepWW("<args>")

