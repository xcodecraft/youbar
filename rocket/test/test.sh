adirname() { odir=`pwd`; cd `dirname $1`; pwd; cd "${odir}"; }
MYDIR=`adirname "$0"`
export MODULE_PATH=$MYDIR/../modules
export LD_LIBRARY_PATH=$MODULE_PATH:/usr/local/lib:$LD_LIBRARY_PATH
RG=/home/q/tools/pylon_rigger/rigger
#python2.7 $HOME/devspace/hydra/test/main_test.py  data_tc
sudo $RG phpunit -f test   -s test
