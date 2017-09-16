export PATH=.:/sbin:/usr/sbin:/usr/local/sbin:/usr/local/bin:/bin:/usr/bin:/usr/local/bin
SCI_PATH=`dirname $0`
CUR_PATH=$SCI_PATH

root=$CUR_PATH/../
tmp=$root/tmp/
cd $root
rm -rf cscope.*
#[-d "$tmp"] || mkdir $root/tmp/
csfile=$root/_prj/cscope.files
touch $root/_prj/cscope.files
find $root/ -name "*.php" >  $csfile
find $root/ -name "*.py" >  $csfile
find $root/ -name "*.h" >>   $csfile
find $root/ -name "*.cpp" >>  $csfile
find $root/ -name "*.hpp" >>  $csfile
cd $CUR_PATH/
cscope -b  -i cscope.files  ;
