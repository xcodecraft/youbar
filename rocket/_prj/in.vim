set nowrap
set encoding=utf-8
set fileencoding=utf-8
set background=dark
set nu
set makeprg=$HOME/devspace/rocket/build.sh
noremap <F3> <Esc>:! $HOME/devspace/rocket/test/test.sh <CR>
noremap <F4> <Esc>:! $HOME/devspace/rocket/test/test_reducer.sh <CR>
call Probe_ide_init("$HOME/devspace/rocket")
au! BufRead,BufNewFile *.html setfiletype php


