In order to make sure you can compile the system, run the following commands

```bash
chmod u+x ./configure
./configure
```

and then

```bash
cd kern/conf
chmod u+x config newvers.sh
```

and then

```bash
cd ../..
chmod u+x compile.sh
./compile.sh -nr
```

After this you may have to add permissions to some files in the compile script, check for errors in the output to make sure

### Options for the compile script

There are two build types, NORAND and RAND (use -nr or -r in the compile script to pick them).
You can also choose to run the system after, by adding the -s option.
You can also use the -w option to do debugging if you desire.
```bash
./compile.sh -nr #builds NORAND
./compile.sh -nr -s #builds NORAND and runs sys161 from ~/csc369/root
./compile.sh -nr -s -w #similar to above, but runs sys161 -w for debugging
```
