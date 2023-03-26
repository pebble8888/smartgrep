"
" Filename : smartgrep.vim 
" Brief    : This file is vim plugin
" Function : grep source file excluding comment and wrapper for major grep command
" Author   : pebble8888@gmail.com 2009-2023 Copyright
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

" exclude comment
function! RSmartGrepE(word)
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

" exclude comment word
function! RSmartGrepEW(word)
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

" exclude comment case insensitive
function! RSmartGrepEC(word)
  if exists("g:smartgrep_no_detectrepo")
    set grepprg=smartgrep\ -ec
  else
    set grepprg=smartgrep\ -ec\ -g
  endif
  silent! execute "cd " . '.'
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

" .h exclude comment
function! RSmartGrepHW(word)
  if exists("g:smartgrep_no_detectrepo") 
    set grepprg=smartgrep\ -h
  else
    set grepprg=smartgrep\ -h\ -g
  endif
  silent! execute "cd " . '.'
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

" .h exclude comment word
function! RSmartGrepHW(word)
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

" .h exclude comment word
function! RSmartGrepHC(word)
  if exists("g:smartgrep_no_detectrepo") 
    set grepprg=smartgrep\ -hc
  else
    set grepprg=smartgrep\ -hc\ -g
  endif
  silent! execute "cd " . '.'
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

" including comment
function! RSmartGrepI(word)
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

" including comment word
function! RSmartGrepI(word)
  if exists("g:smartgrep_no_detectrepo")
    set grepprg=smartgrep\ -iw
  else
    set grepprg=smartgrep\ -iw\ -g
  endif
  silent! execute "cd " . '.'
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

" including comment case insensitive
function! RSmartGrepIC(word)
  if exists("g:smartgrep_no_detectrepo")
    set grepprg=smartgrep\ -ic
  else
    set grepprg=smartgrep\ -ic\ -g
  endif
  silent! execute "cd " . '.'
  silent! execute "lgrep " . g:smartgrep_user_option . " " . a:word
  silent! lopen
  set grepprg&
  call RSmartHilight(a:word)
endfunction

" silver searcher
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

" jvgrep
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

" find
function! RFind(word)
  let s:basedir = system('git rev-parse --show-toplevel')
  let s:cmd = 'lcd ' . s:basedir
  execute s:cmd
  set grepprg=find\ .\ -type\ f\ -name
  set grepformat=%f
  silent! execute "lgrep *" . a:word . "*"
  silent! lopen
  set grepprg&
  set grepformat&
endfunction

if !exists('g:smartgrep_no_default_key_mappings')
  " <Leader>g  : recursive word grep for supported files exclude comment by mouse cursored word 
  " <Leader>h  : recursive word grep for h file exclude comment by mouse cursored word
  noremap <Leader>g :call RSmartGrepEW("<C-R><C-W>")<CR>
  noremap <Leader>h :call RSmartGrepHW("<C-R><C-W>")<CR>
endif

if !exists('g:smartgrep_no_default_key_mappings')
  " :R  -> recursive word grep for supported files exclude comment
  " :Rh -> recursive word grep for h file exclude comment
  " :Ri -> recursive grep for supported files include comment
  " :Rc -> recursive case insensitive grep for supported files include comment
  " :Rd -> recursive case insensitive grep for supported files exclude comment
  command! -nargs=1 -complete=file R call RSmartGrepEW("<args>")
  command! -nargs=1 -complete=file Rh call RSmartGrepHW("<args>")
  command! -nargs=1 -complete=file Ri call RSmartGrepI("<args>")
  command! -nargs=1 -complete=file Rc call RSmartGrepIC("<args>")
  command! -nargs=1 -complete=file Rd call RSmartGrepEC("<args>")
endif
