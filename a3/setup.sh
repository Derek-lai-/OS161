VOLNAME="FSVOL"

# compile the system, compile the user programs
compile.sh -u

# setup an SFS file system on one disk
cd ~/csc369/root
./hostbin/host-mksfs DISK1.img $VOLNAME

sys161 kernel "mount sfs lhd0"
