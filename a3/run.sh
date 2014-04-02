VOLNAME="FSVOL"
cd ~/csc369/root
./hostbin/host-mksfs DISK1.img $VOLNAME

sys161 kernel "mount sfs lhd0; fs6 $VOLNAME"
