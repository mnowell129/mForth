#sh
# $1 is the directory to pass to make to work in
# $2 is the make file to use
# $3 is the path to the tools directory
# $4 is the log file
# $5 (optional) is a list of file base names to apply the makesall.exe program to instead of the default makes.exe program
echo >> $4
echo "Running Stringify.sh from $1" >> $4
echo $2 >> $4
make -C $1 -f $2/Tools/StringifyTool/StringifyMake TOOLS_DIR=$3 MAKESALL_BASES="$5" 2>&1 >> $4

