export HOME=~

export PGDATA=/usr/local/var/postgres

# ClaimLynx laptop dev environment
export LYNXTREE=~/lynx

# WSO2 Web Services Framework WSF/C++
export WSFCPP_HOME=/usr/local/wsf
export WSF=$WSFCPP_HOME/bin
# not a happy way of doing this:
#export DYLD_LIBRARY_PATH=/usr/local/wsf/lib

# Maven junk
export M2_HOME=/usr/local/apache-maven/apache-maven-3.0.4
export M2=$M2_home/bin

export PATH="$HOME/bin:/usr/local/bin:/usr/bin:/bin:/sbin:/usr/sbin:$M2:$WSF:."
if [ -d $LYNXTREE/bin ] ; then
    # wtf bash?
    BAR=$LYNXTREE/bin
    export PATH="${PATH}:${BAR}"
fi

export CDPATH=".:~:~/disk2:~/work:~/lynx"

unset MAILCHECK

# Make Vim the command line editor, not Emacs!
set -o vi

# forcibly enable command line filename completion on tab
bind '"\t":menu-complete' 

# Do not exit on a single control-D
export IGNOREEOF=1

export EDITOR='vim'     # in very different places on Mac, FreeBSD, etc.
export PAGER='/usr/bin/less'
export GIT_PAGER='/usr/bin/less'
export BLOCKSIZE='1024'
export CVS_RSH='ssh'
export JAVA_HOME='/Library/Java/Home'
export LESS='-X -R'
export TAR_OPTIONS='--no-same-owner -m'

export LSCOLORS='CxgxfxfxDxfxfxDeDeCeCe'
export LS_COLORS='di=1;32:ln=0;36:so=0;35:pi=0;35:ex=1;33:bd=0;35:cd=0;35'

export TMPDIR=$HOME/tmp

# In theory, this file won't ever be called for non-interactive shells, but
# just in case, here's how to tell and do something about it, using the 
# special '-' paramter.
# case "$-" in
# *i*)    echo This shell is interactive. ;;
# *)      echo This shell is NOT interactive. ;;
# esac
# another way is to check PS1 which is only set in interactive shells.
if [ -z "$PS1" ]; then
    echo This shell is NOT interactive.
else
    # Set XTerm title bar
    # echo may cause inbound git using ssh to fail
    echo -ne "\033]0;${USER}@${HOSTNAME%%.*}\007"

    # echo This shell is interactive.
    # Bash 3 doesn't support path shortening (ellipses)
    export PS1="\! \h:\w\$ "
    export PS1='\[\e[0;31m\]\! \h:\w\$ \[\e[0m\]'
    # . ~/ellipses.bash
    # export PS1='\! \h:$(_dir_chomp $(pwd) 20)$ '
fi


# aliases

alias b='git branch'

alias cp='cp -i'
alias drush='php -d error_reporting=E_ALL^E_NOTICE ~/bin/drush/drush.php'
alias j='jobs -l'
alias l1='ls -1'
alias l='ls -lartG'
alias la='ls -a'
alias ls='ls -FG '
alias mv='mv -i'
alias po='popd'
alias pu='pushd'
alias pwd='echo $PWD'
alias rm='rm -i'
alias vi='vim'
#alias view='vim -R'

# cd ()   { command cd "$@";echo $PWD ; }
d ()    { command tree -d -L 1 -aFC "$@" ; }
h ()    { command history "$@" | less ; }
ht ()   { command history "$@" | tail ; }
lt ()   { command ls -lartG "$@" | tail ; }
t ()    { command tree -L 4 -aFC "$@" ; }
tree () { command tree -L 4 -aFC "$@" ; }
f ()    { command find . -name "$@" ; }

OS=`uname -s`
#if test "$OS" = "Darwin"
#then
    # Mac OS
#fi
