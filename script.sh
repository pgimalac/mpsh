#!/bin/zsh

rm ~/.mpshrc 2> /dev/null
PATH=$PATH:$PWD
echo "mkdir test
cd test
touch file1 file2
ls ./ not 1> file1 2> file2
cat file1 file2
rm file1 file2
ls ./ not 1> file1 2> file2
cat file1
ls | wc
rm file1 file2
echo done
exit 0
" | mpsh > file1
echo "~/.mpshrc not found.
Creation of a default .mpshrc
The default content is

export CHEMIN=\$PATH
INVITE=\"[\\\u@\\h : \\W]$ \"

~/.mpshrc created.
Execution of the ~/.mpshrc script.
~/.mpshrc successfully executed.
[$USER@$HOST : mpsh]$ mkdir test
[$USER@$HOST : mpsh]$ cd test
[$USER@$HOST : test]$ touch file1 file2
[$USER@$HOST : test]$ ls ./ not 1> file1 2> file2
[$USER@$HOST : test]$ cat file1 file2
./:
file1
file2
ls: impossible d'accéder à 'not': Aucun fichier ou dossier de ce type
[$USER@$HOST : test]$ rm file1 file2
[$USER@$HOST : test]$ ls ./ not 1> file1 2> file2
[$USER@$HOST : test]$ cat file1
./:
file1
file2
[$USER@$HOST : test]$ ls | wc
      2       2      12
[$USER@$HOST : test]$ rm file1 file2
[$USER@$HOST : test]$ echo done
done
[$USER@$HOST : test]$ exit 0" > file2
rm -r test
diff -y -a --report-identical-files --color --suppress-common-lines file1 file2
rm file1 file2
