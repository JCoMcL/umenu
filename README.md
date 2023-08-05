# umenu
#### *micro-menu* or *unnamed menu*
Inspired by dmenu and slmenu, but designed around single-keypress interaction. It maps each line of stdin (up to a maximum of 35 by default) to a key and outputs the selected line to stdout.
Useful for repetitive user-input tasks like categorising training data or or organising media into directories.
## Installation
run `make install` from the source directory (with root if neccesary)
no dependencies should be required

## Example usage
Navigate to a subdirectory:
```shell
cd `ls */ | umenu`
```
Navigate up and down directories
```shell
while CWD=`ls -al | awk '/^d/ {print $NF}' | umenu`; do
  echo
  cd $CWD
done
```
Move all PNGs one-by-one to some subdirectory, or to trash
```shell
mkdir trash
for PNG in `ls *.png`; do
  echo $PNG
  mv $PNG `ls -d */ | umenu`
done
trash-put trash
```
