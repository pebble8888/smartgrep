"
" Filename : pgrep.vim 
" Brief    : This file is vim plugin
" Function : grep h/c/cpp file excluding comment 
" Author   : pebble7777 <pebble@abelia.ocn.ne.jp> 2009-2011 Copyright
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
"
" How To Install
"	Put pgrep.exe or pgrep in C:\windows\system32\ or pathed directory.
"	Put pgrep.vim in vim plugin directory
"		example:
"			C:\vim7\runtime\plugin\		(for windows)
"			~/.vim/plugin/				(for Unix)
"
" 	Define g:base_dir in your .vimrc file.
" 	    example:
" 			let g:base_dir="c:\\develop" (for windows)
"			let g:base_dir="/develop/"	 (for Unix)
"
"	If you use MacVim, add pgrep path to .vimrc file.
"		example:
"	        let $PATH .= ':~/bin'
"
"	Optional 
"	Define s:sys_dir_w and s:sys_dir_l in your .vimrc file.
"		example:
" 			let g:sys_dir_w="c:\\WinDDK"			(for windows)
" 			let g:sys_dir_l="c:\\linux\\include"	(for windows)
"
" Cover OS
"	Windows/Unix
"
" Cover File Extention
"	C++ 		: *.c *.cpp *.cxx *.tli *.inc *.rc 
"   C++ heder	: *.h *.hpp *.hxx *.tlh *.inl
"	Objective-C : *.m *.mm
" 
" Implementation below

function! RPGrepNW(word)
  set grepprg=pgrep\ /nw
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RPGrepHW(word)
  set grepprg=pgrep\ /hw
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RPGrepW(word)
  set grepprg=pgrep\ /bw
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RPGrepN(word)
  set grepprg=pgrep\ /n
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RPGrepH(word)
  set grepprg=pgrep\ /h
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RPGrep(word)
  set grepprg=pgrep\ /b
  execute "cd " . g:base_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RPGrepWA(word)
  set grepprg=pgrep\ /bw
  execute "cd " . g:sys_dir
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RPGrepWW(word)
  set grepprg=pgrep\ /bw
  execute "cd " . g:sys_dir_w
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

function! RPGrepWL(word)
  set grepprg=pgrep\ /bw
  execute "cd " . g:sys_dir_l
  execute "lgrep " . a:word
  lopen
  set grepprg&
endfunction

" ,g : recursive word grep for c,h file exclude comment by mouse cursored word 
" ,h : recursive word grep for h file exclude comment by mouse cursored word
noremap ,g :call RPGrepW("<C-R><C-W>")<CR>
noremap ,h :call RPGrepHW("<C-R><C-W>")<CR>

" :Rn  -> recursive word grep for c,h file include comment
" :Rh  -> recursive word grep for h file exclude comment
" :R   -> recursive word grep for c,h file exclude comment
" :Rno -> recursive grep for c,h file include comment
" :Rho -> recursive grep for h file exclude comment
" :Ro  -> recursive grep for c,h file exclude comment
command! -nargs=1 -complete=file Rn call RPGrepNW("<args>")
command! -nargs=1 -complete=file Rh call RPGrepHW("<args>")
command! -nargs=1 -complete=file R call RPGrepW("<args>")
command! -nargs=1 -complete=file Rno call RPGrepN("<args>")
command! -nargs=1 -complete=file Rho call RPGrepH("<args>")
command! -nargs=1 -complete=file Ro call RPGrep("<args>")

" :Rl  -> recursive word grep for c,h file exclude comment in sys_dir_w
" :Rw  -> recursive word grep for c,h file exclude comment in sys_dir_l
command! -nargs=1 -complete=file Rl call RPGrepWL("<args>")
command! -nargs=1 -complete=file Rw call RPGrepWW("<args>")

