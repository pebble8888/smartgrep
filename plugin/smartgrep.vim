"
" Filename : smartgrep.vim 
" Brief    : This file is vim plugin
" Function : grep source file excluding comment and wrapper for major grep command
" Author   : pebble8888@gmail.com 2009-2015 Copyright
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
  silent! execute "cd " . '.'
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
  silent! execute "cd " . '.'
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
  silent! execute "cd " . '.'
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
  silent! execute "cd " . '.'
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
  silent! execute "cd " . '.'
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
  " <Leader>g  : recursive word grep for supported files exclude comment by mouse cursored word 
  " <Leader>h  : recursive word grep for h file exclude comment by mouse cursored word
  " <Leader>i  : recursive grep for supported files include comment
  " <Leader>u  : git grep by mouse cursored word
  " <Leader>s  : ag by mouse cursored word
  " <Leader>l  : jvgrep by ouse cursored word
  noremap <Leader>g :call RSmartGrepEWG("<C-R><C-W>")<CR>
  noremap <Leader>h :call RSmartGrepHWG("<C-R><C-W>")<CR>
  noremap <Leader>i :call RSmartGrepIG("<C-R><C-W>")<CR>
  noremap <Leader>s :call RSilverSearcherGrep("<C-R><C-W>")<CR>
  noremap <Leader>l :call RJvgrep("<C-R><C-W>")<CR>
endif

if !exists('g:smartgrep_no_default_key_mappings')
  " :R  -> recursive word grep for supported files exclude comment
  " :Rg -> recursive grep for supported files exclude comment
  " :Rh -> recursive word grep for h file exclude comment
  " :Ri -> recursive grep for supported files include comment
  " :Rc -> recursive case insensitive grep for supported files include comment
  " :Rs -> ag
  " :Rl -> jvgrep
  command! -nargs=1 -complete=file R call RSmartGrepEWG("<args>")
  command! -nargs=1 -complete=file Rg call RSmartGrepEG("<args>")
  command! -nargs=1 -complete=file Rh call RSmartGrepHWG("<args>")
  command! -nargs=1 -complete=file Ri call RSmartGrepIG("<args>")
  command! -nargs=1 -complete=file Rc call RSmartGrepCG("<args>")
  command! -nargs=1 -complete=file Rs call RSilverSearcherGrep("<args>")
  command! -nargs=1 -complete=file Rl call RJvgrep("<args>")
endif
