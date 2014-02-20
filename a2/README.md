In order to make sure you can compile the system, run the following commands

chmod u+x ./configure
./configure

and then

cd kern/conf
chmod u+x config newvers.sh

and then

cd ../..
chmod u+x compile.sh
./compile.sh
