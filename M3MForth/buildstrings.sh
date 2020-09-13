#sh
# $1 is log for for logging build data
echo >> $1
echo 'running buildStrings.sh' >> $1
make 2>&1 >> $1

