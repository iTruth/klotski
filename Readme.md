# klotski game
## Usage
> klotski [-x N] [-y N] [-p] [-u N] [-e [situation]] [-s] [-q] [-b] [-f file] [-s [-o file]] [-r] [-h]

use klotski -h for more detail  

### Example:
upset 3x3 board 20 times and play:
> klotski -p -u 20 -b

edit 3x3 board and search the answer:
> klotski -s -e -b   
> klotski -s -e1,0,6,7,3,8,2,4,5 -b

upset 5x6 board for 30 times and search the answer, output the answer to ans.txt:
> klotski -x 5 -y 6 -u 30 -s -o ans.txt

init 3x3 board and upset for 3 times, and then search the answer:
> klotski -p -e1,2,3,4,5,6,7,8,0 -u 3 -s

init 2x2 board and search the answer:
> klotski -x 2 -y 2 -e1,0,3,2 -s -b


## klotski command line
print board:
> klotski>print   
> klotski>p

quit the game:
> klotski>quit   
> klotski>q

search answer:
> klotski>search   
> klotski>s

reset board:
> klotski>reset   
> klotski>r

move block(enter coord):
> klotski>0 0  
> klotski>1 2

exec command(enter system command):
> klotski>echo hi

## Build
```sh
git clone https://github.com/iTruth/klotski
cd klotski
make
```
