#sh
# $1 is the log file to log build data to
echo >> $1
echo 'Running buildmakeString.sh' >> $1
make 2>&1 >> $1

